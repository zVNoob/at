NAME := main

SRC_DIR := ./src
BUILD_DIR := .build

CXX := g++
LLVM_CONFIG := llvm-config-17
LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS := $(shell $(LLVM_CONFIG) --ldflags --libs core support native)
LLVM_LIBS := $(shell $(LLVM_CONFIG) --libs core support native orcjit object)

CXXFLAGS :=-std=c++20 -MMD -I src -I src/generated $(LLVM_CXXFLAGS) -fexceptions
LDFLAGS := $(LLVM_LDFLAGS)
LDLIBS := $(LLVM_LIBS) -lelf

LDLIBS := $(LLVM_LIBS) -lelf


SRC_DIRS := $(shell find $(SRC_DIR) -type d -not -path '*/.*')
SRCS := $(filter-out ./src/generated/%, $(foreach inc, $(SRC_DIRS), $(wildcard $(inc)/*.cpp)))
GEN_SRCS := src/generated/ast_parser.cpp
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o) $(BUILD_DIR)/generated/ast_parser.o
DEPS := $(OBJS:.o=.d)

# Every library source except the CLI entry point, for linking tests.
TEST_SRCS := $(filter-out ./src/main.cpp, $(SRCS)) $(GEN_SRCS)

BUILD_DIRS := $(SRC_DIRS:$(SRC_DIR)/%=$(BUILD_DIR)/%/)

all: debug
debug: CXXFLAGS += -g
debug: exec
release: CXXFLAGS += -O2 -flto=auto
release: exec
exec: $(NAME)
$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDLIBS) -o $@
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIRS)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(BUILD_DIR)/generated/ast_parser.o: src/generated/ast_parser.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%/:
	mkdir -p $(@D)

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)

test: $(NAME)
	@for f in tests/*.cpp; do \
		base=$$(basename $$f .cpp); \
		$(CXX) $(CXXFLAGS) -I src -I src/generated $$f $(TEST_SRCS) $(LDLIBS) -o /tmp/opencode/$$base 2>&1 || exit 1; \
		/tmp/opencode/$$base || exit 1; \
	done
	@echo "All tests passed"

# Build std module .o from C runtime
std_modules/io.o: src/at_runtime.cpp
	$(CXX) -c -o $@ $<

dump: $(NAME)
	$(CXX) $(CXXFLAGS) -I src -I src/generated tools/dump_ast.cpp src/lexer.cpp src/parser.cpp src/generated/ast_parser.cpp -o dump_ast

.PHONY: all clean test dump
