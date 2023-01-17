#ifndef PARSER_H
#define PARSER_H

#include "lexer.hh"
#include "../lang/HClass.hh"

namespace Parser {
    extern const std::wstring treeTypes[];

    struct SyntaxTree;
    typedef std::vector<SyntaxTree> SyntaxTrees;
    struct SyntaxTree {
        enum Type {
            ScopeRoot,
            OperationNew,
            OperationBinary,
            OperationAssign,
            ControlFlowIf,
            ControlFlowWhile,
            ControlFlowBreak,
            Variable,
            Constant,
            CallMethod,
        } type;
        std::variant<H::Class::LObject, SyntaxTrees> value;
    };

    //helpers
    H::Class::LObject HStringFromWString(std::wstring);
    SyntaxTree treeForString(std::wstring);
    SyntaxTree parseNumber(size_t&, std::vector<Lexer::Token>&);
    std::vector<SyntaxTree> parseCallArguments(size_t&, std::vector<Lexer::Token>&, bool);
    SyntaxTree parseExpression(size_t&, std::vector<Lexer::Token>&);

    SyntaxTree syntaxTreeFor(std::vector<Lexer::Token>&);
    void logTree(const Parser::SyntaxTree&, unsigned short = 0);
}

#endif