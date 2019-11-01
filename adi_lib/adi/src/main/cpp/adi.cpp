#include <jni.h>
#include <string>
#include "jvmti.h"
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <signal.h>
#include "handler/VMObjectAllocHandler.h"
#include "handler/MethodEntryHandler.h"
#include "handler/ThreadStartHandler.h"
#include "handler/GCCallbackHandler.h"
#include "handler/ObjectFreeHandler.h"
#include "handler/MonitorContendedHandler.h"
#include "handler/Config.h"
#include "common/jdi_native.h"
#include "common/log.h"
#include "handler/ClassFileLoadHookHandler.h"
#include "handler/ClassPrepareHandler.h"
#include "handler/FramePopHandler.h"

extern "C" {
#include "dumper.h"
}

static jvmtiEnv *jvmti_env;

/**
 * 打印支持的能力，仅调试用~
 * @param caps
 */
void printAllCapabilities(jvmtiCapabilities caps) {
    ALOGI("----------->> can: %s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d,%s:%d",
          "1", caps.can_tag_objects,
          "2", caps.can_generate_field_modification_events,
          "3", caps.can_generate_field_access_events,
          "4", caps.can_get_bytecodes,
          "5", caps.can_get_synthetic_attribute,
          "6", caps.can_get_owned_monitor_info,
          "7", caps.can_get_current_contended_monitor,
          "8", caps.can_get_monitor_info,
          "9", caps.can_pop_frame,
          "10", caps.can_redefine_classes,
          "11", caps.can_signal_thread,
          "12", caps.can_get_source_file_name,
          "13", caps.can_get_line_numbers,
          "14", caps.can_get_source_debug_extension,
          "15", caps.can_access_local_variables,
          "16", caps.can_maintain_original_method_order,
          "17", caps.can_generate_single_step_events,
          "18", caps.can_generate_exception_events,
          "19", caps.can_generate_frame_pop_events,
          "20", caps.can_generate_breakpoint_events,
          "21", caps.can_suspend,
          "22", caps.can_redefine_any_class,
          "23", caps.can_get_current_thread_cpu_time,
          "24", caps.can_get_thread_cpu_time,
          "25", caps.can_generate_method_entry_events,
          "26", caps.can_generate_method_exit_events,
          "27", caps.can_generate_all_class_hook_events,
          "28", caps.can_generate_compiled_method_load_events,
          "29", caps.can_generate_monitor_events,
          "30", caps.can_generate_vm_object_alloc_events,
          "31", caps.can_generate_native_method_bind_events,
          "32", caps.can_generate_garbage_collection_events,
          "33", caps.can_generate_object_free_events,
          "34", caps.can_force_early_return,
          "35", caps.can_get_owned_monitor_stack_depth_info,
          "36", caps.can_get_constant_pool,
          "37", caps.can_set_native_method_prefix,
          "38", caps.can_retransform_classes,
          "39", caps.can_retransform_any_class,
          "40", caps.can_generate_resource_exhaustion_heap_events,
          "41", caps.can_generate_resource_exhaustion_threads_events);
}

void SetAllCapabilities(jvmtiEnv *jvmti) {
    jvmtiCapabilities caps;
    jvmtiError error;
    error = jvmti->GetPotentialCapabilities(&caps);
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("Error on GetPotentialCapabilities: %d", error);
    }
//    printAllCapabilities(caps);
    error = jvmti->AddCapabilities(&caps);
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("Error on AddCapabilities: %d", error);
    }
}

void SetEventNotification(jvmtiEnv *jvmti, jvmtiEventMode mode,
                          jvmtiEvent event_type) {
    jvmtiError err = jvmti->SetEventNotificationMode(mode, event_type, nullptr);
    if (err != JVMTI_ERROR_NONE) {
        ALOGI("Error on SetEventNotification: %d", err);
    }
}

bool isNativeBinded = false;

void JNICALL
JvmTINativeMethodBind(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jmethodID method,
                      void *address, void **new_address_ptr) {
    if (isNativeBinded) {
        return;
    }
    isNativeBinded = true;
    ALOGI("===========NativeMethodBind===============");

    jclass clazz = jni_env->FindClass("com/adi/ADIManager");
    //绑定 package code 到BootClassLoader 里
    jfieldID packageCodePathId = jni_env->GetStaticFieldID(clazz, "packageCodePath",
                                                           "Ljava/lang/String;");
    jstring packageCodePath = static_cast<jstring>(jni_env->GetStaticObjectField(clazz,
                                                                                 packageCodePathId));
    const char *pathChar = jni_env->GetStringUTFChars(packageCodePath, JNI_FALSE);
    ALOGI("===========add to boot classloader %s===============", pathChar);
    jvmti_env->AddToBootstrapClassLoaderSearch(pathChar);
    jni_env->ReleaseStringUTFChars(packageCodePath, pathChar);

}

void ignoreHandler(int sig) { ALOGI("!!!!!-> %d", sig); }

extern "C" JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options, void *reserved) {
    signal(SIGTRAP, ignoreHandler);

    jvmti_env = getJvmtiEnv(vm);

    if (jvmti_env == nullptr) {
        return JNI_ERR;
    }
    SetAllCapabilities(jvmti_env);

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));

    //TODO NativeMethodBind 比较特殊，需要时注意
