#pragma once

#include <jni.h>

namespace jshackle {

// some jni versions require the first argument to JavaVM::AttachCurrentThread to be void** instead of JNIEnv**
template <typename R, typename C, typename A, typename B>
R AttachCurrentThreadWrapper(JavaVM* jvm, R (C::*f)(A, B), JNIEnv** env, void* args) {
    return (jvm->*f)(reinterpret_cast<A>(env), args);
};

struct JavaAttachment {
    JavaAttachment(JavaVM* jvm, jint jniVersion) : jvm(jvm) {
        if (jvm->GetEnv((void**)&env, jniVersion) != JNI_OK || !env) {
            JavaVMAttachArgs args;
            args.version = jniVersion;
            args.name = nullptr;
            args.group = nullptr;
            AttachCurrentThreadWrapper(jvm, &JavaVM::AttachCurrentThread, &env, &args);
            didAttach = true;
        }
    }

    ~JavaAttachment() {
        if (didAttach) { jvm->DetachCurrentThread(); }
    }

    JavaVM* jvm = nullptr;
    JNIEnv* env = nullptr;
    bool didAttach = false;
};

} // namespace jshackle
