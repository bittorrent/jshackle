/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
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
