#ifndef PARSER_HH
#define PARSER_HH

#include "lexer.hh"
#include "../lang/HClassDecls.hh"
#include <variant>

namespace Parser {
    extern const std::wstring treeTypes[];

    struct SyntaxTree;
    typedef std::vector<SyntaxTree> SyntaxTrees;
    typedef std::shared_ptr<SyntaxTree> LSyntaxTree;
    struct SyntaxTree {
        enum Type {
            ScopeRoot,
            OperationNew,
            OperationBinary,
            OperationAssign,
            ControlFlowIf,
            ControlFlowWhile,
            ControlFlowFor,
            ControlFlowBreak,
            Variable,
            Constant,
            CallMethod,
        } type;
        std::variant<H::LObject, SyntaxTrees> value;
    };

    //helpers
    SyntaxTree treeForString(std::wstring);
    SyntaxTree parseNumber(size_t&, std::vector<Lexer::Token>&);
    std::vector<SyntaxTree> parseCallArguments(size_t&, std::vector<Lexer::Token>&, bool);
    SyntaxTree parseExpression(size_t&, std::vector<Lexer::Token>&);

    SyntaxTree syntaxTreeFor(std::vector<Lexer::Token>&);
    void logTree(const Parser::SyntaxTree&, unsigned short = 0);
}

#endif
