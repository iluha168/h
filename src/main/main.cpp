#include <sstream>
#include <fstream>
#include <codecvt>
#include <iomanip>

#include "../global/Global.hh"
#include "../lang/HClass.hh"
#include "../script/runner.hh"

// return codes
#define RETCODE_ERR_FILE -1
#define RETCODE_SUCCESS 0
#define RETCODE_ARGS_AMOUNT 1
#define RETCODE_SCRIPT_ERROR 2

std::wstring readUTF8(const char* filename)
{
	std::wifstream wif(filename, std::ios_base::in);
	if(!wif.is_open()){
		printf("File open error");
		exit(RETCODE_ERR_FILE);
	}
    wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    std::wstringstream wss;
    wss << wif.rdbuf();
	wif.close();
    return wss.str();
}

int main(int argc, char** argv) {
	//one-time init
	Runner::Entries globalScope{};
	Runner::classes = {H::Window, H::Number, H::String, H::Boolean, H::Array};
    H::null = H::Uninitialized->instantiate();
	for(bool i : {false, true})
		(H::Booleans[i] = H::Boolean->instantiate())->data = i;
	srand(time(nullptr));
	// read cmd arguments
	switch(argc){
		case 1: {
			std::wcout << L"h REPL - Hit Ctrl+C to exit" << std::endl;
			H::Class::LObject toString = Parser::HStringFromWString(L"toString");
			std::wstring code{};
			while(std::wcout<<L"> "<<std::flush, std::getline(std::wcin, code)){
				try {
					auto tokens = Lexer::tokenize(code);
					auto tree = Parser::syntaxTreeFor(tokens);
					H::Class::LObjects result = {Runner::run(tree, globalScope)};
					std::wcout << rawString(Runner::methodCall(toString, result, result[0]->parent)) << std::endl;
				} catch(std::wstring& e){
					std::wcerr << e << std::endl;
				}
			}
		} return RETCODE_SUCCESS;
		case 2:
			// run script
			try {
				Lexer::Tokens tokens = Lexer::tokenize(readUTF8(argv[1]));
				for(size_t i = 0; i < tokens.size(); i++){
					std::wcout << i << " " << Lexer::tokenTypes[tokens[i].type] << L" | " << tokens[i].value << std::endl;
				}
				Parser::SyntaxTree tree = Parser::syntaxTreeFor(tokens);
				Parser::logTree(tree);
				Runner::Entries globalScope{};
				Runner::run(tree, globalScope);
			} catch (std::exception& e) {
				std::wcerr << "Script "<<argv[1]<<" caused native error:\n"<<typeid(e).name()<<L"\n"<<e.what() << std::endl;
				return RETCODE_SCRIPT_ERROR;
			} catch (std::wstring& e) {
				std::wcerr << e << std::endl;
				return RETCODE_SCRIPT_ERROR;
			}
		break;
		default:
			std::wcerr << "Incorrect amount of arguments" << std::endl;
		return RETCODE_ARGS_AMOUNT;
	}
	// event loop
	XEvent event;
	while(!H::Class::refs.empty()){
		XNextEvent(Global::dis, &event);
		switch(event.type){
			case ClientMessage:
				//find and destroy window
				if((Atom)event.xclient.data.l[0] == Global::wmDeleteWindow){
					auto it = std::find_if(H::Class::refs.begin(), H::Class::refs.end(),
						[&event](H::Class::LObject& w){
							try {
								return rawWin(w) == event.xclient.window;
							} catch(...) {return false;}
						}
					);
					if(it == H::Class::refs.end()) break;
					H::Class::unref(*it);
				}
			break;
			default:
				//std::wcout << L"Event type: " << event.type << std::endl;
			break;
		}
	}
	return RETCODE_SUCCESS;
}