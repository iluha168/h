#include "runner.hh"

namespace Runner {
    std::vector<H::LClass> classes{};

    H::LClass getClassByName(std::wstring className){
        for(H::LClass lclass : classes){
            if(lclass->name == className)
                return lclass;
        }
        throw std::wstring(L"no such class exists: ")+className;
    }
    std::wstring getVariableName(H::Class::LObject& o, Entries& variables){
        std::wclog << "Attempt to find "<<o<<" in:\n";
        for(auto& kv : variables){
            std::wclog<<kv.second<<std::endl;
            if(kv.second == o) return kv.first;
        }
        throw std::wstring(L"internal error you shouldn't see");
    }
    
    H::Class::LObject methodCall(std::wstring methodName, H::Class::LObjects& arguments, H::LClass provider){
        const std::wstring fullName = provider->name + L"." + methodName;
        try {
            H::Class::Function& method = H::emptyF;
            try {
                method = provider->prototype.at(methodName);
            } catch(std::out_of_range&) {
                throw fullName + L" is not defined";
            }
            return method(arguments);
        } catch(std::out_of_range&){
            throw fullName + L": not enough arguments";
        } catch(std::bad_variant_access&){
            throw fullName + L": invalid arguments' types passed";
        }
    }
    H::Class::LObject methodCall(H::Class::LObject methodName, H::Class::LObjects& arguments, H::LClass provider){
        try {
            return methodCall(rawString(methodName), arguments, provider);
        } catch(std::bad_variant_access&){
            throw std::wstring(L"provided method name is not a string");
        }
    }
    
    H::Class::LObjects execSubtrees(const Parser::SyntaxTree& parent, Entries& scope){
        const Parser::SyntaxTrees& trees = std::get<Parser::SyntaxTrees>(parent.value);
        H::Class::LObjects children{};
        std::transform(trees.begin(), trees.end(), std::back_inserter(children),
            [&scope](const Parser::SyntaxTree& tree){
                return execTree(tree, scope);
            }
        );
        return children;
    }

    H::Class::LObject execTree(const Parser::SyntaxTree& tree, Entries& outerScope){
        switch(tree.type){
            case Parser::SyntaxTree::CallMethod: {
                H::Class::LObjects args = execSubtrees(tree, outerScope);
                H::Class::LObject methodName = args.at(1);
                args.erase(args.begin()+1);
            return methodCall(methodName, args, args.at(0)->parent);
            }
            case Parser::SyntaxTree::Constant:
            return std::get<H::Class::LObject>(tree.value);
            case Parser::SyntaxTree::OperationAssign: {
                H::Class::LObjects args = execSubtrees(tree, outerScope);
                outerScope.insert_or_assign(rawString(args.at(0)), args.at(1));
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
                try {
                    while(rawBool(execTree(subtrees[0], outerScope))){
                        execTree(subtrees[1], outerScope);
                    }
                } catch(std::bad_variant_access&){
                    throw std::wstring(L"the while statement requires a Boolean condition");
                } catch (Exceptions::Break&){}
            } return H::null;
            case Parser::SyntaxTree::ControlFlowBreak:
            throw Exceptions::Break();
            case Parser::SyntaxTree::OperationBinary: {
                H::Class::LObjects args = execSubtrees(tree, outerScope);
                H::Class::LObject operatorName = args.at(0);
                args.erase(args.begin());
            return methodCall(operatorName, args, args.at(0)->parent);
            }
            case Parser::SyntaxTree::OperationNew: {
                H::Class::LObjects args = execSubtrees(tree, outerScope);
                H::LClass instantiator = getClassByName(rawString(args.at(0)));
                args.erase(args.begin());
            return instantiator->instantiate(args);
            }
            case Parser::SyntaxTree::ScopeRoot: {
                //Entries localScope(outerScope);
                execSubtrees(tree, outerScope);
            }
            return H::null;
            case Parser::SyntaxTree::Variable: {
                std::wstring& varName = rawString(std::get<H::Class::LObject>(tree.value));
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

    void run(Parser::SyntaxTree& tree, Entries& variables){
        try {
            execTree(tree, variables);
        } catch(std::wstring& err){
            throw L"Runner: "+err;
        } catch(Exceptions::Break&){
            throw std::wstring(L"Runner: break outside a loop");
        }
    }
}