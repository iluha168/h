#ifndef RUNNER_HH
#define RUNNER_HH

#include "../lang/HClassDecls.hh"
#include "../global/Global.hh"
#include "parser.hh"

namespace Runner {
    std::wstring getVariableName(H::LObject&, H::Entries&) noexcept;
    H::LObject safeArgsCall(const std::wstring&, H::LObjects&, H::Entries& = Global::Scope);

    H::LObject execTree(const Parser::SyntaxTree&, H::Entries&);
    H::LObject run(Parser::SyntaxTree&, H::Entries&);

    namespace Exceptions {
        class Break{};
    }
}

#endif