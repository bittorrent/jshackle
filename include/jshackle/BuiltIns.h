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
