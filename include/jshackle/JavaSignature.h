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
