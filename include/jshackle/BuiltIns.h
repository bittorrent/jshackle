#pragma once

#include "jshackle/config.h"

#include "jshackle/android/app/Activity.h"
#include "jshackle/android/content/Context.h"
#include "jshackle/android/content/Intent.h"
#include "jshackle/android/content/res/AssetManager.h"

namespace jshackle {

inline void RegisterBuiltIns(JNIContext* context) {
    ::android::content::res::AssetManager::Traits::Register(context, "android/content/res/AssetManager");
    ::android::content::Intent::Traits::Register(context, "android/content/Intent");
    ::android::content::Context::Traits::Register(context, "android/content/Context");
    ::android::content::SharedPreferences::Traits::Register(context, "android/content/SharedPreferences");
    ::android::content::SharedPreferences::Editor::Traits::Register(context, "android/content/SharedPreferences$Editor");
    ::android::app::Activity::Traits::Register(context, "android/app/Activity");
}

} // namespace jshackle
