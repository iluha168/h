# h
## The h programming language
Is written in C++ using STD and X11 libraries.
Learn more at [the wiki](https://github.com/iluha168/h/wiki).
### Notes
* Requires compiler that supports C++20 standart.
* Tested on Windows 10 with Cygwin and Debian 11
## Compilation
Requires:
- `rm`
- `g++` ^14.2.0
- `make` ^4.4.1

CWD: at the root of this repository

Run:
- `make main` (default)
- `make debug` (prints debug info for lexer, parser, and runtime)

Produces:
- `./dist/h` - executable, the interpreter

## Usage of the interpreter
Run a script:
```sh
h /PATH/TO/script.wh
```
Enter REPL:
```sh
h
```
