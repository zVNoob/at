NAME := main

SRC_DIR := .

INCS := $(shell find . -type d -not -path '*/.*')

SRCS := $(foreach inc, $(INCS), $(wildcard $(inc)/*.cpp))

SRCS := $(SRCS:%=$(SRC_DIR)/%)

BUILD_DIR := .build
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

CXX := g++
CXXFLAGS :=-std=c++20
CPPFLAGS := -MMD $(addprefix -I, $(INCS))

LD := ld
LIBS := gmp
LDLIBS := $(addprefix -l, $(LIBS))

all: debug

debug: CXXFLAGS += -g
debug: exec
release: CXXFLAGS += -O2 -flto=auto
release: exec

exec: bison $(NAME)

parser.cpp: parser.y
	bison $^ -o $@ -Wcounterexample

bison: parser.cpp

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $@

-include $(DEPS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f parser.cpp parser.hpp

.PHONY: all clean bison fclean

