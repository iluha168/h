#include "HClass.hh"

namespace H {
    Proto UninitializedProto = {
        {L"constructor", emptyF},
		{L"destructor", emptyF},
        {L"toString", [](LObjects&){
            return H::HStringFromString(L"null");
        }}
    };

    Proto UninitializedClassProto = {
        {L"$new", [](LObjects& o){
            o[0]->prototype = UninitializedProto;
            return null;
        }},
    };
    LObject Uninitialized = LObject(new Object(UninitializedClassProto));
}