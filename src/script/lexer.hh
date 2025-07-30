#ifndef LEXER_HH
#define LEXER_HH

#include <string>
#include <iostream>
#include <vector>

namespace Lexer {
    extern const std::vector<std::wstring> reserved_words, all_puncts;
    extern const std::wstring tokenTypes[];

    size_t indexOf(const std::wstring, const std::vector<std::wstring>&);
    bool includes(const std::wstring, const std::vector<std::wstring>&);

    struct Token {
        enum Type {
            Number,
            Punct,
            String,
            Identifier,
            Reserved,
            Sign,
        } type;
        std::wstring value;
    };
    typedef std::vector<Token> Tokens;

    Tokens tokenize(std::wstring);
}

#endif
