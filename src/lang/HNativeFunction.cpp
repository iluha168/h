#include "HClass.hh"

namespace H {
    LObject HNativeFunctionFromFunctionType(FunctionType pointer){
        LObject f = Object::instantiate(NativeFunction);
        f->data.function = pointer;
        return f;
    }

    DEFINE_H_CLASS(NativeFunction)
        NativeFunctionProto = {
            {L"$new", LObject(new Object(NativeFunctionObjectProto))},
        };
        NativeFunction = LObject(new Object(NativeFunctionProto));
    DEFINE_H_CLASS_END(NativeFunction)
}
