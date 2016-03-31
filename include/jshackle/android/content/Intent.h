#pragma once

#include "jshackle/JavaClass.h"

namespace android {
namespace content {

JSHACKLE_JAVA_CLASS_BEGIN(Intent);
    JSHACKLE_JAVA_CLASS_METHOD(std::string, getDataString);
JSHACKLE_JAVA_CLASS_END();

} // namespace content
} // namespace android
