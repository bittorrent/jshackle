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

#include "jshackle/JavaAttachment.h"
#include "jshackle/JavaSignature.h"
#include "jshackle/MethodCallers.h"
#include "jshackle/utility.h"

namespace jshackle {

struct JavaCaller : std::enable_shared_from_this<JavaCaller> {
    JavaCaller(JNIContext* context) : context(context) { assert(context); }

    JavaCaller(JNIContext* context, JNIEnv* env, jobject object, const char* name)
        : context{context}, globalRef{env->NewWeakGlobalRef(object)}, isWeak{true} {
        assert(context);
    }

    ~JavaCaller() {
        JavaAttachment attachment(context->jvm, context->jniVersion);
        if (isWeak) {
            attachment.env->DeleteWeakGlobalRef(globalRef);
        } else {
            attachment.env->DeleteGlobalRef(globalRef);
        }
    }

    template <typename... Args>
    void createRef(jclass classRef, const char* name, Args&&... args) {
        assert(context); // remember to call Register()

        const auto signature = JavaMethodSignature<void, Args...>(*context);

        if (!globalRef) {
            JavaAttachment attachment(context->jvm, context->jniVersion);
            auto mid = attachment.env->GetMethodID(classRef, "<init>", signature.c_str());
            assert(mid);
            auto object =
                attachment.env->NewObject(classRef, mid, ToJava<Args>::Convert(*context, attachment.env, args)...);
            globalRef = attachment.env->NewGlobalRef(object);
        }
    }

    template <typename Return, typename... Args>
    Return callMethod(JNIEnv* env, jclass classRef, const char* name, const std::string& signature, Args&&... args)
        const {
        auto mid = env->GetMethodID(classRef, name, signature.c_str());
        return CallJavaMethodAndDeleteLocalRefArgs<Return>(*context, env, globalRef, mid, std::forward<Args>(args)...);
    }

    template <typename Return, typename... Args>
    static Return callStaticMethod(JNIContext& context,
                                   JNIEnv* env,
                                   jclass classRef,
                                   const char* name,
                                   const std::string& signature,
                                   Args&&... args) {
        auto mid = env->GetStaticMethodID(classRef, name, signature.c_str());
        return CallStaticJavaMethodAndDeleteLocalRefArgs<Return>(
            context, env, classRef, mid, std::forward<Args>(args)...);
    }

    JNIContext* context;
    jobject globalRef = nullptr;
    bool isWeak       = false;
};

template <typename T>
struct JavaTraits {
    static void* New(JNIEnv* env, jobject obj) { return new T(env, obj); }

    static std::shared_ptr<JavaCaller> MakeCaller(JNIEnv* env, jobject obj) {
        return std::make_shared<JavaCaller>(sJNIContext, env, obj, sName);
    }

    template <typename... Args>
    static std::shared_ptr<JavaCaller> MakeCaller(Args&&... args) {
        auto caller = std::make_shared<JavaCaller>(sJNIContext);
        caller->createRef(sClassRef, sName, std::forward<Args>(args)...);
        return caller;
    }

    static void RegisterClassRef(JNIContext* jniContext, const char* name) {
        sName       = name;
        sJNIContext = jniContext;

        JavaAttachment attachment(sJNIContext->jvm, sJNIContext->jniVersion);

        sClassRef = static_cast<jclass>(attachment.env->NewGlobalRef(attachment.env->FindClass(name)));
    }

    static void Register(JNIContext* jniContext, const char* name) {
        JSHACKLE_LOG("registering java class %s", name);

        RegisterClassRef(jniContext, name);

        sJNIContext->registerJavaClass<T>(name, &JavaTraits::New);
    }

