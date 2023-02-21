#include "HClass.hh"
#include <math.h>

namespace H {
    LObject HNumberFromQuaternion(Quaternion q){
        LObject result = Object::instantiate(Number);
        *result->data.number = q;
        return result;
    }
    
    namespace /*Number functions*/ {
        Quaternion sum(Quaternion& q1, Quaternion& q2){
            Quaternion result{};
            for(uint8_t i = 0; i < 4; i++)
                result[i] = q1[i]+q2[i];
            return result;
        }
        Quaternion sub(Quaternion& q1, Quaternion& q2){
            Quaternion result{};
            for(uint8_t i = 0; i < 4; i++)
                result[i] = q1[i]-q2[i];
            return result;
        }
        Quaternion mul(Quaternion& q1, Quaternion& q2){
            using std::fma;
            return {
                fma(q1[0], q2[0], -fma(q1[1], q2[1], fma(q1[2],q2[2],  q1[3]*q2[3]))),
                fma(q1[0], q2[1],  fma(q1[1], q2[0], fma(q1[2],q2[3], -q1[3]*q2[2]))),
                fma(q1[0], q2[2], fma(-q1[1], q2[3], fma(q1[2],q2[0],  q1[3]*q2[1]))),
                fma(q1[0], q2[3],  fma(q1[1], q2[2], fma(-q1[2],q2[1], q1[3]*q2[0])))
            };
        }
        Quaternion mul(Quaternion q, Quaternion::value_type scalar){
            for(uint8_t i = 0; i < 4; i++) q[i] *= scalar;
            return q;
        }
        Quaternion absSq(Quaternion& q){
            Quaternion result{};
            for(uint8_t i = 0; i < 4; i++)
                result[0] += q[i]*q[i];
            return result;
        }
        Quaternion abs(Quaternion& q){
            return {std::sqrt(absSq(q)[0]),0,0,0};
        }
        Quaternion conjugate(Quaternion& q){
            return {q[0],-q[1],-q[2],-q[3]};
        }
        Quaternion unit(Quaternion& q){
            return mul(q, 1/abs(q)[0]);
        }
        Quaternion reciprocal(Quaternion& q){
            return mul(conjugate(q), 1/absSq(q)[0]);
        }
        Quaternion vectorPart(Quaternion& q){
            return {0,q[1],q[2],q[3]};
        }
        Quaternion exp(Quaternion& q){
            Quaternion v = vectorPart(q);
            Quaternion::value_type vNorm = abs(v)[0];
            if(vNorm == 0) return {std::exp(q[0]),0,0,0};
            Quaternion temp = mul(v, std::sin(vNorm)/vNorm);
            temp[0] += std::cos(vNorm);
            return mul(temp, std::exp(q[0]));
        }
        Quaternion square(Quaternion& q){
            return {q[0]*q[0]-q[1]*q[1]-q[2]*q[2]-q[3]*q[3], 2*q[0]*q[1], 2*q[0]*q[2], 2*q[0]*q[3]};
        }
    }

    DEFINE_H_CLASS(Number)
        NumberObjectProto = {
            {L"constructor", HFunctionFromNativeFunction([](LObjects& o){
                o[0]->data.number = new Quaternion{};
                return null;
            })},
            {L"destructor", HFunctionFromNativeFunction([](LObjects& o){
                delete o[0]->data.number;
                return null;
            })},
            {Global::Strings::toString, HFunctionFromNativeFunction([](LObjects& o){
                if(o[0]->parent != Number) throw std::bad_cast();
                std::wstringstream ss;
                Quaternion& v = *o[0]->data.number;
                for(uint8_t i = 0; i < 4; i++){
                    ss << (ss.tellp() ? std::showpos : std::noshowpos);
                    if(v[i]) ss<<v[i]<<Global::quaternionBasisNames[i];
                }
                if(!ss.tellp()) ss << L"0";
                return H::HStringFromString(ss.str());
            })},
            {L"+", HFunctionFromNativeFunction([](LObjects& nn){
                return HNumberFromQuaternion(sum(*nn[0]->data.number, *nn[1]->data.number));
            })},
            {L"-", HFunctionFromNativeFunction([](LObjects& nn){
                return HNumberFromQuaternion(sub(*nn[0]->data.number, *nn[1]->data.number));
            })},
            {L"*", HFunctionFromNativeFunction([](LObjects& nn){
                return HNumberFromQuaternion(mul(*nn[0]->data.number, *nn[1]->data.number));
            })},
            {L"absSq", HFunctionFromNativeFunction([](LObjects& q){
                return HNumberFromQuaternion(absSq(*q[0]->data.number));
            })},
            {L"abs", HFunctionFromNativeFunction([](LObjects& q){
                return HNumberFromQuaternion(abs  (*q[0]->data.number));
            })},
            {L"conjugate", HFunctionFromNativeFunction([](LObjects& q){
                return HNumberFromQuaternion(conjugate(*q[0]->data.number));
            })},
            {L"normalize", HFunctionFromNativeFunction([](LObjects& q){
                return HNumberFromQuaternion(unit(*q[0]->data.number));
            })},
            {L"reciprocal", HFunctionFromNativeFunction([](LObjects& q){
                return HNumberFromQuaternion(reciprocal(*q[0]->data.number));
            })},
            {L"exp", HFunctionFromNativeFunction([](LObjects& q){
                return HNumberFromQuaternion(exp(*q[0]->data.number));
            })},
            {L"sq", HFunctionFromNativeFunction([](LObjects& q){
                return HNumberFromQuaternion(square(*q[0]->data.number));
            })},
    
            {L"[]", HFunctionFromNativeFunction([](LObjects& i){
                return HNumberFromQuaternion({ i[0]->data.number->at(i[1]->data.number->at(0)) ,0,0,0});
            })},
            //comparison functions ignore any complex parts, because complexes "is not an ordered field"
            //https://en.wikipedia.org/wiki/Complex_number#Field_structure
            {L"<", HFunctionFromNativeFunction([](LObjects& op){
                return Booleans[op[0]->data.number->at(0) < op[1]->data.number->at(0)];
            })},
            {L">", HFunctionFromNativeFunction([](LObjects& op){
                return Booleans[op[0]->data.number->at(0) > op[1]->data.number->at(0)];
            })}
        };
    
        NumberProto = {
            {L"$new", LObject(new Object(NumberObjectProto))},
            {L"random", HFunctionFromNativeFunction([](LObjects&){
                LObject n = Object::instantiate(Number);
                for(uint8_t i = 0; i < 4; i++)
                    n->data.number->at(i) = Quaternion::value_type(rand())/Quaternion::value_type(RAND_MAX);
                return n;
            })},
        };
        Number = LObject(new Object(NumberProto));
    DEFINE_H_CLASS_END(Number)
}