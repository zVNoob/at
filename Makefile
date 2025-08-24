NAME := main

SRC_DIR := .

SRCS := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/objects/*.cpp)

SRCS := $(SRCS:%=$(SRC_DIR)/%)

BUILD_DIR := .build
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

CXX := g++
CXXFLAGS :=-std=c++20 -g
CPPFLAGS :=
LIBS := gmp
LDLIBS := $(addprefix -l, $(LIBS))

all: bison $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

bison: parser.y
	[ -e $(^:.y=.cpp) ] && mv $(^:.y=.cpp) $(addprefix ~,$(^:.y=.cpp)) || true
	[ -e $(^:.y=.hpp) ] && mv $(^:.y=.hpp) $(addprefix ~,$(^:.y=.hpp)) || true
	bison $^ -o $(^:.y=.cpp) -Wcounterexample
	if diff $(^:.y=.hpp) $(addprefix ~,$(^:.y=.hpp)) >/dev/null;\
	then mv $(addprefix ~,$(^:.y=.hpp)) $(^:.y=.hpp);mv $(addprefix ~,$(^:.y=.cpp)) $(^:.y=.cpp);\
	else rm $(addprefix ~,$(^:.y=.hpp));rm $(addprefix ~,$(^:.y=.cpp));\
	fi

.PHONY: all clean bison


