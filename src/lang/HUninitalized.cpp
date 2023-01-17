#include "HClass.hh"
#include "../script/parser.hh"

namespace H {
    LClass Uninitialized = new Class(
        L"Uninitialized",
        {
            {L"constructor", emptyF},
		    {L"destructor", emptyF},
            {L"toString", [](Class::LObjects&){
                return Parser::HStringFromWString(Uninitialized->name);
            }}
        }
    );
}