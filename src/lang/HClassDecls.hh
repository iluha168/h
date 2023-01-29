#ifndef HCLASSDECLS_HH
#define HCLASSDECLS_HH

#include <vector>
#include <memory>
#include <map>

namespace H {
    class Object;
    typedef std::shared_ptr<Object> LObject;
    typedef std::vector<LObject> LObjects;
    typedef LObject(*NativeFunction)(LObjects&);
    typedef std::map<std::wstring, H::LObject> Entries;

    typedef std::map<std::wstring, NativeFunction> Proto;
    typedef std::array<float,4> Quaternion;

    LObject HStringFromString(std::wstring);
    LObject HNumberFromQuaternion(Quaternion);

    extern NativeFunction emptyF;
    extern LObject null;

    //Index 0 - false, index 1 - true
    extern LObject Booleans[2];

    extern LObject Number, Window, String, Uninitialized, Boolean, Array;
}

#endif