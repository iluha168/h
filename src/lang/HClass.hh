#ifndef HCLASS_HH
#define HCLASS_HH

#include <X11/Xlib.h>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <iostream>
#include <variant>

namespace H {
    class Class;
    typedef Class* LClass;

    class Class {
        class Object;
    public:
        typedef std::shared_ptr<Object> LObject;
        typedef std::vector<LObject> LObjects;
        typedef LObject(*Function)(LObjects&);

        static LObjects refs;
        static void addref(LObject);
        static void unref(LObject);
    private:

        class Object {
        public:
            Class* parent;
            Object(decltype(parent));
            
            // native data of object
            std::variant<\
                std::array<double,4>, // for H::Number
                std::pair<::Window, ::GC>, // for H::Window
                std::wstring, // for H::String
                bool //for H::Boolean
                #define rawNumber(var) (std::get<std::array<double,4>>(var->data))
                #define rawrawWin(var) (std::get<std::pair<::Window, ::GC>>(var->data))
                #define rawWin(var) (rawrawWin(var).first)
                #define rawGC(var) (rawrawWin(var).second)
                #define rawString(var) (std::get<std::wstring>(var->data))
                #define rawBool(var) (std::get<bool>(var->data))
            > data;
        };
    public:
        std::wstring name;
        std::map<std::wstring, Function> prototype;

        Class(decltype(name), decltype(prototype));
        LObject instantiate(LObjects = {});
        ~Class();
    };

    extern Class::Function emptyF;
    extern Class::LObject null;

    //Index 0 - false, index 1 - true
    extern Class::LObject Booleans[2];

    extern LClass Number, Window, String, Uninitialized, Boolean;
}

#endif