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

#include "jshackle/JavaSignature.h"
#include "jshackle/utility.h"

#include <cassert>
#include <mutex>

namespace jshackle {

template <typename Decl, typename T>
struct NativeTraits {
    using Type = T;

    static constexpr const char* JavaName() { return sJavaName.c_str(); }

    static std::unordered_map<TypeIdentification, std::ptrdiff_t> PointerConversions() {
        std::unordered_map<TypeIdentification, std::ptrdiff_t> conversions;
        conversions[TypeIdentifier<Type>()()] = 0;
        JSHACKLE_FOREACH(JSHACKLE_POINTER_CONVERSION, (Type))
        return conversions;
    }

    static void Register(JNIContext* jniContext, const char* name) {
        JSHACKLE_LOG("registering native class %s", name);

        sJNIContext = jniContext;
        sJavaName = name;

        Decl temp;
        std::vector<JNINativeMethod> nativeMethods;
        for (auto& method : temp.nativeMethods) {
            JSHACKLE_LOG("registering native class method %s - %s", method.name.c_str(), method.signature.c_str());
            JNINativeMethod nativeMethod;
            nativeMethod.name = const_cast<char*>(method.name.c_str());
            nativeMethod.signature = const_cast<char*>(method.signature.c_str());
            nativeMethod.fnPtr = method.pointer;
            nativeMethods.emplace_back(nativeMethod);
        }

        sJNIContext->registerNativeClass<T>(sJavaName, nativeMethods, std::move(PointerConversions()));
    }

    static JNIContext* sJNIContext;
    static std::string sJavaName;
};

template <typename T, typename U>
JNIContext* NativeTraits<T, U>::sJNIContext = nullptr;
template <typename T, typename U>
std::string NativeTraits<T, U>::sJavaName;

struct NativeClass {
    struct NativeMethodRegistration {
        NativeMethodRegistration(std::vector<NativeMethodRegistration>* destination,
                                 std::string name,
                                 std::string signature,
                                 void* pointer)
            : destination(destination), name(name), signature(signature), pointer(pointer) {}
        std::vector<NativeMethodRegistration>* destination;
        std::string name;
        std::string signature;
        void* pointer;
    };

    struct NativeMethodRegistrar {
        NativeMethodRegistrar(NativeMethodRegistration&& registration) {
            registration.destination->emplace_back(registration);
        }
    };