    static JNIContext* sJNIContext;
    static jclass sClassRef;
    static const char* sName;
};

template <typename T, typename Interface>
struct JavaInterfaceTraits : JavaTraits<T> {
    static void Register(JNIContext* jniContext, const char* name) {
        JSHACKLE_LOG("registering java interface %s", name);

        JavaTraits<T>::RegisterClassRef(jniContext, name);

        jniContext->registerJavaClass<Interface>(name, &JavaTraits<T>::New);
    }
};

template <typename T>
JNIContext* JavaTraits<T>::sJNIContext = nullptr;
template <typename T>
jclass JavaTraits<T>::sClassRef = nullptr;
template <typename T>
const char* JavaTraits<T>::sName = "";

struct JavaClass {
    JavaClass(std::nullptr_t) {}
    JavaClass(const std::shared_ptr<JavaCaller>& caller) : caller{caller} {}

    std::shared_ptr<JavaCaller> caller;
};

struct JavaClassRefBase {};

template <typename T, bool local = true>
struct JavaClassRef : JavaClassRefBase {
    using JavaClassType = T;

    JavaClassRef(JNIContext* context, jobject object) : _context{context}, _object{object} {}

    ~JavaClassRef() {
        if (_context && _object) {
            JavaAttachment attachment(_context->jvm, _context->jniVersion);
            if (local) { attachment.env->DeleteLocalRef(_object); }
            else       { attachment.env->DeleteGlobalRef(_object); }
        }
    }

