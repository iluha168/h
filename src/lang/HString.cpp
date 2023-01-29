#include "HClass.hh"

namespace H {
    namespace /*String functions*/ {

    }

    Proto StringProto = {
        {L"constructor", [](LObjects& o){
            o[0]->data = std::wstring();
            return null;
        }},
        {L"destructor", emptyF},

        {L"toString", [](LObjects& o){
            return H::HStringFromString(L'"'+rawString(o[0])+L'"');
        }},
        {L"log", [](LObjects& strs){
            for(LObject& str : strs)
                std::wcout << rawString(str);
            std::wcout << std::flush;
            return null;
        }},

        {L"+", [](LObjects& strs){
            std::wstring result;
            for(LObject& str : strs){
                result += rawString(str);
            }
            return H::HStringFromString(result);
        }},
    };

    Proto StringClassProto = {
        {L"$new", [](LObjects& o){
            o[0]->prototype = StringProto;
            return null;
        }},
    };
    LObject String = LObject(new Object(StringClassProto));
}