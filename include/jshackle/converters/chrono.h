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
