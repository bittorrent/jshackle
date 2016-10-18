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

#include "jshackle/JavaObject.h"
#include "jshackle/JNIContext.h"

#include <mutex>
#include <string>
#include <vector>

namespace jshackle {

struct JavaObject;
struct JavaClass;
struct JavaClassRefBase;

template <typename T>
void NoDelete(void* ptr) {}

template <typename T, typename Enable = void>
struct ToJava {};

template <typename T>
struct ToJava<T, typename std::enable_if<std::is_pointer<T>::value, void>::type> {
    using Type = jobject;
    using DecayedType = typename std::decay<typename std::remove_pointer<T>::type>::type;

    static jobject Convert(JNIContext& jniContext, JNIEnv* env, T obj) {
        std::lock_guard<std::mutex> lock{jniContext.mutex};

        // if this object was created in java, return the same one

        {
            auto it = jniContext.ownedObjects.find(const_cast<void*>(static_cast<const void*>(obj)));
            if (it != jniContext.ownedObjects.end()) { return it->second; }
        }

        // otherwise if it's something we converted, use that

        {
            for (auto it = jniContext.wrapped.begin(); it != jniContext.wrapped.end();) {
                if (env->IsSameObject(it->object, nullptr)) {
                    it = jniContext.remove(env, it);
                } else if (it->pointer == obj) {
                    return it->object;
                } else {
                    ++it;
                }
            }
        }

        // otherwise if it's a class we've registered, wrap it in a new java instance

        const auto identifier = TypeIdentifier<DecayedType>()();
        {
            auto it = jniContext.nativeClasses.find(identifier);
            if (it != jniContext.nativeClasses.end()) {
                const auto javaClass = it->second.javaClass;
                const auto mid = env->GetMethodID(javaClass, "<init>", "()V");
                const auto fid = env->GetFieldID(javaClass, "_nativeHandle", "J");
                auto jobj = env->NewObject(javaClass, mid);
                env->SetLongField(jobj, fid, reinterpret_cast<jlong>(obj));
                return jobj;
            }
        }

        // otherwise we can't convert the argument
        // the jni must either register this class or provide a specialization of ToJava

        assert(false);

        return nullptr;
    }

    static std::string Signature(JNIContext& jniContext) {
        auto info = jniContext.findClassInfo<DecayedType>();
        assert(info);
        return info->signature();
    }
};

template <typename T>
struct ToJava<T, typename std::enable_if<std::is_reference<T>::value, void>::type>
    : ToJava<typename std::remove_reference<T>::type> {};

template <typename T>
struct ToJava<T,
              typename std::enable_if<std::is_integral<typename std::remove_cv<T>::type>::value &&
                                          sizeof(typename std::remove_cv<T>::type) == 1,
                                      void>::type> {
    using Type = jbyte;
    static constexpr const char* Signature(JNIContext& jniContext) { return "B"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, T n) { return static_cast<Type>(n); }
};

template <typename T>
struct ToJava<T,
              typename std::enable_if<std::is_integral<typename std::remove_cv<T>::type>::value &&
                                          sizeof(typename std::remove_cv<T>::type) == 2,
                                      void>::type> {
    using Type = jshort;
    static constexpr const char* Signature(JNIContext& jniContext) { return "S"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, T n) { return static_cast<Type>(n); }
};

template <typename T>
struct ToJava<T,
              typename std::enable_if<std::is_integral<typename std::remove_cv<T>::type>::value &&
                                          sizeof(typename std::remove_cv<T>::type) == 4,
                                      void>::type> {
    using Type = jint;
    static constexpr const char* Signature(JNIContext& jniContext) { return "I"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, T n) { return static_cast<Type>(n); }
};

template <typename T>
struct ToJava<T,
              typename std::enable_if<std::is_integral<typename std::remove_cv<T>::type>::value &&
                                          sizeof(typename std::remove_cv<T>::type) == 8,
                                      void>::type> {
    using Type = jlong;
    static constexpr const char* Signature(JNIContext& jniContext) { return "J"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, T n) { return static_cast<Type>(n); }
};

template <>
struct ToJava<void> {
    using Type = void;
    static constexpr const char* Signature(JNIContext& jniContext) { return "V"; }
};

template <>
struct ToJava<bool> {
    using Type = jboolean;
    static constexpr const char* Signature(JNIContext& jniContext) { return "Z"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, bool b) { return static_cast<Type>(b); }
};

template <>
struct ToJava<float> {
    using Type = jfloat;
    static constexpr const char* Signature(JNIContext& jniContext) { return "F"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, float f) { return static_cast<Type>(f); }
};

template <>
struct ToJava<double> {
    using Type = jdouble;
    static constexpr const char* Signature(JNIContext& jniContext) { return "D"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, double d) { return static_cast<Type>(d); }
};

template <>
struct ToJava<const char*> {
    using Type = jstring;
    static constexpr const char* Signature(JNIContext& jniContext) { return "Ljava/lang/String;"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, const char* str) { return env->NewStringUTF(str); }
};

template <typename T>
struct ToJava<T, typename std::enable_if<std::is_base_of<JavaObject, T>::value, void>::type> {
    using Type = jobject;
    static std::string Signature(JNIContext& jniContext) {
        return std::string("L") + T::Name() + ';';
    }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jobject obj) { return obj; }
};

template <>
struct ToJava<std::string> {
    using Type = jstring;
    static constexpr const char* Signature(JNIContext& jniContext) { return "Ljava/lang/String;"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, std::string s) { return env->NewStringUTF(s.c_str()); }
};

template <>
struct ToJava<const std::string> {
    using Type = jstring;
    static constexpr const char* Signature(JNIContext& jniContext) { return "Ljava/lang/String;"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, const std::string& s) {
        return env->NewStringUTF(s.c_str());
    }
};

template <typename T>
struct ToJava<T, typename std::enable_if<std::is_base_of<JavaClass, T>::value, void>::type> {
    using Type = jobject;
    static std::string Signature(JNIContext& jniContext) {
        return std::string("L") + T::Traits::sName + ';';
    }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, T obj) { return obj.caller->globalRef; }
};

template <typename T>
struct ToJava<T, typename std::enable_if<std::is_base_of<JavaClassRefBase, T>::value, void>::type> {
    using Type = jobject;
    static std::string Signature(JNIContext& jniContext) {
        return std::string("L") + T::JavaClassType::Traits::sName + ';';
    }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, T obj) { return obj._object; }
};

template <typename T>
struct ToJava<const std::vector<T>> {
    using Type = jobjectArray;
    using Element = ToJava<T>;
    static std::string Signature(JNIContext& jniContext) {
        return std::string("[") + Element::Signature(jniContext);
    }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, const std::vector<T>& vector) {
        std::string className = Element::Signature(jniContext);
        className.pop_back();
        auto array = env->NewObjectArray(vector.size(), env->FindClass(className.c_str() + 1), nullptr);
        for (size_t i = 0; i < vector.size(); ++i) {
            env->SetObjectArrayElement(array, i, Element::Convert(jniContext, env, vector[i]));
        }
        return array;
    }
};

} // namespace jshackle
