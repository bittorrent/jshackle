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
        JSHACKLE_JAVA_CLASS_METHOD(Editor*, putFloat, const std::string&, float);
        JSHACKLE_JAVA_CLASS_METHOD(Editor*, putBoolean, const std::string&, bool);
    JSHACKLE_JAVA_CLASS_END();

    JSHACKLE_JAVA_CLASS_METHOD(Editor*, edit);

    JSHACKLE_JAVA_CLASS_METHOD(bool, contains, const std::string&);

    JSHACKLE_JAVA_CLASS_METHOD(std::string, getString, const std::string&, std::string);
    JSHACKLE_JAVA_CLASS_METHOD(int, getInt, const std::string&, int);
    JSHACKLE_JAVA_CLASS_METHOD(float, getFloat, const std::string&, float);
    JSHACKLE_JAVA_CLASS_METHOD(bool, getBoolean, const std::string&, bool);
JSHACKLE_JAVA_CLASS_END();

} // namespace content
} // namespace android
