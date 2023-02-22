#include "runner.hh"
#include "../lang/HClass.hh"

namespace Runner {
    std::wstring getVariableName(H::LObject& o, H::VarScope& variables) noexcept{
        for(auto& kv : variables)
            if(kv.second == o)
                return kv.first;
        return L"<temporary>";
    }

    std::wstring fullVariableName(H::LObject& o, H::VarScope& variables) noexcept{
        std::wstring name;
        H::LObject& p = o;
        do name = getVariableName(o, variables)+L"."+name;
        while (p = p->parent);
        return name;
    }
    
    H::LObject safeArgsCall(const std::wstring& name, H::LObjects& args, H::VarScope& scope){
        try {
            return args[0]->call(name, args);
        } catch (std::wstring& err){
            throw fullVariableName(args[0], scope)+err;
        } catch (std::out_of_range& err){
            throw std::wstring(L"Not enough arguments passed for ")+fullVariableName(args[0], scope);
        }
    }
    
    H::LObjects execSubtrees(const Parser::SyntaxTree& parent, H::VarScope& scope){
        const Parser::SyntaxTrees& trees = std::get<Parser::SyntaxTrees>(parent.value);
        H::LObjects children(trees.size());
        std::transform(trees.begin(), trees.end(), children.begin(),
            [&scope](const Parser::SyntaxTree& tree){
                return execTree(tree, scope);
            }
        );
        return children;
    }

    H::LObject execTree(const Parser::SyntaxTree& tree, H::VarScope& outerScope){
        switch(tree.type){
            case Parser::SyntaxTree::CallMethod: {
                H::LObjects args = execSubtrees(tree, outerScope);
                std::wstring& methodName = *args[1]->data.string;
                args.erase(args.begin()+1);
            return safeArgsCall(methodName, args);
            }
            case Parser::SyntaxTree::Constant:
            return std::get<H::LObject>(tree.value);
            case Parser::SyntaxTree::OperationAssign: {
                H::LObjects args = execSubtrees(tree, outerScope);
                outerScope.insert_or_assign(*args[0]->data.string, args[1]);
            return args[1];
            }
            case Parser::SyntaxTree::ControlFlowIf: {
                const Parser::SyntaxTrees& subtrees = std::get<Parser::SyntaxTrees>(tree.value);
                H::LObject condition = execTree(subtrees[0], outerScope);
                if(condition->parent != H::Boolean)
                    throw std::wstring(L"the if statement requires a Boolean condition");
                if(condition->data.boolean){
                    return execTree(subtrees[1], outerScope);
                } else if(subtrees.size() > 2){
                    return execTree(subtrees[2], outerScope);
                }
            } return H::null;
            case Parser::SyntaxTree::ControlFlowWhile: {
                const Parser::SyntaxTrees& subtrees = std::get<Parser::SyntaxTrees>(tree.value);
                H::LObject last = H::null;
                try {
                    for(;;){
                        H::LObject condition = execTree(subtrees[0], outerScope);
                        if(condition->parent != H::Boolean)
                            throw std::wstring(L"the while statement requires a Boolean condition");
                        if(!condition->data.boolean) break;
                        last = execTree(subtrees[1], outerScope);
                    }
                } catch (Exceptions::Break&){
                    if(subtrees.size() > 2)
                        return execTree(subtrees[2], outerScope);
                }
            return last;
            }
            case Parser::SyntaxTree::ControlFlowFor: {
                const Parser::SyntaxTrees& subtrees = std::get<Parser::SyntaxTrees>(tree.value);
                H::LObject last = H::null;
                try {
                    execTree(subtrees[0], outerScope);
                    for(;;){
                        H::LObject condition = execTree(subtrees[1], outerScope);
                        if(condition->parent != H::Boolean)
                            throw std::wstring(L"the for statement requires a Boolean condition");
                        if(!condition->data.boolean) break;
                        last = execTree(subtrees[3], outerScope);
                        execTree(subtrees[2], outerScope);
                    }
                } catch (Exceptions::Break&){
                    if(subtrees.size() > 4)
                        return execTree(subtrees[4], outerScope);
                }
            return last;
            }
            case Parser::SyntaxTree::ControlFlowBreak:
            throw Exceptions::Break();
            case Parser::SyntaxTree::OperationBinary: {
                H::LObjects args = execSubtrees(tree, outerScope);
                std::wstring& operatorName = *args[0]->data.string;
                args.erase(args.begin());
            return safeArgsCall(operatorName, args);
            }
            case Parser::SyntaxTree::OperationNew: {
                H::LObjects args = execSubtrees(tree, outerScope);
                try {
                    H::LObject instantiator = outerScope.at(*args[0]->data.string);
                    args.erase(args.begin());
                    return H::Object::instantiate(instantiator, args);
                } catch(std::out_of_range&) {
                    throw *args[0]->data.string + L" was not defined";
                }
            }
            case Parser::SyntaxTree::ScopeRoot: {
                //H::VarScope localScope(outerScope);
                H::LObjects result = execSubtrees(tree, outerScope);
                return (result.size()? (*(result.end()-1)) : H::null);
            }
            case Parser::SyntaxTree::Variable: {
                std::wstring& varName = *std::get<H::LObject>(tree.value)->data.string;
                try {
                    return outerScope.at(varName);
                } catch(std::out_of_range&) {
                    throw varName + L" was not defined";
                }
            }
            default:
            throw std::wstring(L"unknown syntax tree type");
        }
    }

    H::LObject run(Parser::SyntaxTree& tree, H::VarScope& variables){
        try {
            return execTree(tree, variables);
        } catch(std::wstring& err){
            throw L"Runner: "+err;
        } catch(Exceptions::Break&){
            throw std::wstring(L"Runner: break outside a loop");
        }
    }
}