#include "HClass.hh"
#include <math.h>

namespace H {
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

    Proto NumberProto = {
        {L"constructor", [](LObjects& o){
            o[0]->data = Quaternion{};
            return null;
        }},
		{L"destructor", emptyF},
        {L"toString", [](LObjects& o){
            std::wstringstream ss;
            auto& v = rawNumber(o[0]);
            for(uint8_t i = 0; i < 4; i++){
                ss << (ss.tellp() ? std::noshowpos : std::showpos);
                if(v[i]) ss<<v[i]<<Global::quaternionBasisNames[i];
            }
            if(!ss.tellp()) ss << L"0";
            return H::HStringFromString(ss.str());
        }},
        {L"+", [](LObjects& nn){
            return HNumberFromQuaternion(sum(rawNumber(nn.at(0)), rawNumber(nn.at(1))));
        }},
        {L"-", [](LObjects& nn){
            return HNumberFromQuaternion(sub(rawNumber(nn.at(0)), rawNumber(nn.at(1))));
        }},
        {L"*", [](LObjects& nn){
            return HNumberFromQuaternion(mul(rawNumber(nn.at(0)), rawNumber(nn.at(1))));
        }},
        {L"absSq", [](LObjects& q){
            return HNumberFromQuaternion(absSq(rawNumber(q[0])));
        }},
        {L"abs", [](LObjects& q){
            return HNumberFromQuaternion(abs  (rawNumber(q[0])));
        }},
        {L"conjugate", [](LObjects& q){
            return HNumberFromQuaternion(conjugate(rawNumber(q[0])));
        }},
        {L"normalize", [](LObjects& q){
            return HNumberFromQuaternion(unit(rawNumber(q[0])));
        }},
        {L"reciprocal", [](LObjects& q){
            return HNumberFromQuaternion(reciprocal(rawNumber(q[0])));
        }},
        {L"exp", [](LObjects& q){
            return HNumberFromQuaternion(exp(rawNumber(q[0])));
        }},
        {L"sq", [](LObjects& q){
            return HNumberFromQuaternion(square(rawNumber(q[0])));
        }},

        {L"[]", [](LObjects& i){
            return HNumberFromQuaternion({ rawNumber(i[0]).at(rawNumber(i[1])[0]) ,0,0,0});
        }},
        //comparison functions ignore any complex parts, because complexes "is not an ordered field"
        //https://en.wikipedia.org/wiki/Complex_number#Field_structure
        {L"<", [](LObjects& op){
            return Booleans[rawNumber(op.at(0))[0] < rawNumber(op.at(1))[0]];
        }},
        {L">", [](LObjects& op){
            return Booleans[rawNumber(op.at(0))[0] > rawNumber(op.at(1))[0]];
        }}
    };

    Proto NumberClassProto = {
        {L"$new", [](LObjects& o){
            o[0]->prototype = NumberProto;
            return null;
        }},
        {L"random", [](LObjects&){
            LObject n = Object::instantiate(Number);
            for(uint8_t i = 0; i < 4; i++)
                rawNumber(n)[i] = Quaternion::value_type(rand())/Quaternion::value_type(RAND_MAX);
            return n;
        }},
    };
    LObject Number = LObject(new Object(NumberClassProto));
}