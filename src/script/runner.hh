#ifndef RUNNER_H
#define RUNNER_H

#include "../lang/HClass.hh"
#include "parser.hh"

namespace Runner {
    typedef std::map<std::wstring, H::Class::LObject> Entries;

    extern std::vector<H::LClass> classes;

    H::LClass getClassByName(std::wstring);
    std::wstring getVariableName(H::Class::LObject&);
    
    H::Class::LObject methodCall(std::wstring     , H::Class::LObjects&, H::LClass);
    H::Class::LObject methodCall(H::Class::LObject, H::Class::LObjects&, H::LClass);

    H::Class::LObject execTree(const Parser::SyntaxTree&, Entries&);
    void run(Parser::SyntaxTree&, Entries&);

    namespace Exceptions {
        class Break{};
    }
}

#endif