#include "Global.hh"

namespace Global {
    H::Entries Scope = {};
    Display* dis = XOpenDisplay(nullptr);
    Atom wmDeleteWindow = XInternAtom(dis, "WM_DELETE_WINDOW", false);
    Screen* scr = ScreenOfDisplay(dis, DefaultScreen(dis));
    const wchar_t quaternionBasisNames[4] = {L'\0',L'i',L'j',L'k'};
}