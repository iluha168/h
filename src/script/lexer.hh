#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <iostream>
#include <vector>

namespace Lexer {
    extern const std::vector<std::wstring> reserved_words;

    extern const std::wstring tokenTypes[];
    struct Token {
        enum Type {
            Number,
            Punct,
            String,
            Identifier,
            Reserved,
            Sign,
        };

        Type type;
        std::wstring value;
    };
    typedef std::vector<Token> Tokens;

    Tokens tokenize(std::wstring);
}

#endif
