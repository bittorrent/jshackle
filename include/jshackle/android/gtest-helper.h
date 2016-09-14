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

#include <gtest/gtest.h>

#include "jshackle/BuiltIns.h"

#include <android/log.h>
#include <thread>

#define JSHACKLE_GTEST_MAIN(app_name, package_name, JNI_register_func)                             \
                                                                                                   \
namespace {                                                                                        \
    std::unique_ptr<jshackle::JNIContext> gJNIContext;                                             \
                                                                                                   \
    void logFD(FILE* f, int fd, int level) {                                                       \
        int pair[2];                                                                               \
        setvbuf(f, 0, _IOLBF, 0);                                                                  \
        pipe(pair);                                                                                \
        dup2(pair[1], fd);                                                                         \
        return std::thread([=, fd = pair[0]] {                                                     \
            ssize_t bytes = 0;                                                                     \
            char buf[512];                                                                         \
            while ((bytes = read(fd, buf, sizeof(buf) - 1)) > 0) {                                 \
                while (bytes > 0 && buf[bytes - 1] == '\n') {                                      \
                    --bytes;                                                                       \
                }                                                                                  \
                buf[bytes] = '\0';                                                                 \
                __android_log_write(level, app_name, buf);                                         \
            }                                                                                      \
        }).detach();                                                                               \
    }                                                                                              \
}                                                                                                  \
                                                                                                   \
extern "C" {                                                                                       \
                                                                                                   \
JNIEXPORT int JNICALL Java_ ## package_name ## _MainActivity_runTests(JNIEnv* env, jobject obj) {  \
    __android_log_print(ANDROID_LOG_INFO, app_name, "initializing");                               \
                                                                                                   \
    logFD(stdout, 1, ANDROID_LOG_INFO);                                                            \
    logFD(stderr, 2, ANDROID_LOG_ERROR);                                                           \
                                                                                                   \
    JavaVM* jvm = nullptr;                                                                         \
    env->GetJavaVM(&jvm);                                                                          \
    gJNIContext = std::make_unique<jshackle::JNIContext>(jvm, JNI_VERSION_1_4);                    \
                                                                                                   \
    JNI_register_func(gJNIContext.get());                                                          \
                                                                                                   \
    int argc = 1;                                                                                  \
    const char* argv[] = {app_name};                                                               \
    testing::InitGoogleTest(&argc, const_cast<char**>(argv));                                      \
    int result = RUN_ALL_TESTS();                                                                  \
    __android_log_print(ANDROID_LOG_INFO, app_name, "test result = %d", result);                   \
    return result;                                                                                 \
}                                                                                                  \
                                                                                                   \
} // extern "C"
