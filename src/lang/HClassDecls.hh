#ifndef HCLASSDECLS_HH
#define HCLASSDECLS_HH

#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <future>
#include <X11/Xlib.h>

#define DECLARE_H_CLASS(name) extern LObject name;\
    extern ObjectData name##Proto, name##ObjectProto;\
    extern void name##Init();
#define DEFINE_H_CLASS(name) LObject name;\
    ObjectData name##Proto, name##ObjectProto;\
    void name##Init(){
#define DEFINE_H_CLASS_END(name) }

namespace H {
    class Object;
    typedef std::shared_ptr<Object> LObject;
    typedef std::vector<LObject> LObjects;
    typedef LObject(*FunctionType)(LObjects&);

    typedef std::map<std::wstring, LObject> ObjectData;
    typedef ObjectData VarScope;

    typedef std::array<float,4> Quaternion;
    typedef std::pair<::Window, ::GC> WindowInfo;

    LObject HStringFromString(std::wstring);
    LObject HNumberFromQuaternion(Quaternion);
    LObject HNativeFunctionFromFunctionType(FunctionType);

    extern LObject null, emptyF;

    //Index 0 - false, index 1 - true
    extern LObject Booleans[2];

    DECLARE_H_CLASS(NativeFunction)
    DECLARE_H_CLASS(Uninitialized)
    DECLARE_H_CLASS(Number)
    DECLARE_H_CLASS(String)
    DECLARE_H_CLASS(Boolean)
    DECLARE_H_CLASS(Window)
    DECLARE_H_CLASS(Array)
}

#undef DECLARE_H_CLASS
#endif
