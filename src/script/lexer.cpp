#include "lexer.hh"
#include "runner.hh"
#include <cwctype>
#include <functional>

namespace Lexer {
    const std::vector<std::wstring> reserved_words({
        //don't change order
        L"new", L"if", L"else", L"while", L"break", L"for"
    });
    extern const std::wstring tokenTypes[] = {
        L"Number",
        L"Punct",
        L"String",
        L"Identifier",
        L"Reserved",
        L"Sign",
    };

    Token tryReadPunct(size_t& pos, std::wstring& code){
        if(std::iswpunct(code[pos])){
            Token punct(Token::Punct, L"");
            if(code[pos] == L'+' || code[pos] == L'-')
                punct.type = Token::Sign;
            punct.value = code[pos];
            pos++;
            return punct;
        }
        throw -1;
    }
    Token tryReadString(size_t& pos, std::wstring& code){
        if(code[pos] != L'"') throw -1;
        Token str(Token::String, L"");
        bool backSlashFlag = false;
        for(;;){
            pos++;
            if(backSlashFlag){
                switch (code[pos]){
                    case L'"':
                    case L'\'':
                    case L'\\':
                        str.value += code[pos];
                    break;
                    case L'n': str.value += L'\n'; break;
                    case L'r': str.value += L'\r'; break;
                    case L't': str.value += L'\t'; break;
                    default:
                    throw std::wstring(L"Lexer: doesn't know symbol '\\")+code[pos]+L"'.";
                }
                backSlashFlag = false;
            } else switch(code[pos]){
                case L'\\':
                    backSlashFlag = true;
                break;
                case L'"':
                    pos++;
                    return str;
                default:
                    str.value += code[pos];
                break;
            }
        }
    }
    Token tryReadNumber(size_t& pos, std::wstring& code){
        if(!std::iswdigit(code[pos]) && code[pos] != L'.') throw -1;
        Token num(Token::Number, L"");
        bool dot = false;
        do {
            if(code[pos] == L'.'){
                if(dot) return num;
                else dot = true;
            }
            num.value += code[pos];
            pos++;
        } while(std::iswdigit(code[pos]) || code[pos] == L'.');
        if(num.value == L".") num.type = Token::Punct;
        return num;
    }
    Token tryReadIdentifier(size_t& pos, std::wstring& code){
        if(!std::iswalpha(code[pos]) && code[pos] != L'_') throw -1;
        Token id(Token::Identifier, L"");
        id.value = code[pos];
        for(;;){
            pos++;
            if(std::iswalnum(code[pos]) || code[pos] == L'_')
                id.value += code[pos];
            else {
                if(std::find(reserved_words.begin(), reserved_words.end(), id.value) != reserved_words.end())
                    id.type = Token::Reserved;
                return id;
            }
        }
    }
    const std::initializer_list<Token(*)(size_t&, std::wstring&)> tryReads = {
        tryReadString,
        tryReadNumber,
        tryReadPunct,
        tryReadIdentifier,
    };

    Tokens tokenize(std::wstring code){
        Tokens tokens;
        size_t pos = 0;

        //remove comments
        for(size_t posEnd = 0; posEnd < code.length();){
            posEnd = code.find(L'\n',pos);
            if(code[pos] == L'#') code.erase(pos, posEnd-pos+1);
            else pos = posEnd+1;
        }

        for(pos = 0; ;){
            for(;std::iswspace(code[pos]); pos++);
            bool successRead = false;
            for(const auto& tryRead : tryReads){
                try {
                    tokens.push_back(tryRead(pos, code));
                    successRead = true;
                }
                catch (int) {}
                if(successRead) break;
            }
            if(pos >= code.length()) break;
            if(!successRead){
                std::wstring err = std::wstring(L"Lexer: at ");
                err += std::to_wstring(std::count(code.begin(), code.begin()+pos, L'\n')+1) + L":";
                const size_t lineBegin = std::find(code.crbegin()+pos, code.crbegin(), L'\n') - code.crbegin();
                const size_t lineEnd = std::find(code.begin()+pos, code.end(), L'\n') - code.begin() - 1;
                err += std::to_wstring(pos-lineBegin) + L"\n";
                err += code.substr(lineBegin, lineEnd) + L"\n";
                throw err;
            }
        }

        #ifdef DEBUG
		    for(size_t i = 0; i < tokens.size(); i++){
    			std::wclog << i << " " << tokenTypes[tokens[i].type] << L" | " << tokens[i].value << std::endl;
		    }
        #endif
        return tokens;
    }
}