    std::vector<NativeMethodRegistration> nativeMethods;
};

template <typename ReturnType>
struct Invoker {
    template <typename F, typename I, typename... Args>
    static typename ToJava<ReturnType>::Type Invoke(JNIContext& jniContext,
                                                    JNIEnv* env,
                                                    F&& func,
                                                    I&& instance,
                                                    Args&&... args) {
        return ToJava<ReturnType>::Convert(jniContext, env, (instance->*(func))(std::forward<Args>(args)...));
    }
};

template <>
struct Invoker<void> {
    template <typename F, typename I, typename... Args>
    static void Invoke(JNIContext& jniContext, JNIEnv* env, F&& func, I&& instance, Args&&... args) {
        return (instance->*(func))(std::forward<Args>(args)...);
    }
};

// helpers
#define JSHACKLE_TO_NATIVE_ARG(N, T) , jshackle::ToNative<decltype(arg##N), T>::Convert(*Traits::sJNIContext, env, arg##N)

#define JSHACKLE_NATIVE_CLASS_CONSTRUCTOR(...)                                                                      \
    static void _construct(JNIEnv* env, jobject obj JSHACKLE_FOREACH(JSHACKLE_NUMBER_JAVA_ARG, (__VA_ARGS__))) {    \
        auto implementation = new Type(JSHACKLE_FOREACH(JSHACKLE_TO_NATIVE_CONSTRUCTOR_ARG, (__VA_ARGS__)));        \
        auto c              = env->GetObjectClass(obj);                                                             \
        auto fid = env->GetFieldID(c, "_nativeHandle", "J");                                                        \
        env->SetLongField(obj, fid, reinterpret_cast<jlong>(implementation));                                       \
        std::unique_lock<std::mutex> lock(Traits::sJNIContext->mutex);                                              \
        Traits::sJNIContext->ownedObjects[implementation] = env->NewWeakGlobalRef(obj);                             \
    }                                                                                                               \
    NativeMethodRegistrar _constructNativeMethodRegistrar = NativeMethodRegistration(                               \
        &nativeMethods,                                                                                             \
        "_construct",                                                                                               \
        jshackle::JavaMethodSignature<void JSHACKLE_FOREACH(JSHACKLE_APPEND, (__VA_ARGS__))>(*Traits::sJNIContext), \
        reinterpret_cast<void*>(&_construct));

#define JSHACKLE_NATIVE_CLASS_DESTRUCTOR()                                                             \
    static void destroy(JNIEnv* env, jobject obj) {                                                  \
        auto c = env->GetObjectClass(obj);                                                           \
        auto fid = env->GetFieldID(c, "_nativeHandle", "J");                                         \
        auto handle = env->GetLongField(obj, fid);                                                   \
        auto implementation = reinterpret_cast<Type*>(handle);                                       \
        delete implementation;                                                                       \
        std::unique_lock<std::mutex> lock(Traits::sJNIContext->mutex);                               \
        auto it = Traits::sJNIContext->ownedObjects.find(implementation);                            \
        if (it != Traits::sJNIContext->ownedObjects.end()) {                                         \
            env->DeleteWeakGlobalRef(it->second);                                                    \
            Traits::sJNIContext->ownedObjects.erase(it);                                             \
        }                                                                                            \
        lock.unlock();                                                                               \
    }                                                                                                \
    NativeMethodRegistrar _destroyNativeMethodRegistrar = NativeMethodRegistration(                  \
        &nativeMethods, "destroy", jshackle::JavaMethodSignature<void>(*Traits::sJNIContext),        \
        reinterpret_cast<void*>(&destroy));

#define JSHACKLE_NATIVE_CLASS_METHOD_IMPL(RETURN_TYPE, NAME, ID, ...)                                \
    static jshackle::ToJava<RETURN_TYPE>::Type NAME##ID(                                             \
        JNIEnv* env, jobject obj JSHACKLE_FOREACH(JSHACKLE_NUMBER_JAVA_ARG, (__VA_ARGS__))) {        \
        auto c              = env->GetObjectClass(obj);                                              \
        auto fid            = env->GetFieldID(c, "_nativeHandle", "J");                              \
        auto handle         = env->GetLongField(obj, fid);                                           \
        auto implementation = reinterpret_cast<Type*>(handle);                                       \
        return jshackle::Invoker<RETURN_TYPE>::Invoke(                                               \
            *Traits::sJNIContext,                                                                    \
            env,                                                                                     \
            (RETURN_TYPE (Type::*)(JSHACKLE_FOREACH(JSHACKLE_TYPE, (__VA_ARGS__)))) & Type::NAME,    \
            implementation JSHACKLE_FOREACH(JSHACKLE_TO_NATIVE_ARG, (__VA_ARGS__)));                 \
    }                                                                                                \
    NativeMethodRegistrar _##NAME##ID##NativeMethodRegistrar = NativeMethodRegistration(             \
        &nativeMethods,                                                                              \
        #NAME,                                                                                       \
        jshackle::JavaMethodSignature<RETURN_TYPE JSHACKLE_FOREACH(JSHACKLE_APPEND, (__VA_ARGS__))>( \
            *Traits::sJNIContext),                                                                   \
        reinterpret_cast<void*>(reinterpret_cast<void (*)(__VA_ARGS__)>(&NAME##ID)));

#define JSHACKLE_NATIVE_CLASS_METHOD_IMPL_WRAPPER(RETURN_TYPE, NAME, COUNTER, ...) \
    JSHACKLE_NATIVE_CLASS_METHOD_IMPL(RETURN_TYPE, NAME, COUNTER, __VA_ARGS__)

#define JSHACKLE_NATIVE_CLASS_METHOD(RETURN_TYPE, NAME, ...) \
    JSHACKLE_NATIVE_CLASS_METHOD_IMPL_WRAPPER(RETURN_TYPE, NAME, __COUNTER__, __VA_ARGS__)

#define JSHACKLE_NATIVE_CLASS_BEGIN(NAME, REPRESENTED)            \
    struct NAME : jshackle::NativeClass {                         \
        using Traits = jshackle::NativeTraits<NAME, REPRESENTED>; \
        using Type   = Traits::Type;

#define JSHACKLE_NATIVE_CLASS_END() \
    }                               \
    ;

} // namespace jshackle
