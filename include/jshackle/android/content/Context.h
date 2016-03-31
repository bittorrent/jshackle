#pragma once

#include "jshackle/JavaClass.h"

#include "jshackle/android/content/res/AssetManager.h"

namespace android {
namespace content {

JSHACKLE_JAVA_CLASS_BEGIN(Context);
    JSHACKLE_JAVA_CLASS_METHOD(res::AssetManager*, getAssets);
JSHACKLE_JAVA_CLASS_END();

} // namespace content
} // namespace android
