#pragma once

#include "jshackle/JavaClass.h"

#include "jshackle/android/content/Context.h"
#include "jshackle/android/content/Intent.h"

namespace android {
namespace app {

struct Activity : virtual public jshackle::JavaClass, public content::Context {
    using BaseType = jshackle::JavaClass;
    using Traits = jshackle::JavaTraits<Activity>;

    Activity(JNIEnv* env, jobject obj)
        : BaseType{Traits::MakeCaller(env, obj)}, content::Context{nullptr} {}

    JSHACKLE_JAVA_CLASS_METHOD(content::Intent*, getIntent)
};

} // namespace app
} // namespace android
