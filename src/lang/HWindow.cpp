#include "HClass.hh"

#include <X11/Xutil.h>

namespace H {
    namespace /*Window functions*/ {
        struct WGeometry
        {
            ::Window root;
            int x, y;
            unsigned width, height, border, depth;
        };
        std::string WStringToString(std::wstring wstr){
            char* c = new char[wstr.length()*sizeof(wchar_t)+1];
            c[std::wcstombs(c, wstr.c_str(), wstr.length())] = 0;
            std::string str(c);
            delete[] c;
            return str;
        }
        WGeometry getGeometry(LObject& o){
            WGeometry tmp;
	        XGetGeometry(Global::dis, o->data.window->first, &tmp.root, &tmp.x, &tmp.y, &tmp.width, &tmp.height, &tmp.border, &tmp.depth);
	        return tmp;
        }

        constexpr unsigned long RGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a){
            union {unsigned long l; int8_t i[4];} result;
            result.i[0] = b;
            result.i[1] = g;
            result.i[2] = r;
            result.i[3] = a;
            return result.l;
        }
        Quaternion unRGBA(unsigned long color){
            Quaternion n;
            for(int8_t i = 3; i >= 0; i--){
                n[i] = color & 0b11111111;
                color >>= 8;
            }
            return n;
        }

        XGCValues WinGetGCValues(::GC& gc, unsigned long mask){
            XGCValues v;
            XGetGCValues(Global::dis, gc, mask, &v);
            return v;
        }
    }

    DEFINE_H_CLASS(Window)
        WindowObjectProto = {
            {L"constructor", HFunctionFromNativeFunction([](LObjects& o){
                if(o.at(1)->parent != Number) throw std::bad_cast();
                if(o.at(2)->parent != String) throw std::bad_cast();
                WindowInfo* w = new WindowInfo;
                Quaternion& geom = *o[1]->data.number;
                w->first = XCreateSimpleWindow(
                    Global::dis, DefaultRootWindow(Global::dis),
                    geom[0],geom[1], geom[2],geom[3], 1,
                    0, RGBA(255,255,255,255)
                );
               {XSizeHints size;
                size.flags = PPosition|PSize;
                size.x      = geom[0]; size.y     = geom[1];
                size.height = geom[2]; size.width = geom[3];
	            XSetWMProtocols(Global::dis, w->first, &Global::wmDeleteWindow, 1);
                XSetStandardProperties(Global::dis, w->first, WStringToString(*o[2]->data.string).c_str(),"HIconName", None, NULL,0,&size);}
	            XSelectInput(Global::dis, w->first, 0b111111111111111111111111L);
	            XMapRaised(Global::dis, w->first);
                w->second = XCreateGC(Global::dis, w->first, 0,0);
                XFlush(Global::dis);
                o.at(0)->data.window = w;
                Object::addref(o[0]);
	            return null;
	        })},
	        {L"destructor", HFunctionFromNativeFunction([](LObjects& o){
                WindowInfo*& w = o[0]->data.window;
	            XFreeGC(Global::dis, w->second);
	            XDestroyWindow(Global::dis, w->first);
                delete w;
                return null;
            })},

            {Global::Strings::toString, HFunctionFromNativeFunction([](LObjects& o){
                if(o[0]->parent != Window) throw std::bad_cast();
                return H::HStringFromString(L"Window"+std::to_wstring(o[0]->data.window->first));
            })},
            {L"geometry", HFunctionFromNativeFunction([](LObjects& o){
                try {
                    if(o.at(1)->parent != Number) throw std::bad_cast();
                    Quaternion& geom = *o[1]->data.number;
                    XMoveResizeWindow(Global::dis, o[0]->data.window->first, geom[0], geom[1], geom[2], geom[3]);
                    return null;
                } catch (std::out_of_range&){
                    WGeometry geom = getGeometry(o[0]);
                    return HNumberFromQuaternion({
                        (Quaternion::value_type)geom.x,
                        (Quaternion::value_type)geom.y,
                        (Quaternion::value_type)geom.width,
                        (Quaternion::value_type)geom.height
                    });
                }
            })},
            {L"drawPoint", HFunctionFromNativeFunction([](LObjects& o){
                if(o.at(1)->parent != Number) throw std::bad_cast();
                WindowInfo& win = *o[0]->data.window;
                Quaternion& n = *o[1]->data.number;
                XDrawPoint(Global::dis, win.first, win.second, n[0], n[1]);
                return o[0];
            })},
            {L"drawLine", HFunctionFromNativeFunction([](LObjects& o){
                if(o.at(1)->parent != Number) throw std::bad_cast();
                WindowInfo& win = *o[0]->data.window;
                Quaternion& n = *o[1]->data.number;
                XDrawLine(Global::dis, win.first, win.second, n[0], n[1], n[2], n[3]);
                return o[0];
            })},
            {L"foreground", HFunctionFromNativeFunction([](LObjects& o){
                if(o.at(1)->parent != Number) throw std::bad_cast();
                GC& gc = o[0]->data.window->second;
                try {
                    Quaternion& c = *o[1]->data.number;
                    XSetForeground(Global::dis, gc, RGBA(c[0],c[1],c[2],c[3]));
                    return o[0];
                } catch(std::out_of_range&){
                    return HNumberFromQuaternion(unRGBA(WinGetGCValues(gc, GCForeground).foreground));
                }
            })},
            {L"clear", HFunctionFromNativeFunction([](LObjects& o){
                XClearWindow(Global::dis, o[0]->data.window->first);
                return o[0];
            })}
        };

        WindowProto = {
            {L"$new", LObject(new Object(WindowObjectProto))},
        };
        Window = LObject(new Object(WindowProto));
    DEFINE_H_CLASS_END(Window)
}