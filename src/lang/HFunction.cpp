#include "HClass.hh"

namespace H {
    LObject HFunctionFromNativeFunction(NativeFunction NativeF){
        LObject f = Object::instantiate(Function);
        f->data = NativeF;
        std::wclog << L"Set " << rawNativeF(f) << L" to " << NativeF << std::endl;
        return f;
    }

    IMPLEMENT_H_CLASS(Function)
        FunctionProto = {
            {L"$new", LObject(new Object(FunctionObjectProto))},
        };
        Function = LObject(new Object(FunctionProto));
        H::FunctionObjectProto.insert_or_assign(L"()",
            H::HFunctionFromNativeFunction([](H::LObjects& o){
                return rawNativeF(o[0])(o);
            })
	    );
    IMPLEMENT_H_CLASS_END(Function)
}