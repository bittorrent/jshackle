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
#include "jshackle/utility.h"

#include <mutex>
#include <string>

namespace jshackle {

template <typename T>
void DefaultDelete(void* ptr) {
    delete reinterpret_cast<T>(ptr);
}

template <typename T, typename Hint = void, typename Enable = void>
struct ToNative {};

template <typename Hint, typename Enable = void>
struct ObjToNative {
    typedef Hint Type;
    using DecayedType = typename std::decay<typename std::remove_pointer<Hint>::type>::type;

    static Type Convert(JNIContext& jniContext, JNIEnv* env, jobject obj) {
        std::lock_guard<std::mutex> lock{jniContext.mutex};

        // if this is a native object, just use the native pointer
        auto c = env->GetObjectClass(obj);
        auto fid = env->GetFieldID(c, "_nativeHandle", "J");
        if (fid) {
            auto handle = env->GetLongField(obj, fid);
            if (handle) {
                for (auto& kv : jniContext.nativeClasses) {
                    if (env->IsSameObject(c, kv.second.javaClass)) {
                        auto it2 = kv.second.pointerConversions.find(TypeIdentifier<typename std::remove_pointer<Hint>::type>()());
                        if (it2 != kv.second.pointerConversions.end()) { return reinterpret_cast<Hint>(handle + it2->second); }
                        break;
                    }
                }
            }
        } else { env->ExceptionClear(); }

        // not a native object, see if we have a native wrapper for it

        for (auto it = jniContext.wrapped.begin(); it != jniContext.wrapped.end();) {
            if (env->IsSameObject(it->object, nullptr)) {
                it = jniContext.remove(env, it);
            } else if (env->IsSameObject(obj, it->object) && &TypeIdentifier<Type>::Identifier == it->type) {
                return reinterpret_cast<Type>(it->pointer);
            } else { ++it; }
        }

        // nope, create one

        auto it = jniContext.javaClasses.find(TypeIdentifier<DecayedType>()());
        if (it == jniContext.javaClasses.end()) {
            assert(false);
            return nullptr;
        }

        auto interface = reinterpret_cast<Type>(it->second.interfaceFunc(env, obj));

        JNIContext::Wrapper wrap;
        wrap.object = env->NewWeakGlobalRef(obj);
        wrap.type = &TypeIdentifier<Type>::Identifier;
        wrap.pointer = const_cast<void*>(reinterpret_cast<const void*>(interface));
        wrap.deleter = &DefaultDelete<Type>;
        jniContext.wrapped.emplace_back(std::move(wrap));

        return interface;
    }
};

template <typename Hint>
struct ObjToNative<Hint, typename std::enable_if<std::is_base_of<JavaObject, Hint>::value, void>::type> {
    typedef Hint Type;

    static Type Convert(JNIContext& jniContext, JNIEnv* env, jobject obj) { return Hint(&jniContext, obj); }
};

template <typename Hint>
struct ToNative<jobject, Hint> {
    typedef typename ObjToNative<Hint>::Type Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jobject obj) {
        return ObjToNative<Hint>::Convert(jniContext, env, obj);
    }
};

template <typename Hint>
struct ToNative<jbyte, Hint> {
    typedef Hint Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jbyte n) { return static_cast<Type>(n); }
};

template <typename Hint>
struct ToNative<jboolean, Hint> {
    typedef Hint Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jboolean n) { return static_cast<Type>(n); }
};

template <typename Hint>
struct ToNative<jshort, Hint> {
    typedef Hint Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jshort n) { return static_cast<Type>(n); }
};

template <typename Hint>
struct ToNative<jint, Hint> {
    typedef Hint Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jint n) { return static_cast<Type>(n); }
};

template <typename Hint>
struct ToNative<jlong, Hint> {
    typedef Hint Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jlong n) { return static_cast<Type>(n); }
};

template <typename Hint>
struct ToNative<jfloat, Hint> {
    typedef Hint Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jfloat f) { return static_cast<Type>(f); }
};

template <typename Hint>
struct ToNative<jdouble, Hint> {
    typedef Hint Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jdouble d) { return static_cast<Type>(d); }
};

template <typename Hint>
struct ToNative<void, Hint> {
    typedef void Type;
};

template <typename Hint>
struct ToNative<jstring, Hint> {
    struct Container {
        Container(JNIContext& jniContext, JNIEnv* env, const jstring& s)
            : jvm(jniContext.jvm)
            , jniVersion(jniContext.jniVersion)
            , globalRef(env->NewGlobalRef(s))
            , str(s ? env->GetStringUTFChars(s, nullptr) : nullptr) {}
        ~Container() {
            if (str) {
                JavaAttachment attachment(jvm, jniVersion);
                attachment.env->ReleaseStringUTFChars(static_cast<jstring>(globalRef), str);
                attachment.env->DeleteGlobalRef(globalRef);
            }
        }

        operator const char*() const { return str ? str : ""; }

        operator std::string() const { return str ? str : ""; }

        JavaVM* const jvm = nullptr;
        const jint jniVersion = JNI_VERSION_1_6;
        const jobject globalRef = nullptr;
        const char* const str = nullptr;
    };

    static Container Convert(JNIContext& jniContext, JNIEnv* env, const jstring& s) { return Container(jniContext, env, s); }
};

} // namespace jshackle
