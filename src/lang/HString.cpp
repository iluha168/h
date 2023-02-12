#include "HClass.hh"

namespace H {
    LObject HStringFromString(std::wstring native){
        LObject str = Object::instantiate(String);
        str->data = native;
        return str;
    }

    IMPLEMENT_H_CLASS(String)
        StringObjectProto = {
            {L"constructor", HFunctionFromNativeFunction([](LObjects& o){
                o[0]->data = std::wstring();
                return null;
            })},
    
            {L"toString", HFunctionFromNativeFunction([](LObjects& o){
                return H::HStringFromString(L'"'+rawString(o[0])+L'"');
            })},
            {L"log", HFunctionFromNativeFunction([](LObjects& strs){
                for(LObject& str : strs)
                    std::wcout << rawString(str);
                std::wcout << std::flush;
                return null;
            })},
    
            {L"+", HFunctionFromNativeFunction([](LObjects& strs){
                std::wstring result;
                for(LObject& str : strs){
                    result += rawString(str);
                }
                return H::HStringFromString(result);
            })},
        };
    
        StringProto = {
            {L"$new", LObject(new Object(StringObjectProto))},
        };
        String = LObject(new Object(StringProto));
    IMPLEMENT_H_CLASS_END(String)
}