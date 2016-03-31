#pragma once

#include "jshackle/config.h"

#include "jshackle/JavaAttachment.h"
#include "jshackle/utility.h"

#include <cassert>
#include <list>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace jshackle {

struct JavaClassInfo {
    enum Type {
        kTypeJava,
        kTypeNative
    };

    JavaClassInfo(const std::string& name, Type type) : name(name), type(type) {}

    std::string name;
    Type type = kTypeJava;

    std::string signature() const {
        return std::string("L") + name + ';';
    }
};

typedef void (*TypeIdentification)();

template <typename T>
struct TypeIdentifier {
    static void Identifier() {}
    TypeIdentification operator()() const { return &Identifier; }
};

struct JNIContext {
    using InterfaceFunc = void* (*)(JNIEnv* env, jobject obj);

    struct JavaProperties {
        std::shared_ptr<const JavaClassInfo> info;
        InterfaceFunc interfaceFunc;
    };

    struct NativeProperties {
        std::shared_ptr<const JavaClassInfo> info;
        jclass javaClass;
        std::unordered_map<TypeIdentification, std::ptrdiff_t> pointerConversions;
    };

    JNIContext(JavaVM* jvm, jint jniVersion) : jvm(jvm), jniVersion(jniVersion) {}

    mutable std::mutex mutex;

    JavaVM* const jvm = nullptr;
    const jint jniVersion = JNI_VERSION_1_6;
    std::unordered_map<TypeIdentification, std::shared_ptr<const JavaClassInfo>> classInfo;
    std::unordered_map<TypeIdentification, NativeProperties> nativeClasses;
    std::unordered_map<TypeIdentification, JavaProperties> javaClasses;

    std::unordered_map<void*, jweak> ownedObjects; // void* points directly to the native implementation itself

    struct Wrapper {
        jweak object;
        void* pointer;
        TypeIdentification type;
        std::function<void(void*)> deleter;
    };

    std::list<Wrapper> wrapped;

    std::list<Wrapper>::iterator remove(JNIEnv* env, std::list<Wrapper>::iterator it) {
        env->DeleteWeakGlobalRef(it->object);
        it->deleter(it->pointer);
        return wrapped.erase(it);
    }

    template <typename T>
    std::shared_ptr<const JavaClassInfo> findClassInfo() const {
        std::unique_lock<std::mutex> lock(mutex);
        const auto identifier = TypeIdentifier<T>()();
        auto it = classInfo.find(identifier);
        assert(it != classInfo.end());
        return it->second;
    }

    template <typename T>
    void registerJavaClass(const std::string& javaName, InterfaceFunc interfaceFunc) {
        const auto identifier = TypeIdentifier<T>()();
        auto info = std::make_shared<JavaClassInfo>(javaName, JavaClassInfo::kTypeJava);
        auto result = classInfo.emplace(identifier, info);
        if (!result.second) {
            JSHACKLE_LOG("%s class already is registered", javaName.c_str());
            assert(false);
        }
        javaClasses.emplace(identifier, std::move(JavaProperties{info, std::move(interfaceFunc)}));
    }

    template <typename T>
    void registerNativeClass(const std::string& javaName, const std::vector<JNINativeMethod>& nativeMethods, std::unordered_map<TypeIdentification, std::ptrdiff_t> pointerConversions) {
        const auto identifier = TypeIdentifier<T>()();
        auto info = std::make_shared<JavaClassInfo>(javaName, JavaClassInfo::kTypeNative);
        classInfo.emplace(identifier, info);

        JavaAttachment attachment(jvm, jniVersion);
        jclass javaClass = attachment.env->FindClass(javaName.c_str());
        assert(javaClass);
        attachment.env->RegisterNatives(javaClass, nativeMethods.data(), nativeMethods.size());
        auto globalRef = reinterpret_cast<jclass>(attachment.env->NewGlobalRef(javaClass));
        nativeClasses.emplace(identifier, std::move(NativeProperties{info, globalRef, std::move(pointerConversions)}));
    }
};

} // namespace jshackle
