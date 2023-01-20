#include "parser.hh"
#include "runner.hh"
#include "../global/Global.hh"

namespace Parser {
    const std::wstring treeTypes[] = {
        L"ScopeRoot",
        L"OperationNew",
        L"OperationBinary",
        L"OperationAssign",
        L"ControlFlowIf",
        L"ControlFlowWhile",
        L"ControlFlowFor",
        L"ControlFlowBreak",
        L"Variable",
        L"Constant",
        L"CallMethod",
    };
    
    H::Class::LObject HStringFromWString(std::wstring native){
        H::Class::LObject str = H::String->instantiate();
        str->data = native;
        return str;
    }
    SyntaxTrees treesConcat(SyntaxTrees t1, SyntaxTrees t2){
        t1.insert(t1.end(), t2.begin(), t2.end());
        return t1;
    }
    SyntaxTree treeForVariable(std::wstring name){
        return {
            SyntaxTree::Variable,
            HStringFromWString(name),
        };
    }
    SyntaxTree treeForConstant(H::Class::LObject o){
        return {
            SyntaxTree::Constant,
            o
        };
    }
    SyntaxTree treeForString(std::wstring str){
        return treeForConstant(HStringFromWString(str));
    }
    SyntaxTree treeForOperation(std::wstring op, SyntaxTrees operands){
        return {
            SyntaxTree::OperationBinary,
            treesConcat(
                {treeForString(op)},
                operands
            )
        };
    }

    SyntaxTree parseNumber(size_t& pos, Lexer::Tokens& tokens){
        H::Class::LObject number = H::Number->instantiate();
        const auto& helper = [&]{
            double tmp = 1, ijk = 0;
            if(tokens[pos  ].type == Lexer::Token::Sign &&
               tokens[pos+1].type == Lexer::Token::Number){
                if(tokens[pos++].value[0] == L'-') tmp = -1;
            }
            if(tokens.at(pos).type == Lexer::Token::Number){
                tmp *= std::stod(tokens[pos++].value);
                if(tokens[pos].type == Lexer::Token::Identifier && tokens[pos].value.length() == 1UL){
                    auto iter = std::find(std::begin(Global::quaternionBasisNames), std::end(Global::quaternionBasisNames), tokens[pos].value[0]);
                    if(iter != std::end(Global::quaternionBasisNames)){
                        ijk = iter-std::begin(Global::quaternionBasisNames);
                        pos++;
                    }
                }
                rawNumber(number)[ijk] += tmp;
            } else {
                throw std::wstring(L"expected a number");
            }
        };
        helper();
        try {
            for(;;) helper();
        } catch (...) {}
        return treeForConstant(number);
    }
    
    SyntaxTrees parseCallArguments(size_t& pos, Lexer::Tokens& tokens, bool optional = false){
        if(tokens[pos].type == Lexer::Token::Punct && tokens[pos].value[0] == L'('){
            SyntaxTrees args = {};
            for(;;){
                pos++;
                if(tokens[pos].type == Lexer::Token::Punct
                && tokens[pos].value[0] == L')'){
                        pos++;
                        return args;
                }
                args.push_back(parseExpression(pos, tokens));
                if(tokens[pos].type == Lexer::Token::Punct)
                    switch(tokens[pos].value[0]){
                        case L')':
                            pos--;
                        case L',':
                        continue;
                        default:
                        throw std::wstring(L"expected ',' or ')' in call arguments.");
                }
                else throw std::wstring(L"expected Punct.");
            }
        } else {
            if(optional) return {};
            throw std::wstring(L"expected call arguments.");
        }
    }

