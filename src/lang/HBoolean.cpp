#include "HClass.hh"

namespace H {
    namespace /*Boolean functions*/ {
    }

    Proto BooleanProto = {
        {L"constructor", [](LObjects& o){
            o[0]->data = false;
            return null;
        }},
        {L"destructor", emptyF},

        {L"toString", [](LObjects& o){
            return H::HStringFromString(rawBool(o[0])?L"true":L"false");
        }},
        {L"not", [](LObjects& o){
            return H::Booleans[!rawBool(o.at(0))];
        }},

        {L"+", [](LObjects& o){
            LObject result = Object::instantiate(Boolean);
            result->data = false;
            for(LObject& b : o)
                result->data = rawBool(result) || rawBool(b);
            return result;
        }},
        {L"*", [](LObjects& o){
            LObject result = Object::instantiate(Boolean);
            result->data = true;
            for(LObject& b : o)
                result->data = rawBool(result) && rawBool(b);
            return result;
        }},
    };

    Proto BooleanClassProto = {
        {L"$new", [](LObjects& o){
            o[0]->prototype = BooleanProto;
            return null;
        }},
    };
    LObject Boolean = LObject(new Object(BooleanClassProto));
}