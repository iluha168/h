#include "HClass.hh"
#include "../script/runner.hh"

H::Object::Object(Proto& prototype, LObject parent):
    prototype(prototype),
    parent(parent)
{
}

void H::Object::addref(LObject o)
{
    refs.push_back(o);
}
void H::Object::unref(LObject o)
{
    refs.erase(std::find(refs.begin(), refs.end(), o));
}

H::Proto noProto{};
H::LObject H::Object::instantiate(LObject& instantiator, LObjects args)
{
    LObject o(new Object(noProto, instantiator),
        [](Object* o_delete){
            LObjects lo = {LObject(o_delete)}; //! passing object without custom deallocator
            try {
                o_delete->prototype.at(L"destructor")(lo);
            } catch(std::out_of_range&) {}
        }
    );
    LObjects newObject = {o};
    instantiator->call(L"$new", newObject);
    args.insert(args.begin(), o);
    o->call(L"constructor", args);
    #ifdef DEBUG
        std::wclog <<"Object "<<o<<" has Proto "<<(&o->prototype)<<" and parent "<<(o->parent)<<std::endl;
    #endif
    return o;
}

H::LObject H::Object::call(std::wstring methodName, LObjects& args){
    try {
        H::NativeFunction& method = H::emptyF;
        try {
            method = this->prototype.at(methodName);
        } catch(std::out_of_range&) {
            throw std::wstring(L" is not defined");
        }
        #ifdef DEBUG
            std::wclog <<"Calling "<<methodName<<" on "<<args[0]<<" with Proto "<<(&args[0]->prototype)<<" and parent "<<(args[0]->parent)<<std::endl;
        #endif
        return method(args);
    } catch(std::out_of_range&){
        throw std::wstring(L": not enough arguments");
    } catch(std::bad_variant_access&){
        throw std::wstring(L": invalid arguments' types passed");
    }
}

namespace H {
    LObjects Object::refs{};

    //does nothing
    NativeFunction emptyF = [](LObjects& o){
        return o[0];
    };
    
    LObject HStringFromString(std::wstring native){
        LObject str = Object::instantiate(String);
        str->data = native;
        return str;
    }
    
    LObject HNumberFromQuaternion(Quaternion q){
        LObject result = Object::instantiate(Number);
        result->data = q;
        return result;
    }

    LObject null(nullptr);
    LObject Booleans[2]{};
}