# jShackle

JNI programming sucks. This is a collection of macros we use to make it suck a little bit less, and be a little more fool proof.

This repo is extremely macro and template heavy, and documentation is nearly non-existent at the moment. If at all possible, the easiest thing is to look at our usage in other codebases, and copy that.

But if you're curious, the basics look something like this:

#### Exposing a Java class implementation to C++

```java
public class JavaThing {
    public int foo() {
        return 0;
    }
}
```

```c++
JSHACKLE_JAVA_CLASS_BEGIN(JavaThing);
    JSHACKLE_JAVA_CLASS_METHOD(int, foo);
JSHACKLE_JAVA_CLASS_END();

int main() {
    JavaThing x;
    return x.foo();
}
```

#### Exposing a C++ implementation to Java

```c++
class CPPThing {
public:
    int foo() {
        return 0;
    }
};

JSHACKLE_NATIVE_CLASS_BEGIN(NativeCPPThing, CPPThing);
    JSHACKLE_NATIVE_CLASS_METHOD(int, foo);
JSHACKLE_NATIVE_CLASS_END();
```

```java
public class NativeCPPThing {
    public native int foo();

    private long _nativeHandle = 0;
}
```

Constructors, inheritance, and arbitrary argument types are all supported so you can pretty easily entirely abstract away the boundary between your Java and C++.

# License

jShackle is provided under the Apache License, which can be found in the LICENSE file.
