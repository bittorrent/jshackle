#pragma once

#include <jni.h>

static_assert(sizeof(jlong) >= sizeof(void*), "unable to store pointers in jlong");
