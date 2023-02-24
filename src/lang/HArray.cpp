#include "HClass.hh"

namespace H {
    namespace /*Array functions*/ {
        size_t level = 0;
        std::wstring ArrayToString(LObjects& arr){
            level++;
            std::wstringstream ss;
            ss << L"[";
            if(level > 5) ss << L"...";
            else {
                auto& end = std::min(arr.end(), arr.begin()+10);
                std::for_each(arr.begin(), end, [&ss](LObject& i){
                    LObjects args = {i};
                    if(ss.tellp() > 1) ss << L",";
                    ss << *Runner::safeArgsCall(Global::Strings::toString, args)->data.string;
                });
                if(auto diff = arr.end()-end)
                    ss << L", .." << diff << " more items";
            }
            ss << L"]";
            level--;
            return ss.str();
        }
    }

    DEFINE_H_CLASS(Array)
        ArrayObjectProto = {
            {L"constructor", HNativeFunctionFromFunctionType([](LObjects& o){
                LObjects* newArray = new LObjects{o};
                newArray->erase(newArray->begin()); //remove first element
                o[0]->data.array = newArray;
                return null;
            })},
            {L"destructor", HNativeFunctionFromFunctionType([](LObjects& o){
                delete o[0]->data.array;
                return null;
            })},
            {Global::Strings::toString, HNativeFunctionFromFunctionType([](LObjects& o){
                if(o[0]->parent != Array) throw std::bad_cast();
                return H::HStringFromString(ArrayToString(*o[0]->data.array));
            })},
            {L"push", HNativeFunctionFromFunctionType([](LObjects& o){
                if(o[0]->parent != Array) throw std::bad_cast();
                LObjects*& This = o[0]->data.array;
                This->insert(This->end(), o.begin()+1, o.end());
                return o[0];
            })},

            {L"[]", HNativeFunctionFromFunctionType([](LObjects& o){
                if(o.size() < 2) throw std::out_of_range("");
                if(o[1]->parent != Number) throw std::bad_cast();
                try {
                    return o[0]->data.array->at((*o[1]->data.number)[0]);
                } catch (std::out_of_range&){
                    return null;
                }
            })}
        };

        ArrayProto = {
            {L"$new", LObject(new Object(ArrayObjectProto))},
        };
        Array = LObject(new Object(ArrayProto));
    DEFINE_H_CLASS_END(Array)
}