//    callbacks.NativeMethodBind = &JvmTINativeMethodBind;
    // 不知为何会发 调试信号 导致应用终止
    // callbacks.MethodEntry = &MethodEntry;
    callbacks.ClassPrepare = &ClassPrepare;
    callbacks.ClassFileLoadHook = &ClassFileLoadHook;
    callbacks.VMObjectAlloc = &ObjectAllocCallback;
    callbacks.GarbageCollectionStart = &GCStartCallback;
    callbacks.GarbageCollectionFinish = &GCFinishCallback;
    callbacks.ObjectFree = &ObjectFree;
    callbacks.ThreadStart = &ThreadStart;
    callbacks.MonitorContendedEnter = &MonitorContendedEnter;
    callbacks.MonitorContendedEntered = &MonitorContendedEntered;
    int error = jvmti_env->SetEventCallbacks(&callbacks, sizeof(callbacks));
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("Error on Agent_OnAttach: %d", error);
    }

    ALOGI("==========Agent_OnAttach=======");
    return JNI_OK;

}

extern "C" {
#include "common/utils.h"
}

extern "C" JNIEXPORT void JNICALL startDump(JNIEnv *env, jclass jclazz, jstring dumpDir) {
    char *dumpDirChar = const_cast<char *>(env->GetStringUTFChars(dumpDir, JNI_FALSE));
    dumper_start(dumpDirChar);
    env->ReleaseStringUTFChars(dumpDir, dumpDirChar);
}


extern "C" JNIEXPORT void JNICALL stopDump(JNIEnv *env, jclass jclazz) {
    dumper_stop();
}

extern "C" JNIEXPORT void JNICALL disableEvents(JNIEnv *env, jclass jclazz, jintArray jevents) {
    jint len = env->GetArrayLength(jevents);
    if (len == 0) {
        return;
    }
    jint events[len];
    env->GetIntArrayRegion(jevents, 0, len, events);
    for (int i : events) {
        SetEventNotification(jvmti_env, JVMTI_DISABLE, jvmtiEvent(i));
    }
}

extern "C" JNIEXPORT void JNICALL
enableEvents(JNIEnv *env, jclass jclazz, jobject configObj, jintArray jevents) {
    jint len = env->GetArrayLength(jevents);
    if (len == 0) {
        return;
    }
    // 解析 Config
    jclass configClass = env->GetObjectClass(configObj);
    jfieldID sampleField = env->GetFieldID(configClass, "sampleIntervalMs", "F");
    jfloat sampleInterval = env->GetFloatField(configObj, sampleField);
    jfieldID stackDepthField = env->GetFieldID(configClass, "stackDepth", "I");
    jint stackDepth = env->GetIntField(configObj, stackDepthField);
    setSampleIntervalMs(sampleInterval);
    setStackDepth(stackDepth);

    jint events[len];
    env->GetIntArrayRegion(jevents, 0, len, events);
    for (int i : events) {
        SetEventNotification(jvmti_env, JVMTI_ENABLE, jvmtiEvent(i));
    }
}

//===============用于 Looper 的测试方法 start =============
extern "C" {
#include "clooper/looper_test.h"
}
extern "C" JNIEXPORT void JNICALL startLooper(JNIEnv *env, jclass jclazz) {
    test_looper_start();
}

extern "C" JNIEXPORT void JNICALL pushToLooper(JNIEnv *env, jclass jclazz, jstring data) {
    char *dataChar = const_cast<char *>(env->GetStringUTFChars(data, JNI_FALSE));
    test_looper_push(dataChar);
    env->ReleaseStringUTFChars(data, dataChar);
}

extern "C" JNIEXPORT void JNICALL stopLooper(JNIEnv *env, jclass jclazz) {
    test_looper_destroy();
}
//===============用于 Looper 的测试方法 end =============

//===========用于 retransformClasses 测试 start ========
extern "C" JNIEXPORT void JNICALL
retransformClasses(JNIEnv *env, jclass jclazz, jobjectArray jclasses) {
    jsize len = env->GetArrayLength(jclasses);
    auto *classes = (jclass *) malloc(len * sizeof(jclass));

    for (int i = 0; i < len; i++) {
        classes[i] = (jclass) env->NewGlobalRef(env->GetObjectArrayElement(jclasses, i));
    }
    jvmtiError error = jvmti_env->RetransformClasses(len, classes);
    if (error != JVMTI_ERROR_NONE) {
        ALOGI("Error on retransformClasses: %d", error);
    }
    for (int i = 0; i < len; i++) {
        env->DeleteGlobalRef(classes[i]);
    }
    free(classes);
}
//===========用于 retransformClasses 测试 end ========

static JNINativeMethod methods[] = {
        {"startDump",          "(Ljava/lang/String;)V",    (void *) startDump},
        {"stopDump",           "()V",                      (void *) stopDump},
        {"getObjectSize",      "(Ljava/lang/Object;)J",    (void *) getObjectSize},
        {"enableEvents",       "(Lcom/adi/ADIConfig;[I)V", (void *) enableEvents},
        {"disableEvents",      "([I)V",                    (void *) disableEvents},
        {"retransformClasses", "([Ljava/lang/Class;)V",    (void *) retransformClasses},
        // 用于 Looper 的测试方法
//        {"startLooperForTest",  "()V",                      (void *) startLooper},
//        {"pushToLooperForTest", "(Ljava/lang/String;)V",    (void *) pushToLooper},
//        {"stopLooperForTest",   "()V",                      (void *) stopLooper},
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    ALOGI("==============library load pid: %d====================", getpid());
    jclass clazz = env->FindClass("com/adi/ADIManager");
    env->RegisterNatives(clazz, methods, sizeof(methods) / sizeof(methods[0]));
    return JNI_VERSION_1_6;
}

