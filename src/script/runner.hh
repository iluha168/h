#ifndef RUNNER_HH
#define RUNNER_HH

#include "../lang/HClass.hh"
#include "parser.hh"

namespace Runner {
    typedef std::map<std::wstring, H::LObject> Entries;

    extern std::vector<H::LClass> classes;

    H::LClass getClassByName(std::wstring);
    std::wstring getVariableName(H::LObject&);
    
    H::LObject methodCall(std::wstring     , H::LObjects&, H::LClass);
    H::LObject methodCall(H::LObject, H::LObjects&, H::LClass);

    H::LObject execTree(const Parser::SyntaxTree&, Entries&);
    H::LObject run(Parser::SyntaxTree&, Entries&);

    namespace Exceptions {
        class Break{};
    }
}

#endif