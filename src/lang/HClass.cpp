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

H::ObjectData noProto{};
H::LObject H::Object::instantiate(LObject& instantiator, LObjects args)
{
    try {
        LObject o(new Object(instantiator->entries.at(L"$new")->entries, instantiator),
            [](Object* o_delete){
                LObjects lo = {LObject(o_delete)}; //! passing object without custom deallocator
                try {
                    rawNativeF(o_delete->entries.at(L"destructor")->entries.at(L"()"))(lo);
                } catch(std::out_of_range&) {}
            }
        );
        args.insert(args.begin(), o);
        try {
            LObjects lo{o};
            rawNativeF(o->entries.at(L"constructor")->entries.at(L"()"))(lo);
        } catch(std::out_of_range&){}
        return o;
    } catch(std::out_of_range&){
        throw std::wstring(L"Object is not instantiable");
    }
}

H::LObject H::Object::call(std::wstring methodName, LObjects& args){
    try {
        H::LObject& method = H::emptyF;
        try {
            method = this->entries.at(methodName)->entries.at(L"()");
        } catch(std::out_of_range&) {
            throw std::wstring(L" is not defined");
        }
        #ifdef DEBUG
            std::wclog <<"Calling "<<methodName<<" on "<<args[0]<<" with Proto "<<(&args[0]->entries)<<" and parent "<<(args[0]->parent)<<std::endl;
        #endif
        try {
            return rawNativeF(method)(args);
        } catch(std::bad_variant_access&){
            throw std::wstring(L" is not a method");
        }
    } catch(std::out_of_range&){
        throw std::wstring(L": not enough arguments");
    } catch(std::bad_variant_access&){
        throw std::wstring(L": invalid arguments' types passed");
    }
}

namespace H {
    LObjects Object::refs{};
    decltype(ClassInits) ClassInits{};

    //does nothing
    LObject emptyF;

    LObject null(nullptr);
    LObject Booleans[2];
}