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

#include "jshackle/JavaClass.h"

namespace android {
namespace content {

JSHACKLE_JAVA_CLASS_BEGIN(SharedPreferences);
    JSHACKLE_JAVA_CLASS_BEGIN(Editor);
        JSHACKLE_JAVA_CLASS_METHOD(void, apply);
        JSHACKLE_JAVA_CLASS_METHOD(Editor*, remove, const std::string&);

        JSHACKLE_JAVA_CLASS_METHOD(Editor*, putString, const std::string&, const std::string&);
        JSHACKLE_JAVA_CLASS_METHOD(Editor*, putInt, const std::string&, int);
        JSHACKLE_JAVA_CLASS_METHOD(Editor*, putLong, const std::string&, long long);
        JSHACKLE_JAVA_CLASS_METHOD(Editor*, putFloat, const std::string&, float);
        JSHACKLE_JAVA_CLASS_METHOD(Editor*, putBoolean, const std::string&, bool);
    JSHACKLE_JAVA_CLASS_END();

    JSHACKLE_JAVA_CLASS_METHOD(Editor*, edit);

    JSHACKLE_JAVA_CLASS_METHOD(bool, contains, const std::string&);

    JSHACKLE_JAVA_CLASS_METHOD(std::string, getString, const std::string&, std::string);
    JSHACKLE_JAVA_CLASS_METHOD(int, getInt, const std::string&, int);
    JSHACKLE_JAVA_CLASS_METHOD(long long, getLong, const std::string&, long long);
    JSHACKLE_JAVA_CLASS_METHOD(float, getFloat, const std::string&, float);
    JSHACKLE_JAVA_CLASS_METHOD(bool, getBoolean, const std::string&, bool);
JSHACKLE_JAVA_CLASS_END();

} // namespace content
} // namespace android
