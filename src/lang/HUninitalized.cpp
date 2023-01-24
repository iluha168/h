#include "HClass.hh"

namespace H {
    LClass Uninitialized = new Class(
        L"Uninitialized",
        {
            {L"constructor", emptyF},
		    {L"destructor", emptyF},
            {L"toString", [](LObjects&){
                return H::HStringFromString(Uninitialized->name);
            }}
        }
    );
}