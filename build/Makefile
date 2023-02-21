RM = rm
CXXFLAGS = -std=c++20 -pipe -Wall -Wextra -Wpedantic -lX11 -lstdc++ -lm -O0
CXX = g++

debug: CXXFLAGS += -g -DDEBUG
debug: main

SOURCES = $(wildcard ../src/*/*.cpp)
HEADERS = $(wildcard ../src/*/*.hh )
OBJECTS = $(notdir $(patsubst %.cpp, %.o, $(SOURCES)))

main: $(OBJECTS)
	$(CXX) $^ -o ./h $(CXXFLAGS)

%.o: ../src/*/%.cpp $(HEADERS)
	$(CXX) -c $< -o $@ -I$(dir $<) $(CXXFLAGS)

clean:
	$(RM) *.o
