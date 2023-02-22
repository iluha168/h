#include "HClass.hh"
#include "../script/runner.hh"

H::Object::Object(ObjectData& a, LObject b):
                      entries(a), parent(b){}

void H::Object::addref(LObject o)
{
    refs.push_back(o);
}
void H::Object::unref(LObject o)
{
    refs.erase(std::find(refs.begin(), refs.end(), o));
}

H::LObject H::Object::instantiate(LObject& instantiator, LObjects args)
{
    LObject o(new Object(instantiator->entries.at(L"$new")->entries, instantiator),
        [](Object* o_delete){
            LObjects lo = {LObject(o_delete)}; //! passing object without custom deallocator
            try {
                o_delete->entries.at(L"destructor")->data.function(lo);
            } catch(std::out_of_range&) {}
        }
    );
    args.insert(args.begin(), o);
    NativeFunction constructor;
    try {
        constructor = o->entries.at(L"constructor")->data.function;
    } catch(std::out_of_range&){
        return o;
    }
    constructor(args);
    return o;
}

H::LObject H::Object::call(std::wstring methodName, LObjects& args){
    try {
        H::LObject& method = H::emptyF;
        try {
            method = this->entries.at(methodName);
        } catch(std::out_of_range&) {
            throw std::wstring(L" is not defined");
        }
        #ifdef DEBUG
            std::wclog <<"Calling "<<methodName<<" on "<<args[0]<<" with Proto "<<(&args[0]->entries)<<" and parent "<<(args[0]->parent)<<std::endl;
        #endif
        if(method->parent != Function) throw std::wstring(L" is not a method");
        return method->data.function(args);
    } catch(std::out_of_range&){
        throw std::wstring(L": not enough arguments");
    } catch(std::bad_cast&){
        throw std::wstring(L": invalid arguments' types passed");
    }
}

namespace H {
    LObjects Object::refs{};

    //does nothing
    LObject emptyF;

    LObject null(nullptr);
    LObject Booleans[2];
}