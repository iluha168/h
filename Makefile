RM = rm -rf
CXXFLAGS = -std=c++20 -pipe -Wall -Wextra -Wpedantic -lX11 -lstdc++ -lm -O0
CXX = g++

DIST = ./dist/
DIST_OBJECTS = $(addprefix $(DIST), obj/)
SOURCES = $(wildcard ./src/*/*.cpp)
HEADERS = $(wildcard ./src/*/*.hh )
OBJECTS = $(addprefix $(DIST_OBJECTS), $(notdir $(patsubst %.cpp, %.o, $(SOURCES))))

main: $(OBJECTS)
	$(CXX) $^ -o $(addprefix $(DIST), h) $(CXXFLAGS)

$(DIST_OBJECTS)%.o: ./src/*/%.cpp $(HEADERS)
	@mkdir -p $(DIST)
	@mkdir -p $(DIST_OBJECTS)
	$(CXX) -c $< -o $@ -I$(dir $<) $(CXXFLAGS)

clean:
	$(RM) $(DIST)

debug: CXXFLAGS += -g -DDEBUG
debug: main
