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

        ObjectData& entries;
        LObject parent;
        Object(ObjectData&, LObject = null);

        static LObject instantiate(LObject&, LObjects = {});
        LObject call(std::wstring, LObjects&);

        union {
            Quaternion* number;
            WindowInfo* window;
            std::wstring* string;
            bool boolean;
            LObjects* array;
            NativeFunction function;
        } data;
    };
}

#endif