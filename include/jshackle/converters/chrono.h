#pragma once

#include "jshackle/config.h"

#include "jshackle/ToJava.h"
#include "jshackle/ToNative.h"

#include <chrono>

namespace jshackle {

template <>
struct ToJava<std::chrono::steady_clock::time_point> {
    typedef jlong Type;
    static constexpr const char* Signature(JNIContext& jniContext) { return "J"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, const std::chrono::steady_clock::time_point& tp) { return static_cast<Type>(std::chrono::duration_cast<std::chrono::microseconds>(tp.time_since_epoch()).count()); }
};

template <>
struct ToNative<jlong, std::chrono::steady_clock::time_point> {
    typedef const std::chrono::steady_clock::time_point Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jlong us) { return Type(std::chrono::microseconds(us)); }
};

template <>
struct ToJava<std::chrono::microseconds> {
    typedef jlong Type;
    static constexpr const char* Signature(JNIContext& jniContext) { return "J"; }
    static Type Convert(JNIContext& jniContext, JNIEnv* env, const std::chrono::microseconds& d) { return static_cast<Type>(d.count()); }
};

template <>
struct ToNative<jlong, std::chrono::microseconds> {
    typedef const std::chrono::microseconds Type;
    static Type Convert(JNIContext& jniContext, JNIEnv* env, jlong us) { return Type(us); }
};

} // namespace jshackle
