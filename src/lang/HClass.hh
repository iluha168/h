#ifndef HCLASS_HH
#define HCLASS_HH

#include <X11/Xlib.h>
#include <string>
#include <sstream>
#include <map>
#include <iostream>
#include <variant>

#include "HClassDecls.hh"

namespace H {
    class Object {
    public:
        Class* parent;
        Object(decltype(parent));
        
        // native data of object
        std::variant<\
            Quaternion, // for H::Number
            std::pair<::Window, ::GC>, // for H::Window
            std::wstring, // for H::String
            bool, //for H::Boolean
            LObjects //for H::Array
            #define rawNumber(var) (std::get<std::array<double,4>>(var->data))
            #define rawrawWin(var) (std::get<std::pair<::Window, ::GC>>(var->data))
            #define rawWin(var) (rawrawWin(var).first)
            #define rawGC(var) (rawrawWin(var).second)
            #define rawString(var) (std::get<std::wstring>(var->data))
            #define rawBool(var) (std::get<bool>(var->data))
            #define rawArray(var) (std::get<H::LObjects>(var->data))
        > data;
    };

    class Class {
    public:
        static LObjects refs;
        static void addref(LObject);
        static void unref(LObject);
    public:
        std::wstring name;
        std::map<std::wstring, NativeFunction> prototype;

        Class(decltype(name), decltype(prototype));
        LObject instantiate(LObjects = {});
        ~Class();
    };
}

#endif