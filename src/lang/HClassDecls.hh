#ifndef HCLASSDECLS_HH
#define HCLASSDECLS_HH

#include <vector>
#include <memory>

namespace H {
    class Object;
    typedef std::shared_ptr<Object> LObject;
    typedef std::vector<LObject> LObjects;
    typedef LObject(*NativeFunction)(LObjects&);

    typedef std::array<double,4> Quaternion;

    class Class;
    typedef Class* LClass;

    LObject HStringFromString(std::wstring);

    extern NativeFunction emptyF;
    extern LObject null;

    //Index 0 - false, index 1 - true
    extern LObject Booleans[2];

    extern LClass Number, Window, String, Uninitialized, Boolean, Array;
}

#endif