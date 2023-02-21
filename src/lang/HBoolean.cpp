#include "HClass.hh"

namespace H {
    DEFINE_H_CLASS(Boolean)
        BooleanObjectProto = {
            {L"constructor", HFunctionFromNativeFunction([](LObjects& o){
                o[0]->data.boolean = false;
                return null;
            })},

            {Global::Strings::toString, HFunctionFromNativeFunction([](LObjects& o){
                // std::format does not work on my machine ¯\_(ツ)_/¯
                std::wstringstream s;
                s<<std::boolalpha<<o[0]->data.boolean;
                return H::HStringFromString(s.str());
            })},
            {L"not", HFunctionFromNativeFunction([](LObjects& o){
                return H::Booleans[!o.at(0)->data.boolean];
            })},

            {L"+", HFunctionFromNativeFunction([](LObjects& o){
                for(LObject& b : o)
                    if(b->data.boolean)
                        return Booleans[1];
                return Booleans[0];
            })},
            {L"*", HFunctionFromNativeFunction([](LObjects& o){
                for(LObject& b : o)
                    if(!b->data.boolean)
                        return Booleans[0];
                return Booleans[1];
            })},
        };

        BooleanProto = {
            {L"$new", LObject(new Object(BooleanObjectProto))},
        };
        Boolean = LObject(new Object(BooleanProto));
    DEFINE_H_CLASS_END(Boolean)
}