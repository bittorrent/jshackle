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

#include "jshackle/config.h"

#include "jshackle/ToJava.h"
#include "jshackle/ToNative.h"
#include "jshackle/utility.h"

namespace jshackle {

template <typename T, typename Enable = void>
struct JavaMethodCaller {};

template <typename T, typename Enable = void>
struct StaticJavaMethodCaller {};

template <typename T>
struct JavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jint>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, va_list& args) {
        return ToNative<jint, T>::Convert(jniContext, env, env->CallIntMethodV(object, method, args));
    }
};

template <typename T>
struct StaticJavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jint>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, va_list& args) {
        return ToNative<jint, T>::Convert(jniContext, env, env->CallStaticIntMethodV(clazz, method, args));
    }
};

template <typename T>
struct JavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jlong>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, va_list& args) {
        return ToNative<jlong, T>::Convert(jniContext, env, env->CallLongMethodV(object, method, args));
    }
};

template <typename T>
struct StaticJavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jlong>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, va_list& args) {
        return ToNative<jlong, T>::Convert(jniContext, env, env->CallStaticLongMethodV(clazz, method, args));
    }
};

template <typename T>
struct JavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jboolean>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, va_list& args) {
        return ToNative<jboolean, T>::Convert(jniContext, env, env->CallBooleanMethodV(object, method, args));
    }
};

template <typename T>
struct StaticJavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jboolean>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, va_list& args) {
        return ToNative<jboolean, T>::Convert(jniContext, env, env->CallStaticBooleanMethodV(clazz, method, args));
    }
};

template <typename T>
struct JavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jfloat>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, va_list& args) {
        return ToNative<jfloat, T>::Convert(jniContext, env, env->CallFloatMethodV(object, method, args));
    }
};

template <typename T>
struct StaticJavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jfloat>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, va_list& args) {
        return ToNative<jfloat, T>::Convert(jniContext, env, env->CallStaticFloatMethodV(clazz, method, args));
    }
};

template <typename T>
struct JavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jobject>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, va_list& args) {
        return ToNative<jobject, T>::Convert(jniContext, env, env->CallObjectMethodV(object, method, args));
    }
};

template <typename T>
struct StaticJavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jobject>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, va_list& args) {
        return ToNative<jobject, T>::Convert(jniContext, env, env->CallStaticObjectMethodV(clazz, method, args));
    }
};

template <typename T>
struct JavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jstring>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, va_list& args) {
        const auto result = static_cast<jstring>(env->CallObjectMethodV(object, method, args));
        auto ret = ToNative<jstring, T>::Convert(jniContext, env, result);
        env->DeleteLocalRef(result);
        return ret;
    }
};

template <typename T>
struct StaticJavaMethodCaller<T, std::enable_if_t<std::is_same_v<typename ToJava<T>::Type, jstring>, void>> {
    static T Call(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, va_list& args) {
        const auto result = static_cast<jstring>(env->CallStaticObjectMethodV(clazz, method, args));
        auto ret = ToNative<jstring, T>::Convert(jniContext, env, result);
        env->DeleteLocalRef(result);
        return ret;
    }
};

template <typename T>
T CallJavaMethod(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, ...) {
    va_list args;
    va_start(args, method);
    auto ret = JavaMethodCaller<T>::Call(jniContext, env, object, method, args);
    va_end(args);
    return ret;
}

template <typename T>
T CallStaticJavaMethod(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, ...) {
    va_list args;
    va_start(args, method);
    auto ret = StaticJavaMethodCaller<T>::Call(jniContext, env, clazz, method, args);
    va_end(args);
    return ret;
}

template <>
inline void CallJavaMethod<void>(JNIContext& jniContext, JNIEnv* env, jobject object, jmethodID method, ...) {
    va_list args;
    va_start(args, method);
    env->CallVoidMethodV(object, method, args);
    va_end(args);
}

template <>
inline void CallStaticJavaMethod<void>(JNIContext& jniContext, JNIEnv* env, jclass clazz, jmethodID method, ...) {
    va_list args;
    va_start(args, method);
    env->CallStaticVoidMethodV(clazz, method, args);
    va_end(args);
}

template <typename T>
inline std::enable_if_t<!std::is_convertible_v<T, jobject>, void>
GetLocalRef(JNIEnv* env, std::vector<jobject>& dest, T nonobj) {}

template <typename T>
inline std::enable_if_t<std::is_convertible_v<T, jobject>, void>
GetLocalRef(JNIEnv* env, std::vector<jobject>& dest, T obj) {
    if (env->GetObjectRefType(obj) == JNILocalRefType) { dest.push_back(obj); }
}

inline void GetLocalRefs(JNIEnv* env, std::vector<jobject>& dest) {}

template <typename Next, typename... Rem>
void GetLocalRefs(JNIEnv* env, std::vector<jobject>& dest, Next&& next, Rem&&... rem) {
    GetLocalRef(env, dest, std::forward<Next>(next));
    GetLocalRefs(env, dest, std::forward<Rem>(rem)...);
}

struct ScopedLocalRefs {
    template <typename... Args>
    ScopedLocalRefs(JNIEnv* env, Args&&... args)
        : env(env) {
        GetLocalRefs(env, refs, std::forward<Args>(args)...);
    }

    ~ScopedLocalRefs() {
        for (auto& ref : refs) {
            env->DeleteLocalRef(ref);
        }
    }

    JNIEnv* env = nullptr;
    std::vector<jobject> refs;
};

template <typename T, typename... Args>
auto CallJavaMethodAndDeleteLocalRefArgs(JNIContext& jniContext,
                                         JNIEnv* env,
                                         jobject object,
                                         jmethodID method,
                                         Args&&... args)
    -> decltype(CallJavaMethod<T>(jniContext, env, object, method, std::forward<Args>(args)...)) {
    ScopedLocalRefs localRefs(env, std::forward<Args>(args)...);
    return CallJavaMethod<T>(jniContext, env, object, method, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
auto CallStaticJavaMethodAndDeleteLocalRefArgs(JNIContext& jniContext,
                                               JNIEnv* env,
                                               jclass clazz,
                                               jmethodID method,
                                               Args&&... args)
    -> decltype(CallStaticJavaMethod<T>(jniContext, env, clazz, method, std::forward<Args>(args)...)) {
    ScopedLocalRefs localRefs(env, std::forward<Args>(args)...);
    return CallStaticJavaMethod<T>(jniContext, env, clazz, method, std::forward<Args>(args)...);
}

} // namespace jshackle
