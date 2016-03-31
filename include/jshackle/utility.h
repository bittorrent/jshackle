#pragma once

#include "jshackle/config.h"

#include <string>
#include <type_traits>
#include <vector>

#if __ANDROID__
#include <android/log.h>
#define JSHACKLE_LOG(...) __android_log_print(ANDROID_LOG_INFO, "jni", __VA_ARGS__)
#else
#include <stdio>
#define JSHACKLE_LOG(...) printf(__VA_ARGS__)
#endif

namespace jshackle {

#define JSHACKLE_HAS_COMMA_(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, ...) _15
#define JSHACKLE_HAS_COMMA(...) JSHACKLE_HAS_COMMA_(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define JSHACKLE_TRIGGER_PARENTHESIS_(...) ,

#define JSHACKLE_IS_EMPTY(...)                                                                                             \
        JSHACKLE_IS_EMPTY_(JSHACKLE_HAS_COMMA(__VA_ARGS__), JSHACKLE_HAS_COMMA(JSHACKLE_TRIGGER_PARENTHESIS_ __VA_ARGS__), \
        JSHACKLE_HAS_COMMA(__VA_ARGS__(/*empty*/)),                                                                        \
        JSHACKLE_HAS_COMMA(JSHACKLE_TRIGGER_PARENTHESIS_ __VA_ARGS__(/*empty*/)))

#define JSHACKLE_PASTE5(_0, _1, _2, _3, _4) _0##_1##_2##_3##_4
#define JSHACKLE_IS_EMPTY_(_0, _1, _2, _3) JSHACKLE_HAS_COMMA(JSHACKLE_PASTE5(JSHACKLE_IS_EMPTY_CASE_, _0, _1, _2, _3))
#define JSHACKLE_IS_EMPTY_CASE_0001 ,

#define JSHACKLE_NUM_ARGS_1(...) 0
#define JSHACKLE_NUM_ARGS_0_(A100, A99, A98, A97, A96, A95, A94, A93, A92, A91, A90, A89, A88, A87, A86, A85, A84, A83, \
                             A82, A81, A80, A79, A78, A77, A76, A75, A74, A73, A72, A71, A70, A69, A68, A67, A66, A65,  \
                             A64, A63, A62, A61, A60, A59, A58, A57, A56, A55, A54, A53, A52, A51, A50, A49, A48, A47,  \
                             A46, A45, A44, A43, A42, A41, A40, A39, A38, A37, A36, A35, A34, A33, A32, A31, A30, A29,  \
                             A28, A27, A26, A25, A24, A23, A22, A21, A20, A19, A18, A17, A16, A15, A14, A13, A12, A11,  \
                             A10, A9, A8, A7, A6, A5, A4, A3, A2, A1, N, ...)                                           \
    N
#define JSHACKLE_NUM_ARGS_0(...)                                                                                         \
    JSHACKLE_NUM_ARGS_0_(__VA_ARGS__, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81,   \
                         80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, \
                         56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, \
                         32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9,  \
                         8, 7, 6, 5, 4, 3, 2, 1)
#define JSHACKLE_NUM_ARGS_FUNC__(EMPTY, ...) JSHACKLE_NUM_ARGS_##EMPTY
#define JSHACKLE_NUM_ARGS_FUNC_(EMPTY, ...) JSHACKLE_NUM_ARGS_FUNC__(EMPTY, __VA_ARGS__)
#define JSHACKLE_NUM_ARGS_FUNC(...) JSHACKLE_NUM_ARGS_FUNC_(JSHACKLE_IS_EMPTY(__VA_ARGS__), __VA_ARGS__)
#define JSHACKLE_NUM_ARGS(...) JSHACKLE_NUM_ARGS_FUNC(__VA_ARGS__)(__VA_ARGS__)

