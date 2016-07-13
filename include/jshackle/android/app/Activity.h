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
