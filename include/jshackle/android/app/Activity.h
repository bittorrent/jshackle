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

#include "jshackle/android/content/Context.h"
#include "jshackle/android/content/Intent.h"
#include "jshackle/android/content/SharedPreferences.h"

namespace android {
namespace app {

struct Activity : virtual public jshackle::JavaClass, public content::Context {
    using BaseType = jshackle::JavaClass;
    using Traits = jshackle::JavaTraits<Activity>;

    Activity(JNIEnv* env, jobject obj)
        : BaseType{Traits::MakeCaller(env, obj)}, content::Context{nullptr} {}

    JSHACKLE_JAVA_CLASS_METHOD(content::Intent*, getIntent);
    JSHACKLE_JAVA_CLASS_METHOD(content::SharedPreferences*, getPreferences, int);
};

} // namespace app
} // namespace android
