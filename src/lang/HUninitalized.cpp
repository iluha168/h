#include "HClass.hh"

namespace H {
    DEFINE_H_CLASS(Uninitialized)
        UninitializedObjectProto = {
            {Global::Strings::toString, HNativeFunctionFromFunctionType([](LObjects&){
                return H::HStringFromString(L"null");
            })}
        };
    
        UninitializedProto = {
            {L"$new", LObject(new Object(UninitializedObjectProto))},
        };
        Uninitialized = LObject(new Object(UninitializedProto));
    DEFINE_H_CLASS_END(Uninitialized)
}