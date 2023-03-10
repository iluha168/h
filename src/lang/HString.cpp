#include "HClass.hh"

namespace H {
    LObject HStringFromString(std::wstring native){
        LObject str = Object::instantiate(String);
        *str->data.string = native;
        return str;
    }

    DEFINE_H_CLASS(String)
        StringObjectProto = {
            {L"constructor", HNativeFunctionFromFunctionType([](LObjects& o){
                o[0]->data.string = new std::wstring();
                return null;
            })},
            {L"destructor", HNativeFunctionFromFunctionType([](LObjects& o){
                delete o[0]->data.string;
                return null;
            })},
    
            {Global::Strings::toString, HNativeFunctionFromFunctionType([](LObjects& o){
                return H::HStringFromString(L'"'+(*o[0]->data.string)+L'"');
            })},
            {L"log", HNativeFunctionFromFunctionType([](LObjects& strs){
                for(LObject& str : strs)
                    std::wcout << *str->data.string;
                std::wcout << std::flush;
                return null;
            })},
    
            {L"+", HNativeFunctionFromFunctionType([](LObjects& strs){
                std::wstring result;
                for(LObject& str : strs){
                    result += *str->data.string;
                }
                return H::HStringFromString(result);
            })},
        };
    
        StringProto = {
            {L"$new", LObject(new Object(StringObjectProto))},
        };
        String = LObject(new Object(StringProto));
    DEFINE_H_CLASS_END(String)
}