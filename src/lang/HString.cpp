#include "HClass.hh"

namespace H {
    namespace /*String functions*/ {

    }

    LClass String = new Class(
        L"String",
        {
            {L"constructor", [](LObjects& o){
                o[0]->data = std::wstring();
                return null;
            }},
		    {L"destructor", emptyF},
            {L"toString", emptyF},
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
        }
    );
}