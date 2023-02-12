#include "HClass.hh"

namespace H {
    IMPLEMENT_H_CLASS(Boolean)
        BooleanObjectProto = {
            {L"constructor", HFunctionFromNativeFunction([](LObjects& o){
                o[0]->data = false;
                return null;
            })},

            {L"toString", HFunctionFromNativeFunction([](LObjects& o){
                return H::HStringFromString(rawBool(o[0])?L"true":L"false");
            })},
            {L"not", HFunctionFromNativeFunction([](LObjects& o){
                return H::Booleans[!rawBool(o.at(0))];
            })},

            {L"+", HFunctionFromNativeFunction([](LObjects& o){
                LObject result = Object::instantiate(Boolean);
                result->data = false;
                for(LObject& b : o)
                    result->data = rawBool(result) || rawBool(b);
                return result;
            })},
            {L"*", HFunctionFromNativeFunction([](LObjects& o){
                LObject result = Object::instantiate(Boolean);
                result->data = true;
                for(LObject& b : o)
                    result->data = rawBool(result) && rawBool(b);
                return result;
            })},
        };

        BooleanProto = {
            {L"$new", LObject(new Object(BooleanObjectProto))},
        };
        Boolean = LObject(new Object(BooleanProto));
    IMPLEMENT_H_CLASS_END(Boolean)
}