CC = g++
MKDIR = mkdir
CFLAGS = -std=c++20 -g -pipe -O0 -Wall -Wextra -Wpedantic -lX11 -lstdc++ -lm

SOURCES = $(wildcard ../src/*/*.cpp)
HEADERS = $(wildcard ../src/*/*.hh )
OBJECTS = $(notdir $(patsubst %.cpp, %.o, $(SOURCES)))

main: $(OBJECTS)
	$(CC) $^ -o ./h.exe $(CFLAGS)

%.o: ../src/*/%.cpp $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS) -I$(dir $<)

clean:
	rm *.o
