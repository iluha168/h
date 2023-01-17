# h
## The h programming language
Is written in C++ using STD and X11 libraries.
Learn more at [the wiki](https://github.com/iluha168/h/wiki).
## Compilation
### Notes
* Requires compiler that supports C++20 standart.
* Assumes a running X11 server. If there aren't any, generates [segmentation fault](https://en.wikipedia.org/wiki/Segmentation_fault).
### Command
Assuming CWD set to the root of this repository.
```sh
gcc ./src/*.cpp ./src/*/*.cpp -o ./h -std=c++20 -pipe -O3 -Wall -Wextra -Wpedantic -lX11 -lstdc++ -lm
```
## Usage of the interpreter
Run a script:
```sh
h /PATH/TO/script.wh
```