#define JSHACKLE_EXPAND(X) X
#define JSHACKLE_EXPAND_VA(...) __VA_ARGS__
#define JSHACKLE_FIRST_ARG(X, ...) (X)
#define JSHACKLE_REM_ARGS(X, ...) (__VA_ARGS__)
#define JSHACKLE_FOREACH(MACRO, LIST) JSHACKLE_FOREACH_(JSHACKLE_NUM_ARGS LIST, MACRO, LIST)
#define JSHACKLE_FOREACH_(N, M, LIST) JSHACKLE_FOREACH__(N, M, JSHACKLE_EXPAND_VA LIST)
#define JSHACKLE_FOREACH__(N, M, LIST) JSHACKLE_FOREACH_##N(M, LIST)
#define JSHACKLE_INVOKE_(M, ...) M(__VA_ARGS__)
#define JSHACKLE_INVOKE(M, ...) JSHACKLE_INVOKE_(M, __VA_ARGS__)
#define JSHACKLE_FOREACH_0(M, X)
#define JSHACKLE_FOREACH_1(M, A1) JSHACKLE_INVOKE(M, 0, A1)
#define JSHACKLE_FOREACH_2(M, A1, A2) JSHACKLE_FOREACH_1(M, A1) JSHACKLE_INVOKE(M, 1, A2)
#define JSHACKLE_FOREACH_3(M, A1, A2, A3) JSHACKLE_FOREACH_2(M, A1, A2) JSHACKLE_INVOKE(M, 2, A3)
#define JSHACKLE_FOREACH_4(M, A1, A2, A3, A4) JSHACKLE_FOREACH_3(M, A1, A2, A3) JSHACKLE_INVOKE(M, 3, A4)
#define JSHACKLE_FOREACH_5(M, A1, A2, A3, A4, A5) JSHACKLE_FOREACH_4(M, A1, A2, A3, A4) JSHACKLE_INVOKE(M, 4, A5)
#define JSHACKLE_FOREACH_6(M, A1, A2, A3, A4, A5, A6) JSHACKLE_FOREACH_5(M, A1, A2, A3, A4, A5) JSHACKLE_INVOKE(M, 5, A6)
#define JSHACKLE_FOREACH_7(M, A1, A2, A3, A4, A5, A6, A7) \ JSHACKLE_FOREACH_6(M, A1, A2, A3, A4, A5, A6) JSHACKLE_INVOKE(M, 6, A7)
// add as many as needed

#define JSHACKLE_COMMA_IF_NONZERO(N) JSHACKLE_COMMA_IF_NONZERO_##N
#define JSHACKLE_COMMA_IF_NONZERO_0
#define JSHACKLE_COMMA_IF_NONZERO_1 ,
#define JSHACKLE_COMMA_IF_NONZERO_2 ,
#define JSHACKLE_COMMA_IF_NONZERO_3 ,
#define JSHACKLE_COMMA_IF_NONZERO_4 ,
#define JSHACKLE_COMMA_IF_NONZERO_5 ,
#define JSHACKLE_COMMA_IF_NONZERO_6 ,
// add as many as needed

#define JSHACKLE_NUMBER_ARG_(N, T) JSHACKLE_COMMA_IF_NONZERO(N) T arg##N
#define JSHACKLE_NUMBER_ARG(N, T) JSHACKLE_NUMBER_ARG_(N, T)

#define JSHACKLE_NUMBER_ARG_NAMES_(N) JSHACKLE_COMMA_IF_NONZERO(N) arg##N
#define JSHACKLE_NUMBER_ARG_NAMES(N, X) JSHACKLE_NUMBER_ARG_NAMES_(N)

#define JSHACKLE_NUMBER_JAVA_ARG(N, T) , jshackle::ToJava<T>::Type arg##N

#define JSHACKLE_TO_NATIVE_CONSTRUCTOR_ARG_(N, T) \
        JSHACKLE_COMMA_IF_NONZERO(N) jshackle::ToNative<decltype(arg##N), T>::Convert(*sJNIContext, env, arg##N)
#define JSHACKLE_TO_NATIVE_CONSTRUCTOR_ARG(N, T) JSHACKLE_TO_NATIVE_CONSTRUCTOR_ARG_(N, T)

#define JSHACKLE_VAR_TO_DECLTYPE(N, V) , decltype(V)

#define JSHACKLE_APPEND(N, T) , T

#define JSHACKLE_TYPE_(N, T) JSHACKLE_COMMA_IF_NONZERO(N) T
#define JSHACKLE_TYPE(N, T) JSHACKLE_TYPE_(N, T)

// XXX: to convert correctly from our native handle to the appropriate type (only known at runtime), even with multiple
// inheritance, we use a map of offsets to add to the base pointer
#define JSHACKLE_POINTER_CONVERSION(N, T)                                                \
    conversions[jshackle::TypeIdentifier<T>()()] =                                       \
        reinterpret_cast<char*>(dynamic_cast<T*>(reinterpret_cast<Type*>(0x80000000))) - \
        reinterpret_cast<char*>(reinterpret_cast<Type*>(0x80000000));

} // namespace jshackle
