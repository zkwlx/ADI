//
// Created by kewen on 2019-10-21.
//

#include <sstream>
#include <cstring>
#include "ClassFileLoadHookHandler.h"
#include <reader.h>
#include <dex_ir.h>
#include <code_ir.h>
#include <dex_ir_builder.h>
#include <dex_utf8.h>
#include <writer.h>
#include <reader.h>
#include <instrumentation.h>
#include <dex_format.h>
#include <code_ir.h>
#include <dex_ir_builder.h>
#include <jni.h>

extern "C" {
#include "../common/log.h"
}

using namespace dex;
using namespace lir;

//================== tools start =================
/**
 * Add a label before instructionAfter
 */
static void addLabel(CodeIr &c,
                     lir::Instruction *instructionAfter,
                     Label *returnTrueLabel) {
    c.instructions.InsertBefore(instructionAfter, returnTrueLabel);
}

/**
 * Add a byte code before instructionAfter
 */
static void addInstr(CodeIr &c,
                     lir::Instruction *instructionAfter,
                     Opcode opcode,
                     const std::list<Operand *> &operands) {
    auto instruction = c.Alloc<Bytecode>();

    instruction->opcode = opcode;

    for (auto operand : operands) {
        instruction->operands.push_back(operand);
    }

    c.instructions.InsertBefore(instructionAfter, instruction);
}


static void addCall(ir::Builder &b,
                    CodeIr &c,
                    lir::Instruction *instructionAfter,
                    Opcode opcode,
                    ir::Type *type,
                    const char *methodName,
                    ir::Type *returnType,
                    const std::vector<ir::Type *> &types,
                    const std::list<int> &regs) {
    auto proto = b.GetProto(returnType, b.GetTypeList(types));
    auto method = b.GetMethodDecl(b.GetAsciiString(methodName), proto, type);

    auto *param_regs = c.Alloc<VRegList>();
    for (auto it = regs.begin(); it != regs.end(); it++) {
        param_regs->registers.push_back(*it);
    }

    addInstr(c, instructionAfter, opcode, {param_regs, c.Alloc<Method>(method,
                                                                       method->orig_index)});
}

static std::string ClassNameToDescriptor(const char *class_name) {
    std::stringstream ss;
    ss << "L";
    for (auto p = class_name; *p != '\0'; ++p) {
        ss << (*p == '.' ? '/' : *p);
    }
    ss << ";";
    return ss.str();
}

static size_t getNumParams(ir::EncodedMethod *method) {
    if (method->decl->prototype->param_types == NULL) {
        return 0;
    }

    return method->decl->prototype->param_types->types.size();
}

typedef struct {
    ir::Type *boxedType;
    ir::Type *scalarType;
    std::string unboxMethod;
} BoxingInfo;

/**
 * Get boxing / unboxing info for a type
 */
static BoxingInfo getBoxingInfo(ir::Builder &b,
                                char typeCode) {
    BoxingInfo boxingInfo;

    if (typeCode != 'L' && typeCode != '[') {
        std::stringstream tmp;
        tmp << typeCode;
        boxingInfo.scalarType = b.GetType(tmp.str().c_str());
    }

    switch (typeCode) {
        case 'B':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Byte;");
            boxingInfo.unboxMethod = "byteValue";
            break;
        case 'S':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Short;");
            boxingInfo.unboxMethod = "shortValue";
            break;
        case 'I':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Integer;");
            boxingInfo.unboxMethod = "intValue";
            break;
        case 'C':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Character;");
            boxingInfo.unboxMethod = "charValue";
            break;
        case 'F':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Float;");
            boxingInfo.unboxMethod = "floatValue";
            break;
        case 'Z':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Boolean;");
            boxingInfo.unboxMethod = "booleanValue";
            break;
        case 'J':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Long;");
            boxingInfo.unboxMethod = "longValue";
            break;
        case 'D':
            boxingInfo.boxedType = b.GetType("Ljava/lang/Double;");
            boxingInfo.unboxMethod = "doubleValue";
            break;
        default:
            // real object
            break;
    }

    return boxingInfo;
}

static bool isHashCode(ir::EncodedMethod *method) {
    return Utf8Cmp(method->decl->name->c_str(), "hashCode") == 0
           && getNumParams(method) == 0;
}

