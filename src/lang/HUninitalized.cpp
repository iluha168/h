#include "HClass.hh"

namespace H {
    IMPLEMENT_H_CLASS(Uninitialized)
        UninitializedObjectProto = {
            {L"toString", HFunctionFromNativeFunction([](LObjects&){
                return H::HStringFromString(L"null");
            })}
        };
    
        UninitializedProto = {
            {L"$new", LObject(new Object(UninitializedObjectProto))},
        };
        Uninitialized = LObject(new Object(UninitializedProto));
    IMPLEMENT_H_CLASS_END(Uninitialized)
}