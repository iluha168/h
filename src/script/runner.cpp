#include "runner.hh"
#include "../lang/HClass.hh"

namespace Runner {
    std::wstring getVariableName(H::LObject& o, H::Entries& variables) noexcept{
        for(auto& kv : variables)
            if(kv.second == o)
                return kv.first;
        return L"<unnamed>";
    }
    
    H::LObject safeArgsCall(const std::wstring& name, H::LObjects& args, H::Entries& scope){
        try {
            return args[0]->call(name, args);
        } catch (std::wstring& err){
            err = name+err;
            H::LObject p = args[0];
            do err = getVariableName(p, scope)+L"."+err;
            while (p = p->parent);
            throw err;
        }
    }
    
    H::LObjects execSubtrees(const Parser::SyntaxTree& parent, H::Entries& scope){
        const Parser::SyntaxTrees& trees = std::get<Parser::SyntaxTrees>(parent.value);
        H::LObjects children(trees.size());
        std::transform(trees.begin(), trees.end(), children.begin(),
            [&scope](const Parser::SyntaxTree& tree){
                return execTree(tree, scope);
            }
        );
        return children;
    }

    H::LObject execTree(const Parser::SyntaxTree& tree, H::Entries& outerScope){
        switch(tree.type){
            case Parser::SyntaxTree::CallMethod: {
                H::LObjects args = execSubtrees(tree, outerScope);
                std::wstring& methodName = rawString(args[1]);
                args.erase(args.begin()+1);
            return safeArgsCall(methodName, args);
            }
            case Parser::SyntaxTree::Constant:
            return std::get<H::LObject>(tree.value);
            case Parser::SyntaxTree::OperationAssign: {
                H::LObjects args = execSubtrees(tree, outerScope);
                outerScope.insert_or_assign(rawString(args[0]), args[1]);
            return args[1];
            }
            case Parser::SyntaxTree::ControlFlowIf: {
                const Parser::SyntaxTrees& subtrees = std::get<Parser::SyntaxTrees>(tree.value);
                try {
                    if(rawBool(execTree(subtrees[0], outerScope))){
                        return execTree(subtrees[1], outerScope);
                    } else if(subtrees.size() > 2){
                        return execTree(subtrees[2], outerScope);
                    }
                } catch(std::bad_variant_access&){
                    throw std::wstring(L"the if statement requires a Boolean condition");
                }
            } return H::null;
            case Parser::SyntaxTree::ControlFlowWhile: {
                const Parser::SyntaxTrees& subtrees = std::get<Parser::SyntaxTrees>(tree.value);
                H::LObject last = H::null;
                try {
                    while(rawBool(execTree(subtrees[0], outerScope))){
                        last = execTree(subtrees[1], outerScope);
                    }
                } catch(std::bad_variant_access&){
                    throw std::wstring(L"the while statement requires a Boolean condition");
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
                    for(       execTree(subtrees[0], outerScope);
                       rawBool(execTree(subtrees[1], outerScope));
                               execTree(subtrees[2], outerScope)){
                        last = execTree(subtrees[3], outerScope);
                    }
                } catch(std::bad_variant_access&){
                    throw std::wstring(L"the for statement requires a Boolean condition");
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
                std::wstring& operatorName = rawString(args[0]);
                args.erase(args.begin());
            return safeArgsCall(operatorName, args);
            }
            case Parser::SyntaxTree::OperationNew: {
                H::LObjects args = execSubtrees(tree, outerScope);
                try {
                    H::LObject instantiator = outerScope.at(rawString(args[0]));
                    args.erase(args.begin());
                    return H::Object::instantiate(instantiator, args);
                } catch(std::out_of_range&) {
                    throw rawString(args[0]) + L" was not defined";
                }
            }
            case Parser::SyntaxTree::ScopeRoot: {
                //H::Entries localScope(outerScope);
                auto result = execSubtrees(tree, outerScope);
                return (result.size()? (*(result.end()-1)) : H::null);
            }
            case Parser::SyntaxTree::Variable: {
                std::wstring& varName = rawString(std::get<H::LObject>(tree.value));
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

    H::LObject run(Parser::SyntaxTree& tree, H::Entries& variables){
        try {
            return execTree(tree, variables);
        } catch(std::wstring& err){
            throw L"Runner: "+err;
        } catch(Exceptions::Break&){
            throw std::wstring(L"Runner: break outside a loop");
        }
    }
}