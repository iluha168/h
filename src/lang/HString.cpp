#include "HClass.hh"
#include "../script/parser.h"

namespace H {
    namespace /*String functions*/ {

    }

    LClass String = new Class(
        L"String",
        {
            {L"constructor", [](Class::LObjects& o){
                o[0]->data = std::wstring();
                return null;
            }},
		    {L"destructor", emptyF},
            {L"toString", [](Class::LObjects& o){
                return o[0];
            }},
            {L"log", [](Class::LObjects& strs){
                for(Class::LObject& str : strs)
                    std::wcout << rawString(str);
                std::wcout << std::flush;
                return null;
            }},
            {L"+", [](Class::LObjects& strs){
                std::wstring result;
                for(Class::LObject& str : strs){
                    result += rawString(str);
                }
                return Parser::HStringFromWString(result);
            }},
        }
    );
}