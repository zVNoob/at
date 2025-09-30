NAME := main

SRC_DIR := ./src
BUILD_DIR := .build

CXX := g++
CXXFLAGS :=-std=c++20 -MMD

LD := ld
LIBS := gmp
LDLIBS := $(addprefix -l, $(LIBS))



SRC_DIRS := $(shell find . -type d -not -path '*/.*')

SRCS := $(foreach inc, $(SRC_DIRS), $(wildcard $(inc)/*.cpp))

INC_DIRS := $(foreach inc, $(SRC_DIRS), -I$(inc))

OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

BUILD_DIRS := $(SRC_DIRS:$(SRC_DIR)/%=$(BUILD_DIR)/%/)

all: debug

debug: CXXFLAGS += -g
debug: exec
release: CXXFLAGS += -O2 -flto=auto
release: exec

exec: bison $(NAME)

$(SRC_DIR)/parser.cpp $(SRC_DIR)/parser.hpp: $(SRC_DIR)/parser.y
	bison $^ -o $@ -Wcounterexample

bison: $(SRC_DIR)/parser.cpp

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $@


$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIRS)
	$(CXX) $(CXXFLAGS) $(INC_DIRS) -c -o $@ $<

%/:
	mkdir -p $(@D)

clean:
	rm -rf $(BUILD_DIR)

fclean: clean
	rm -f parser.cpp parser.hpp

-include $(DEPS)


.PHONY: all clean bison fclean




