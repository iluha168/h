#ifndef HCLASS_HH
#define HCLASS_HH

#include <X11/Xlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <variant>

#include "HClassDecls.hh"
#include "../global/Global.hh"
#include "../script/parser.hh"
#include "../script/runner.hh"

namespace H {
    class Object {
    public:
        static LObjects refs;
        static void addref(LObject);
        static void unref(LObject);

        Proto& prototype;
        LObject parent;
        Object(Proto&, LObject = null);

        static LObject instantiate(LObject&, LObjects = {});
        LObject call(std::wstring, LObjects&);

        // native data of object
        std::variant<\
            Quaternion, // for H::Number
            std::pair<::Window, ::GC>, // for H::Window
            std::wstring, // for H::String
            bool, //for H::Boolean
            LObjects, //for H::Array
            Parser::LSyntaxTree //for H::Function
            #define rawNumber(var) (std::get<H::Quaternion>(var->data))
            #define rawrawWin(var) (std::get<std::pair<::Window, ::GC>>(var->data))
            #define rawWin(var) (rawrawWin(var).first)
            #define rawGC(var) (rawrawWin(var).second)
            #define rawString(var) (std::get<std::wstring>(var->data))
            #define rawBool(var) (std::get<bool>(var->data))
            #define rawArray(var) (std::get<H::LObjects>(var->data))
        > data;
    };
}

#endif