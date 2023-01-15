#include "HClass.hh"
#include "../script/parser.h"

namespace H {
    namespace /*Boolean functions*/ {
    }

    LClass Boolean = new Class(
        L"Boolean",
        {
            {L"constructor", [](Class::LObjects& o){
                o[0]->data = false;
                return null;
            }},
		    {L"destructor", emptyF},
            {L"toString", [](Class::LObjects& o){
                std::wstringstream ss;
                ss << std::boolalpha;
                for(Class::LObject& b : o){
                    ss<<rawBool(b);
                }
                return Parser::HStringFromWString(ss.str());
            }},
            {L"not", [](Class::LObjects& o){
                return H::Booleans[!rawBool(o.at(0))];
            }},
            {L"+", [](Class::LObjects& o){
                Class::LObject result = Boolean->instantiate();
                result->data = false;
                for(Class::LObject& b : o)
                    result->data = rawBool(result) || rawBool(b);
                return result;
            }},
            {L"*", [](Class::LObjects& o){
                Class::LObject result = Boolean->instantiate();
                result->data = true;
                for(Class::LObject& b : o)
                    result->data = rawBool(result) && rawBool(b);
                return result;
            }},
        }
    );
}