#ifndef GLOBAL_HH
#define GLBOAL_HH

#include <X11/Xlib.h>
#include "../lang/HClassDecls.hh"

namespace Global {
	extern H::VarScope Scope;
    extern Display* dis;
    extern Atom wmDeleteWindow;
    extern Screen* scr;
    extern Colormap colormap;
    extern const wchar_t quaternionBasisNames[4];
}

#endif