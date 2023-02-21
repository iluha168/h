#include "HClass.hh"

namespace H {
    LObject HFunctionFromNativeFunction(NativeFunction pointer){
        LObject f = Object::instantiate(Function);
        f->data.function = pointer;
        return f;
    }

    DEFINE_H_CLASS(Function)
        FunctionProto = {
            {L"$new", LObject(new Object(FunctionObjectProto))},
        };
        Function = LObject(new Object(FunctionProto));
    DEFINE_H_CLASS_END(Function)
}