#include "HClass.hh"
#include "../script/parser.hh"

namespace H {
    namespace /*Boolean functions*/ {
    }

    LClass Boolean = new Class(
        L"Boolean",
        {
            {L"constructor", [](LObjects& o){
                o[0]->data = false;
                return null;
            }},
		    {L"destructor", emptyF},
            {L"toString", [](LObjects& o){
                std::wstringstream ss;
                ss << std::boolalpha;
                for(LObject& b : o){
                    ss<<rawBool(b);
                }
                return H::HStringFromString(ss.str());
            }},
            {L"not", [](LObjects& o){
                return H::Booleans[!rawBool(o.at(0))];
            }},
            {L"+", [](LObjects& o){
                LObject result = Boolean->instantiate();
                result->data = false;
                for(LObject& b : o)
                    result->data = rawBool(result) || rawBool(b);
                return result;
            }},
            {L"*", [](LObjects& o){
                LObject result = Boolean->instantiate();
                result->data = true;
                for(LObject& b : o)
                    result->data = rawBool(result) && rawBool(b);
                return result;
            }},
        }
    );
}