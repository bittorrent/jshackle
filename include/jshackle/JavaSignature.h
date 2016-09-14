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

namespace jshackle {

struct JNIContext;

struct NoArg {};

template <typename T = NoArg>
typename std::enable_if<std::is_same<T, NoArg>::value, std::string>::type JavaTypeSignature(JNIContext& jniContext) {
    return "";
}

template <typename T>
typename std::enable_if<!std::is_same<T, NoArg>::value, std::string>::type JavaTypeSignature(JNIContext& jniContext) {
    return ToJava<T>::Signature(jniContext);
}

template <typename Next, typename NextNext, typename... Rem>
std::string JavaTypeSignature(JNIContext& jniContext) {
    return JavaTypeSignature<Next>(jniContext) + JavaTypeSignature<NextNext>(jniContext) +
           JavaTypeSignature<Rem...>(jniContext);
}

template <typename R, typename... Args>
std::string JavaMethodSignature(JNIContext& jniContext) {
    return std::string("(") + JavaTypeSignature<Args...>(jniContext) + ")" + JavaTypeSignature<R>(jniContext);
}

} // namespace jshackle