static bool isEquals(ir::EncodedMethod *method) {
    return Utf8Cmp(method->decl->name->c_str(), "equals") == 0
           && getNumParams(method) == 1
           && Utf8Cmp(method->decl->prototype->param_types->types[0]->Decl().c_str(),
                      "java.lang.Object") == 0;
}

// Throw runtime exception
static void throwRuntimeExpection(JNIEnv *env, const char *fmt, ...) {
    char msgBuf[512];

    va_list args;
    va_start (args, fmt);
    vsnprintf(msgBuf, sizeof(msgBuf), fmt, args);
    va_end (args);

    jclass exceptionClass = env->FindClass("java/lang/RuntimeException");
    env->ThrowNew(exceptionClass, msgBuf);
}

//================== tools end ==================

void JNICALL ClassFileLoadHook(jvmtiEnv *jvmti_env,
                               JNIEnv *jni_env,
                               jclass class_being_redefined,
                               jobject loader,
                               const char *name,
                               jobject protection_domain,
                               jint class_data_len,
                               const unsigned char *class_data,
                               jint *new_class_data_len,
                               unsigned char **new_class_data) {
    if (!strcmp(name, "com/adi/demo/TestObject")) {
        if (loader == nullptr) {
            ALOGI("==========bootclassloader %s, %d=============", name, class_data_len);
        }
        ALOGI("==========ClassTransform %s, %d=======", name, class_data_len);

        Reader reader(class_data, class_data_len);
        u4 index = reader.FindClassIndex(ClassNameToDescriptor(name).c_str());
        reader.CreateClassIr(index);
        std::shared_ptr<ir::DexFile> dex_ir = reader.GetIr();
        ir::Builder builder(dex_ir);

        for (auto &method : dex_ir->encoded_methods) {
            std::string type = method->decl->parent->Decl();
            ir::String *methodName = method->decl->name;
            std::string prototype = method->decl->prototype->Signature();

            ALOGI("==========ClassTransform method %s, %s=======", methodName->c_str(),
                  prototype.c_str());

            if (!strcmp("a", methodName->c_str()) && !strcmp(prototype.c_str(), "()V")) {
                ALOGI("==========Will modify %s, %s=======", methodName->c_str(),
                      prototype.c_str());
                CodeIr codeIr(method.get(), dex_ir);
                // Make sure there are at least 5 local registers to use
                int originalNumRegisters = method->code->registers - method->code->ins_count;
                int numAdditionalRegs = std::max(0, 5 - originalNumRegisters);
                int thisReg = originalNumRegisters + numAdditionalRegs;

                if (numAdditionalRegs > 0) {
                    codeIr.ir_method->code->registers += numAdditionalRegs;
                }

                ir::Type *invokerType = builder.GetType("Lcom/adi/demo/DemoObject2;");
                ir::Type *voidT = builder.GetType("V");
                ir::Type *stringT = builder.GetType("Ljava/lang/String;");

                ALOGI("2------->");
                lir::Instruction *instruction = *(codeIr.instructions.begin());
                VReg *v0 = codeIr.Alloc<VReg>(0);
//                VReg *v1 = codeIr.Alloc<VReg>(1);
//                VReg *v2 = codeIr.Alloc<VReg>(2);

                ir::String *str = builder.GetAsciiString("xxxx");
                addInstr(codeIr, instruction, OP_CONST_STRING,
                         {v0, codeIr.Alloc<String>(str, str->orig_index)});
                addCall(builder, codeIr, instruction, OP_INVOKE_STATIC, invokerType,
                        "publicStaticMethod", voidT, {stringT}, {0});
                codeIr.Assemble();

                struct Allocator : public Writer::Allocator {
                    virtual void *Allocate(size_t size) { return ::malloc(size); }

                    virtual void Free(void *ptr) { ::free(ptr); }
                };
                Allocator allocator;
                Writer writer(dex_ir);
                auto *transformed((jbyte *) writer.CreateImage(&allocator,
                                                               reinterpret_cast<size_t *>(new_class_data_len)));
                if (!jni_env->ExceptionOccurred()) {
                    jvmti_env->Allocate(*new_class_data_len, new_class_data);
                    std::memcpy(*new_class_data, transformed, *new_class_data_len);
                    break;
                } else {
                    ALOGI("---------- exception occurred!");
                }
            }
        }
    }
}