    SyntaxTree parseExpression(size_t& pos, Lexer::Tokens& tokens){
        SyntaxTree tempTree;
        switch(tokens[pos].type){
            case Lexer::Token::Reserved: {
                size_t reservedWordIndex = std::find(Lexer::reserved_words.begin(), Lexer::reserved_words.end(), tokens[pos].value)-Lexer::reserved_words.begin();
                switch(reservedWordIndex){
                    case 0: {//new
                        SyntaxTrees className = {treeForString(tokens[++pos].value)};
                        tempTree = {
                            SyntaxTree::OperationNew,
                            treesConcat(
                                className, parseCallArguments(++pos, tokens, true)
                            )
                        };
                    } break;
                    case 1: {//if
                        SyntaxTree condition = parseExpression(++pos, tokens);
                        tempTree = {
                            SyntaxTree::ControlFlowIf,
                            SyntaxTrees({
                                condition, parseExpression(pos, tokens)
                            })
                        };
                    } break;
                    case 2: //else
                    throw std::wstring(L"else without previous if");
                    case 3: //true
                    case 4: //false
                        tempTree = treeForConstant(H::Booleans[reservedWordIndex == 3]);
                        pos++;
                    break;
                    case 5: {//while
                        SyntaxTree condition = parseExpression(++pos, tokens);
                        tempTree = {
                            SyntaxTree::ControlFlowWhile,
                            SyntaxTrees({
                                condition, parseExpression(pos, tokens)
                            })
                        };
                    } break;
                    case 6: //break
                        tempTree = {
                            SyntaxTree::ControlFlowBreak,
                            SyntaxTrees({})
                        };
                        pos++;
                    break;
                    case 7: //for
                        pos++;
                        tempTree = {
                            SyntaxTree::ControlFlowFor,
                            SyntaxTrees()
                        };
                        for(uint8_t i = 0; i < 4; i++)
                            std::get<SyntaxTrees>(tempTree.value).push_back(
                                parseExpression(pos, tokens)
                            );
                    break;
                    default:
                    goto expected_expression;
                }
                // Add optional else branch
                switch(reservedWordIndex){
                    case 1: //if
                    case 5: //while
                    case 7: //for
                    if(tokens[pos].type == Lexer::Token::Reserved && tokens[pos].value == L"else")
                        std::get<SyntaxTrees>(tempTree.value).push_back(
                            parseExpression(++pos, tokens)
                        );
                }
            } break;
            case Lexer::Token::Sign:
            case Lexer::Token::Number:
            tempTree = parseNumber(pos, tokens);
            break;
            case Lexer::Token::String:
            tempTree = treeForString(tokens[pos++].value);
            break;
            case Lexer::Token::Identifier:
            tempTree = treeForVariable(tokens[pos++].value);
            break;
            case Lexer::Token::Punct:
                switch (tokens[pos].value[0]){
                case L'{':
                    tempTree = {SyntaxTree::ScopeRoot, SyntaxTrees()};
                    for(pos++; tokens[pos].type != Lexer::Token::Punct || tokens[pos].value[0] != L'}';){
                        std::get<SyntaxTrees>(tempTree.value).push_back(parseExpression(pos, tokens));
                    };
                    pos++;
                break;
                case L'(':
                    tempTree = parseExpression(++pos, tokens);
                    if(tokens[pos].type != Lexer::Token::Punct || tokens[pos].value[0] != L')')
                        throw std::wstring(L"unmatched parentheses ')'");
                    pos++;
                break;
                default:
                goto expected_expression;
                }
            break;
            default:
            expected_expression:
            throw std::wstring(L"expected an expression");
        }

        while(pos < tokens.size())
        if(tokens[pos].type == Lexer::Token::Punct){
            switch(tokens[pos].value[0]){
                case L'.': {
                        SyntaxTrees classMethod = {
                                tempTree, //object
                                treeForString(tokens[++pos].value), //method name
                        };
                        tempTree = {
                            SyntaxTree::CallMethod,
                            treesConcat(classMethod, parseCallArguments(++pos, tokens))
                        };
                } break;
                case L'=': {
                    SyntaxTree varName = treeForString(tokens[pos-1].value);
                    tempTree = {
                        SyntaxTree::OperationAssign, 
                        SyntaxTrees({
                            varName,
                            parseExpression(++pos, tokens)
                        })
                    };
                } break;
                case L'[':
                    tempTree = {
                        SyntaxTree::CallMethod,
                        SyntaxTrees({
                            tempTree,
                            treeForString(L"[]"),
                            parseExpression(++pos, tokens)
                        })
                    };
                    if(tokens[pos].type == Lexer::Token::Punct && tokens[pos].value == L"]") pos++;
                    else throw std::wstring(L"unmatched parentheses ']'");
                break;
                case L'<':
                case L'>':
                case L'*':
                    tempTree = {
                        SyntaxTree::OperationBinary,
                        SyntaxTrees({
                            treeForString(tokens[pos].value),
                            tempTree
                        })
                    };
                    std::get<SyntaxTrees>(tempTree.value).push_back(parseExpression(++pos, tokens));
                break;
                default: //unknown binary operation
                return tempTree;
            }
        } else if(tokens[pos].type == Lexer::Token::Sign){
            try {
                tempTree = treeForOperation(L"+", {
                    tempTree,
                    parseNumber(++pos, tokens)
                });
            } catch (std::wstring&){
                std::wstring& sign = tokens[pos-1].value;
                tempTree = treeForOperation(
                    sign, {
                        tempTree,
                        parseExpression(pos, tokens)
                    }
                );
            }
        } else break;
        return tempTree;
    }

    SyntaxTree syntaxTreeFor(Lexer::Tokens& tokens){
        size_t pos = 0;
        SyntaxTree root(SyntaxTree::ScopeRoot, SyntaxTrees());
        try {
            while(pos < tokens.size()){
                std::get<SyntaxTrees>(root.value).push_back(parseExpression(pos, tokens));
            }
        } catch (std::wstring& desc){
            auto traceWhere = tokens.begin()+pos;
            auto traceBegin = std::max(tokens.begin(), traceWhere-4);
            auto traceEnd   = std::min(tokens.end  (), traceWhere+4);
            size_t whereLen = 0;
            std::wstring err = L"Parser error: ";
            err += desc + L'\n';
            for(auto i = traceBegin; i < traceEnd; i++){
                err += i->value + L' ';
                if(i <= traceWhere)
                    whereLen += i->value.length() + 1;
            }
            whereLen -= 2;
            err.reserve(err.size() + 2+whereLen);
            err += L"\n";
            for(size_t i = 0; i < whereLen; i++) err += L'~';
            err += L"^";
            throw err;
        }
        return root;
    }

    void logTree(const SyntaxTree& tree, unsigned short lvl){
        for(decltype(lvl) i = 0; i < lvl; i++)
            std::wcout << L'~';
        std::wcout << L' ';

        if(std::holds_alternative<H::Class::LObject>(tree.value)){
            H::Class::LObjects o = {std::get<H::Class::LObject>(tree.value)};
            std::wcout << rawString(Runner::methodCall(L"toString", o, o[0]->parent)) << std::endl;
            return;
        }
        std::wcout << treeTypes[tree.type] << std::endl;
        const SyntaxTrees& children = std::get<SyntaxTrees>(tree.value);
        for(const SyntaxTree& child : children){
            logTree(child, lvl+1);
        }
    }
}