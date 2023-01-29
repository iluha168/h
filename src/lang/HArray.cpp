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

    Proto ArrayProto = {
        {L"constructor", [](LObjects& o){
            o[0]->data = o;
            rawArray(o[0]).erase(rawArray(o[0]).begin());
            return null;
        }},
		{L"destructor", emptyF},
        {L"toString", [](LObjects& o){
            return H::HStringFromString(ArrayToString(rawArray(o[0])));
        }},
        {L"push", [](LObjects& o){
            LObjects& This = rawArray(o[0]);
            This.insert(This.end(), o.begin()+1, o.end());
            return o[0];
        }},

        {L"[]", [](LObjects& o){
            if(o.size() < 2) throw std::out_of_range("");
            try {
                return rawArray(o[0]).at(rawNumber(o[1])[0]);
            } catch (std::out_of_range&){
                return null;
            }
        }}
    };
    
    Proto ArrayClassProto = {
        {L"$new", [](LObjects& o){
            o[0]->prototype = ArrayProto;
            return null;
        }},
    };
    LObject Array = LObject(new Object(ArrayClassProto));
}