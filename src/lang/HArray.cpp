#include "HClass.hh"

namespace H {
    namespace /*Array functions*/ {
        size_t level = 0;
        std::wstring toStr(L"toString");
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
                    ss << rawString(Runner::safeArgsCall(toStr, args));
                });
                if(auto diff = arr.end()-end)
                    ss << L", .." << diff << " more items";
            }
            ss << L"]";
            level--;
            return ss.str();
        }
    }

    IMPLEMENT_H_CLASS(Array)
        ArrayObjectProto = {
            {L"constructor", HFunctionFromNativeFunction([](LObjects& o){
                o[0]->data = o;
                rawArray(o[0]).erase(rawArray(o[0]).begin());
                return null;
            })},
            {L"toString", HFunctionFromNativeFunction([](LObjects& o){
                return H::HStringFromString(ArrayToString(rawArray(o[0])));
            })},
            {L"push", HFunctionFromNativeFunction([](LObjects& o){
                LObjects& This = rawArray(o[0]);
                This.insert(This.end(), o.begin()+1, o.end());
                return o[0];
            })},

            {L"[]", HFunctionFromNativeFunction([](LObjects& o){
                if(o.size() < 2) throw std::out_of_range("");
                try {
                    return rawArray(o[0]).at(rawNumber(o[1])[0]);
                } catch (std::out_of_range&){
                    return null;
                }
            })}
        };

        ArrayProto = {
            {L"$new", LObject(new Object(ArrayObjectProto))},
        };
        Array = LObject(new Object(ArrayProto));
    IMPLEMENT_H_CLASS_END(Array)
}