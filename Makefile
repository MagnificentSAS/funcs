CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude
GTEST_LIBS ?= -lgtest -lgtest_main -lpthread

BUILD_DIR ?= build
TEST_BIN := $(BUILD_DIR)/tests
TEST_SRCS := $(wildcard test/*.cpp)
SOURCES := src/base_funcs.cpp

.PHONY: test clean

test:
	@rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(TEST_SRCS) $(SOURCES) $(GTEST_LIBS) -o $(TEST_BIN) || \
		{ rm -rf $(BUILD_DIR); exit 1; }
	@$(TEST_BIN); status=$$?; rm -rf $(BUILD_DIR); exit $$status

clean:
	rm -rf $(BUILD_DIR)
