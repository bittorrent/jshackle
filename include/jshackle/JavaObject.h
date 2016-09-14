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

#include "jshackle/JavaAttachment.h"
#include "jshackle/JNIContext.h"
#include "jshackle/utility.h"

#include <jni.h>

namespace jshackle {

struct JavaObject {
    JavaObject() {}
    JavaObject(std::nullptr_t) {}

    JavaObject(const JavaObject& other) : JavaObject(other._context, other._object) {}

    JavaObject(JavaObject&& other) {
        std::swap(_context, other._context);
        std::swap(_object, other._object);
    }

    JavaObject(JNIContext* context, jobject object) : _context(context) {
        if (object) {
            JavaAttachment attachment(context->jvm, context->jniVersion);
            _object = attachment.env->NewGlobalRef(object);
        }
    }

    ~JavaObject() {
        if (_object) {
            JavaAttachment attachment(_context->jvm, _context->jniVersion);
            attachment.env->DeleteGlobalRef(_object);
        }
    }

    operator jobject() const { return _object; }
    explicit operator bool() const { return _object; }

    JNIContext* _context = nullptr;
    jobject _object = nullptr;
};

#define JSHACKLE_JAVA_OBJECT_TYPE(TYPE, NAME)                                                      \
    struct TYPE : jshackle::JavaObject {                                                  \
        using jshackle::JavaObject::JavaObject;                                           \
        static constexpr const char* Name() { return NAME; }                                     \
        TYPE& operator=(const TYPE& other) {                                                     \
            if (_object) {                                                                       \
                jshackle::JavaAttachment attachment(_context->jvm, _context->jniVersion); \
                attachment.env->DeleteGlobalRef(_object);                                        \
            }                                                                                    \
            _context = other._context;                                                           \
            if (other._object) {                                                                 \
                jshackle::JavaAttachment attachment(_context->jvm, _context->jniVersion); \
                _object = attachment.env->NewGlobalRef(other._object);                           \
            } else { _object = nullptr; }                                                        \
            return *this;                                                                        \
        }                                                                                        \
    };

} // namespace jshackle
