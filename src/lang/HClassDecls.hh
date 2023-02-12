#ifndef HCLASSDECLS_HH
#define HCLASSDECLS_HH

#include <vector>
#include <memory>
#include <map>
#define DECLARE_H_CLASS(name) extern LObject name;\
    extern ObjectData name##Proto, name##ObjectProto;\
    extern void name##Init();
#define IMPLEMENT_H_CLASS(name) LObject name;\
    ObjectData name##Proto, name##ObjectProto;\
    void name##Init(){
#define IMPLEMENT_H_CLASS_END(name) }

namespace H {
    class Object;
    typedef std::shared_ptr<Object> LObject;
    typedef std::vector<LObject> LObjects;
    typedef LObject(*NativeFunction)(LObjects&);

    typedef std::map<std::wstring, LObject> ObjectData;
    typedef ObjectData VarScope;
    typedef std::array<float,4> Quaternion;

    LObject HStringFromString(std::wstring);
    LObject HNumberFromQuaternion(Quaternion);
    LObject HFunctionFromNativeFunction(NativeFunction);

    extern LObject null, emptyF;

    //Index 0 - false, index 1 - true
    extern LObject Booleans[2];
    extern std::vector<void(*)()> ClassInits;

    DECLARE_H_CLASS(Function)
    DECLARE_H_CLASS(Uninitialized)
    DECLARE_H_CLASS(Number)
    DECLARE_H_CLASS(String)
    DECLARE_H_CLASS(Boolean)
    DECLARE_H_CLASS(Window)
    DECLARE_H_CLASS(Array)
}

#undef DECLARE_H_CLASS
#endif