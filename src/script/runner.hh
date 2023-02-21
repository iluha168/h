#ifndef RUNNER_HH
#define RUNNER_HH

#include "../lang/HClassDecls.hh"
#include "../global/Global.hh"
#include "parser.hh"

namespace Runner {
    std::wstring getVariableName(H::LObject&, H::VarScope&) noexcept;
    H::LObject safeArgsCall(const std::wstring&, H::LObjects&, H::VarScope& = Global::Scope);

    H::LObject execTree(const Parser::SyntaxTree&, H::VarScope&);
    H::LObject run(Parser::SyntaxTree&, H::VarScope&);

    namespace Exceptions {
        class Break{};
    }
}

#endif