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
#include "handler/Config.h"
#include "common/jdi_native.h"
#include "common/log.h"

extern "C" {
#include "dumper.h"
}

static jvmtiEnv *jvmti_env;

static void ClassTransform(jvmtiEnv *jvmti_env,
                           JNIEnv *env,
                           jclass classBeingRedefined,
                           jobject loader,
                           const char *name,
                           jobject protectionDomain,
                           jint classDataLen,
                           const unsigned char *classData,
                           jint *newClassDataLen,
                           unsigned char **newClassData) {

    if (!strcmp(name, "android/app/Activity")) {
        if (loader == nullptr) {
            ALOGI("==========bootclassloader=============");
        }
        ALOGI("==========ClassTransform %s=======", name);
    }
}

void SetAllCapabilities(jvmtiEnv *jvmti) {
    jvmtiCapabilities caps;
    jvmtiError error;
    error = jvmti->GetPotentialCapabilities(&caps);
    error = jvmti->AddCapabilities(&caps);
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
//    callbacks.MethodEntry = &MethodEntry;
    callbacks.ClassFileLoadHook = &ClassTransform;
    callbacks.VMObjectAlloc = &ObjectAllocCallback;
    callbacks.GarbageCollectionStart = &GCStartCallback;
    callbacks.GarbageCollectionFinish = &GCFinishCallback;
    callbacks.ThreadStart = &ThreadStart;
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

//===============用于 Looper 的测试方法 =============
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

extern "C" JNIEXPORT void JNICALL
enableEvents(JNIEnv *env, jclass jclazz, jobject configObj, jintArray jevents) {
    jint len = env->GetArrayLength(jevents);
    if (len == 0) {
        return;
    }
    // 解析 Config
    jclass configClass = env->GetObjectClass(configObj);
    jfieldID filedId = env->GetFieldID(configClass, "sampleIntervalMs", "F");
    jfloat sampleInterval = env->GetFloatField(configObj, filedId);
    setSampleIntervalMs(sampleInterval);

    jint events[len];
    env->GetIntArrayRegion(jevents, 0, len, events);
    for (int i : events) {
        SetEventNotification(jvmti_env, JVMTI_ENABLE, jvmtiEvent(i));
    }
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

//===============用于 Looper 的测试方法 =============

static JNINativeMethod methods[] = {
        {"startDump",           "(Ljava/lang/String;)V",    (void *) startDump},
        {"stopDump",            "()V",                      (void *) stopDump},
        {"getObjectSize",       "(Ljava/lang/Object;)J",    (void *) getObjectSize},
        {"enableEvents",        "(Lcom/adi/ADIConfig;[I)V", (void *) enableEvents},
        {"disableEvents",       "([I)V",                    (void *) disableEvents},
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