    JNIContext* _context = nullptr;
    jobject     _object = nullptr;
};

// helper macros
#define JSHACKLE_JAVA_CLASS_MULTIPLE_INHERITANCE(N, T) , public T
#define JSHACKLE_TO_JAVA_ARG(N, T) \
    , jshackle::ToJava<decltype(arg##N)>::Convert(*Traits::sJNIContext, attachment.env, arg##N)

#define JSHACKLE_JAVA_CLASS_CONSTRUCTORS(NAME)                                  \
    NAME() : BaseType{Traits::MakeCaller()} {}                                  \
    NAME(std::nullptr_t) : BaseType{nullptr} {}                                 \
    NAME(jshackle::JavaCaller& caller) : BaseType{caller.shared_from_this()} {} \
    NAME(JNIEnv* env, jobject obj) : BaseType{Traits::MakeCaller(env, obj)} {}

#define JSHACKLE_JAVA_CLASS_CONSTRUCTOR(NAME, ...)             \
    NAME(JSHACKLE_FOREACH(JSHACKLE_NUMBER_ARG, (__VA_ARGS__))) \
        : BaseType{Traits::MakeCaller(JSHACKLE_FOREACH(JSHACKLE_NUMBER_ARG_NAMES, (__VA_ARGS__)))} {}

#define JSHACKLE_JAVA_CLASS_BEGIN(NAME, ...)                                                                    \
    struct NAME : virtual public jshackle::JavaClass JSHACKLE_FOREACH(JSHACKLE_JAVA_CLASS_MULTIPLE_INHERITANCE, \
                                                                      (__VA_ARGS__)) {                          \
        using BaseType = jshackle::JavaClass;                                                                   \
        using Traits   = jshackle::JavaTraits<NAME>;                                                            \
    JSHACKLE_JAVA_CLASS_CONSTRUCTORS(NAME)

#define JSHACKLE_JAVA_CLASS_END() \
    }                             \
    ;

#define JSHACKLE_JAVA_INTERFACE_BEGIN(NAME, INTERFACE)                   \
    struct NAME : virtual public jshackle::JavaClass, public INTERFACE { \
        using BaseType = jshackle::JavaClass;                            \
        using Traits   = jshackle::JavaInterfaceTraits<NAME, INTERFACE>; \
    JSHACKLE_JAVA_CLASS_CONSTRUCTORS(NAME)

#define JSHACKLE_JAVA_INTERFACE_END() \
    }                                 \
    ;

#define JSHACKLE_JAVA_CLASS_METHOD(RETURN_TYPE, NAME, ...)                                                       \
    RETURN_TYPE NAME(JSHACKLE_FOREACH(JSHACKLE_NUMBER_ARG, (__VA_ARGS__))) {                                     \
        assert(caller);                                                                                          \
        jshackle::JavaAttachment attachment(caller->context->jvm, caller->context->jniVersion);                  \
        const auto signature =                                                                                   \
            jshackle::JavaMethodSignature<RETURN_TYPE JSHACKLE_FOREACH(JSHACKLE_APPEND, (__VA_ARGS__))>(         \
                *caller->context);                                                                               \
        return caller->callMethod<RETURN_TYPE>(attachment.env,                                                   \
                                               Traits::sClassRef,                                                \
                                               #NAME,                                                            \
                                               signature JSHACKLE_FOREACH(JSHACKLE_TO_JAVA_ARG, (__VA_ARGS__))); \
    }

#define JSHACKLE_JAVA_CLASS_METHOD_OVERRIDE(RETURN_TYPE, NAME, ...)                                              \
    virtual RETURN_TYPE NAME(JSHACKLE_FOREACH(JSHACKLE_NUMBER_ARG, (__VA_ARGS__))) override {                    \
        assert(caller);                                                                                          \
        jshackle::JavaAttachment attachment(caller->context->jvm, caller->context->jniVersion);                  \
        const auto signature =                                                                                   \
            jshackle::JavaMethodSignature<RETURN_TYPE JSHACKLE_FOREACH(JSHACKLE_APPEND, (__VA_ARGS__))>(         \
                *caller->context);                                                                               \
        return caller->callMethod<RETURN_TYPE>(attachment.env,                                                   \
                                               Traits::sClassRef,                                                \
                                               #NAME,                                                            \
                                               signature JSHACKLE_FOREACH(JSHACKLE_TO_JAVA_ARG, (__VA_ARGS__))); \
    }

#define JSHACKLE_JAVA_CLASS_CONST_METHOD(RETURN_TYPE, NAME, ...)                                                 \
    RETURN_TYPE NAME(JSHACKLE_FOREACH(JSHACKLE_NUMBER_ARG, (__VA_ARGS__))) const {                               \
        assert(caller);                                                                                          \
        jshackle::JavaAttachment attachment(caller->context->jvm, caller->context->jniVersion);                  \
        const auto signature =                                                                                   \
            jshackle::JavaMethodSignature<RETURN_TYPE JSHACKLE_FOREACH(JSHACKLE_APPEND, (__VA_ARGS__))>(         \
                *caller->context);                                                                               \
        return caller->callMethod<RETURN_TYPE>(attachment.env,                                                   \
                                               Traits::sClassRef,                                                \
                                               #NAME,                                                            \
                                               signature JSHACKLE_FOREACH(JSHACKLE_TO_JAVA_ARG, (__VA_ARGS__))); \
    }

#define JSHACKLE_JAVA_CLASS_STATIC_METHOD(RETURN_TYPE, NAME, ...)                                        \
    static RETURN_TYPE NAME(JSHACKLE_FOREACH(JSHACKLE_NUMBER_ARG, (__VA_ARGS__))) {                      \
        assert(Traits::sJNIContext);                                                                     \
        jshackle::JavaAttachment attachment(Traits::sJNIContext->jvm, Traits::sJNIContext->jniVersion);  \
        const auto signature =                                                                           \
            jshackle::JavaMethodSignature<RETURN_TYPE JSHACKLE_FOREACH(JSHACKLE_APPEND, (__VA_ARGS__))>( \
                *Traits::sJNIContext);                                                                   \
        return jshackle::JavaCaller::callStaticMethod<RETURN_TYPE>(                                      \
            *Traits::sJNIContext,                                                                        \
            attachment.env,                                                                              \
            Traits::sClassRef,                                                                           \
            #NAME,                                                                                       \
            signature JSHACKLE_FOREACH(JSHACKLE_TO_JAVA_ARG, (__VA_ARGS__)));                            \
    }

} // namespace jshackle
