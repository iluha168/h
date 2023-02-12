#include "Global.hh"

namespace Global {
    H::VarScope Scope = {};
    Display* dis;
    Atom wmDeleteWindow;
    Screen* scr;
    const wchar_t quaternionBasisNames[4] = {L'\0',L'i',L'j',L'k'};
}