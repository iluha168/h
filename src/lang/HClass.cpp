#include "HClass.hh"
#include "../script/runner.hh"


H::Class::Class(decltype(name) name, decltype(prototype) prototype):
    name(name),
    prototype(prototype)
{
}

void H::Class::addref(LObject o)
{
    refs.push_back(o);
}
void H::Class::unref(LObject o)
{
    refs.erase(std::find(refs.begin(), refs.end(), o));
}

H::Class::LObject H::Class::instantiate(LObjects args)
{
    LObject o(new Object(this),
        [](Object* o_delete){
            LObjects lo = {LObject(o_delete)}; //! passing object without custom deallocator
            /*
            std::wclog <<L"Deinstantiating "<<(void*)o_delete<<L" ("<<o_delete->parent->name<<L") ";
            try {
                std::wclog <<rawString(Runner::methodCall(L"toString", lo, o_delete->parent));
            } catch(std::wstring& err){
                std::wclog <<L"FAILED "<<err;
            }
            std::wclog<<std::endl;
            */
            o_delete->parent->prototype.at(L"destructor")(lo);
            // delete o_delete; <- done automatically by LObject
        }
    );
    args.insert(args.begin(), {o});
    Runner::methodCall(L"constructor", args, this);
    return o;
}

H::Class::~Class(){
    std::remove(Runner::classes.begin(), Runner::classes.end(), this);
}

H::Class::Object::Object(decltype(parent) parent):
    parent(parent)
{
}

namespace H {
    Class::LObjects Class::refs{};

    //does nothing
    Class::Function emptyF = [](H::Class::LObjects& o){
        return o[0];
    };

    Class::LObject null(nullptr);
    Class::LObject Booleans[2]{};
}