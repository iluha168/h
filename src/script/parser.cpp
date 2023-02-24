#include "parser.hh"
#include "runner.hh"
#include "../global/Global.hh"
#include "../lang/HClass.hh"

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
    
    SyntaxTrees treesConcat(SyntaxTrees t1, SyntaxTrees t2){
        t1.insert(t1.end(), t2.begin(), t2.end());
        return t1;
    }
    SyntaxTree treeForVariable(std::wstring name){
        return {
            SyntaxTree::Variable,
            H::HStringFromString(name),
        };
    }
    SyntaxTree treeForConstant(H::LObject o){
        return {
            SyntaxTree::Constant,
            o
        };
    }
    SyntaxTree treeForString(std::wstring str){
        return treeForConstant(H::HStringFromString(str));
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
        H::LObject number = H::Object::instantiate(H::Number);
        const auto& helper = [&]{
            double tmp = 1, ijk = 0;
            if(tokens[pos  ].type == Lexer::Token::Sign &&
               tokens[pos+1].type == Lexer::Token::Number){
                if(tokens[pos++].value == L"-") tmp = -1;
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
                (*number->data.number)[ijk] += tmp;
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
    
    SyntaxTrees parseList(size_t& pos, Lexer::Tokens& tokens, const std::wstring separator, const std::wstring end){
        SyntaxTrees args = {};
        for(;;){
            pos++;
            if(tokens[pos].type == Lexer::Token::Punct
            && tokens[pos].value == end){
                pos++;
                return args;
            }
            args.push_back(parseExpression(pos, tokens));
            if(tokens[pos].type == Lexer::Token::Punct){
                const std::wstring punct = tokens[pos].value;
                     if(punct == end) pos--;
                else if(punct == separator) continue;
                else throw std::wstring(L"expected")+separator+L" or "+end+L" in a list.";
            }
            else throw std::wstring(L"expected a punctuation mark.");
        }
        return args;
    }

    SyntaxTrees parseCallArguments(size_t& pos, Lexer::Tokens& tokens, bool optional = false){
        if(tokens[pos].type == Lexer::Token::Punct && tokens[pos].value == L"("){
            return parseList(pos, tokens, L",", L")");
        } else {
            if(optional) return {};
            throw std::wstring(L"expected call arguments.");
        }
    }

    SyntaxTree parseExpression(size_t& pos, Lexer::Tokens& tokens){
        SyntaxTree tempTree;
        switch(tokens[pos].type){
            case Lexer::Token::Reserved: {
                size_t reservedWordIndex = Lexer::indexOf(tokens[pos].value, Lexer::reserved_words);
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
                    case 3: {//while
                        SyntaxTree condition = parseExpression(++pos, tokens);
                        tempTree = {
                            SyntaxTree::ControlFlowWhile,
                            SyntaxTrees({
                                condition, parseExpression(pos, tokens)
                            })
                        };
                    } break;
                    case 4: //break
                        tempTree = {
                            SyntaxTree::ControlFlowBreak,
                            SyntaxTrees({})
                        };
                        pos++;
                    break;
                    case 5: //for
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
                    case 3: //while
                    case 5: //for
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
                switch (Lexer::indexOf(tokens[pos].value, Lexer::all_puncts)){
                case 0: // {
                    tempTree = {SyntaxTree::ScopeRoot, SyntaxTrees()};
                    for(pos++; tokens[pos].type != Lexer::Token::Punct || tokens[pos].value != L"}";){
                        std::get<SyntaxTrees>(tempTree.value).push_back(parseExpression(pos, tokens));
                    };
                    pos++;
                break;
                case 1: // (
                    tempTree = parseExpression(++pos, tokens);
                    if(tokens[pos].type != Lexer::Token::Punct || tokens[pos].value != L")")
                        throw std::wstring(L"unmatched parentheses ')'");
                    pos++;
                break;
                case 2: // [
                    tempTree = {SyntaxTree::OperationNew,
                        treesConcat(
                            {treeForString(L"Array")},
                            parseList(pos, tokens, L",", L"]")
                        )
                    };
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
            switch(Lexer::indexOf(tokens[pos].value, Lexer::all_puncts)){
                case 3: { // .
                    SyntaxTrees classMethod = {
                            tempTree, //object
                            treeForString(tokens[++pos].value), //method name
                    };
                    tempTree = {
                        SyntaxTree::CallMethod,
                        treesConcat(classMethod, parseCallArguments(++pos, tokens))
                    };
                } break;
                case 4: { // =
                    tempTree = {
                        SyntaxTree::OperationAssign, 
                        SyntaxTrees({
                            tempTree,
                            parseExpression(++pos, tokens)
                        })
                    };
                } break;
                case 5: // ;
                    pos++;
                return tempTree;
                case 2: // [
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
                case 6: // <
                case 7: // >
                case 8: // *
                case 15:// ==
                case 16:// ||
                case 17:// &&
                    tempTree = {
                        SyntaxTree::OperationBinary,
                        SyntaxTrees({
                            treeForString(tokens[pos].value),
                            tempTree
                        })
                    };
                    std::get<SyntaxTrees>(tempTree.value).push_back(parseExpression(++pos, tokens));
                break;
                default: //unknown binary operation, ignore
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
        #ifdef DEBUG
            Parser::logTree(root);
        #endif
        return root;
    }

    void logTree(const SyntaxTree& tree, unsigned short lvl){
        for(decltype(lvl) i = 0; i < lvl; i++)
            std::wcout << L'~';
        std::wcout << L' ';

        if(std::holds_alternative<H::LObject>(tree.value)){
            H::LObjects o = {std::get<H::LObject>(tree.value)};
            std::wcout << Runner::safeArgsCall(Global::Strings::toString, o)->data.string << std::endl;
            return;
        }
        std::wcout << treeTypes[tree.type] << std::endl;
        const SyntaxTrees& children = std::get<SyntaxTrees>(tree.value);
        for(const SyntaxTree& child : children){
            logTree(child, lvl+1);
        }
    }
}