#include <jni.h>
#include <string>
#include "jvmti.h"
#include <android/log.h>
#include <sstream>

#define LOG_TAG "jvmti"

#define ALOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
static jvmtiEnv *localJvmtiEnv;

static void
ClassTransform(jvmtiEnv *jvmti_env,
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

jvmtiEnv *CreateJvmtiEnv(JavaVM *vm) {
    jvmtiEnv *jvmti_env;
    jint result = vm->GetEnv((void **) &jvmti_env, JVMTI_VERSION_1_2);
    if (result != JNI_OK) {
        return nullptr;
    }

    return jvmti_env;

}

void ObjectAllocCallback(jvmtiEnv *jvmti, JNIEnv *jni,
                         jthread thread, jobject object,
                         jclass klass, jlong size) {
    jclass cls = jni->FindClass("java/lang/Class");
    jmethodID mid_getName = jni->GetMethodID(cls, "getName", "()Ljava/lang/String;");
    jstring name = static_cast<jstring>(jni->CallObjectMethod(klass, mid_getName));
    const char *className = jni->GetStringUTFChars(name, JNI_FALSE);
    ALOGI("==========alloc callback======= %s {size:%d}", className, size);
    jni->ReleaseStringUTFChars(name, className);
}

void GCStartCallback(jvmtiEnv *jvmti) {
    ALOGI("==========触发 GCStart=======");

}

void GCFinishCallback(jvmtiEnv *jvmti) {
    ALOGI("==========触发 GCFinish=======");

}

void SetEventNotification(jvmtiEnv *jvmti, jvmtiEventMode mode,
                          jvmtiEvent event_type) {
    jvmtiError err = jvmti->SetEventNotificationMode(mode, event_type, nullptr);
}

extern "C" JNIEXPORT void JNICALL retransformClasses(JNIEnv *env,
                                                     jclass clazz,
                                                     jobjectArray classes) {
    jsize numTransformedClasses = env->GetArrayLength(classes);
    jclass *transformedClasses = (jclass *) malloc(numTransformedClasses * sizeof(jclass));

    for (int i = 0; i < numTransformedClasses; i++) {
        transformedClasses[i] = (jclass) env->NewGlobalRef(env->GetObjectArrayElement(classes, i));
    }
    ALOGI("==============retransformClasses ===============");

    jvmtiError error = localJvmtiEnv->RetransformClasses(numTransformedClasses,
                                                         transformedClasses);

    for (int i = 0; i < numTransformedClasses; i++) {
        env->DeleteGlobalRef(transformedClasses[i]);
    }
    free(transformedClasses);
}

void JNICALL
JvmTINativeMethodBind(jvmtiEnv *jvmti_env, JNIEnv *jni_env, jthread thread, jmethodID method,
                      void *address, void **new_address_ptr) {
    ALOGI("===========NativeMethodBind===============");

    jclass clazz = jni_env->FindClass("com/dodola/jvmtilib/JVMTIHelper");
    jmethodID methodid = jni_env->GetStaticMethodID(clazz, "retransformClasses",
                                                    "([Ljava/lang/Class;)V");
    if (methodid == method) {
        *new_address_ptr = reinterpret_cast<void *>(&retransformClasses);
    }
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

extern "C" JNIEXPORT jint JNICALL Agent_OnAttach(JavaVM *vm, char *options,
                                                 void *reserved) {
    jvmtiEnv *jvmti_env = CreateJvmtiEnv(vm);

    if (jvmti_env == nullptr) {
        return JNI_ERR;
    }
    localJvmtiEnv = jvmti_env;
    SetAllCapabilities(jvmti_env);

    jvmtiEventCallbacks callbacks;
    memset(&callbacks, 0, sizeof(callbacks));
    callbacks.ClassFileLoadHook = &ClassTransform;

    callbacks.VMObjectAlloc = &ObjectAllocCallback;
    callbacks.NativeMethodBind = &JvmTINativeMethodBind;

    callbacks.GarbageCollectionStart = &GCStartCallback;
    callbacks.GarbageCollectionFinish = &GCFinishCallback;
    int error = jvmti_env->SetEventCallbacks(&callbacks, sizeof(callbacks));

    SetEventNotification(jvmti_env, JVMTI_ENABLE,
                         JVMTI_EVENT_GARBAGE_COLLECTION_START);
    SetEventNotification(jvmti_env, JVMTI_ENABLE,
                         JVMTI_EVENT_GARBAGE_COLLECTION_FINISH);
    SetEventNotification(jvmti_env, JVMTI_ENABLE,
                         JVMTI_EVENT_NATIVE_METHOD_BIND);
    SetEventNotification(jvmti_env, JVMTI_ENABLE,
                         JVMTI_EVENT_VM_OBJECT_ALLOC);
    SetEventNotification(jvmti_env, JVMTI_ENABLE,
                         JVMTI_EVENT_OBJECT_FREE);
    SetEventNotification(jvmti_env, JVMTI_ENABLE,
                         JVMTI_EVENT_CLASS_FILE_LOAD_HOOK);
    ALOGI("==========Agent_OnAttach=======");
    return JNI_OK;

}

extern "C" JNIEXPORT void JNICALL tempRetransformClasses(JNIEnv *env,
                                                         jclass clazz,
                                                         jobjectArray classes) {
}

static JNINativeMethod methods[] = {
        {"retransformClasses", "([Ljava/lang/Class;)V", reinterpret_cast<void *>(tempRetransformClasses)}
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    if (vm->GetEnv((void **) &env, JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    ALOGI("==============library load====================");
    jclass clazz = env->FindClass("com/dodola/jvmtilib/JVMTIHelper");
    env->RegisterNatives(clazz, methods, 1);
    return JNI_VERSION_1_6;
}