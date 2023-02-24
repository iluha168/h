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
#define RETCODE_XSERVER_UNAVAILABLE 3

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

void HInitialize(){
	if(!(Global::dis = XOpenDisplay(nullptr))){
		std::wcerr << L"X11 server is not available" << std::endl;
		exit(RETCODE_XSERVER_UNAVAILABLE);
	}
	Global::wmDeleteWindow = XInternAtom(Global::dis, "WM_DELETE_WINDOW", false);
	Global::scr = ScreenOfDisplay(Global::dis, DefaultScreen(Global::dis));
	using namespace H;
	srand(time(nullptr));
					//! The order is significant
	for(void(*f)() : {NativeFunctionInit, UninitializedInit, NumberInit, StringInit, BooleanInit, WindowInit, ArrayInit})
		f();
	emptyF = HNativeFunctionFromFunctionType([](LObjects& o){
        return o[0];
    });
	for(bool i : {false, true})
		(Booleans[i] = Object::instantiate(Boolean))->data.boolean = i;
    null = Object::instantiate(Uninitialized);
	Global::Scope = {
		{L"false", Booleans[0]},
		{L"true", Booleans[1]},
		{L"null", null},
		{L"Window", H::Window},
		{L"Number", Number},
		{L"String", String},
		{L"Uninitialized", Uninitialized},
		{L"Boolean", Boolean},
		{L"Array", Array},
	};
}

int main(int argc, char** argv) {
	//one-time init
	HInitialize();
	// read cmd arguments
	switch(argc){
		case 1: {
			std::wcout << L"h REPL - Hit Ctrl+C to exit" << std::endl;
			std::wstring code{};
			while(std::wcout<<L"> "<<std::flush, std::getline(std::wcin, code)){
				try {
					auto tokens = Lexer::tokenize(code);
					auto tree = Parser::syntaxTreeFor(tokens);
					H::LObjects result = {Runner::run(tree, Global::Scope)};
					H::LObject output = Runner::safeArgsCall(Global::Strings::toString, result);
					std::wcout << (output->parent == H::String? *output->data.string : L"¯\\_(ツ)_/¯") << std::endl;
				} catch(std::wstring& e){
					std::wcerr << e << std::endl;
				} catch(std::bad_cast& e){
					std::wcerr << L"Type conversion error!" << std::endl;
				} catch(std::out_of_range& e){
					std::wcerr << L"Not enough arguments passed!" << std::endl;
				}
			}
		} return RETCODE_SUCCESS;
		case 2:
			// run script
			try {
				Lexer::Tokens tokens = Lexer::tokenize(readUTF8(argv[1]));
				Parser::SyntaxTree tree = Parser::syntaxTreeFor(tokens);
				Runner::run(tree, Global::Scope);
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
	while(!H::Object::refs.empty()){
		XNextEvent(Global::dis, &event);
		switch(event.type){
			case ClientMessage:
				//find and destroy window
				if((Atom)event.xclient.data.l[0] == Global::wmDeleteWindow){
					auto it = std::find_if(H::Object::refs.begin(), H::Object::refs.end(),
						[&event](H::LObject& w){
							try {
								return w->data.window->first == event.xclient.window;
							} catch(...) {return false;}
						}
					);
					if(it == H::Object::refs.end()) break;
					H::Object::unref(*it);
				}
			break;
			case Expose:
			break;
				XFlush(Global::dis);
			default:
				//std::wcout << L"Event type: " << event.type << std::endl;
			break;
		}
	}
	return RETCODE_SUCCESS;
}