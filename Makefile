CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Iinclude
GTEST_LIBS ?= -lgtest -lgtest_main -lpthread

BUILD_DIR ?= build
TEST_BIN := $(BUILD_DIR)/tests
TEST_SRCS := $(wildcard test/*.cpp)
SOURCES := src/base_funcs.cpp src/factory.cpp

LIB := libfuncs.a
LIB_OBJECTS := $(SOURCES:.cpp=.o)

.PHONY: test test-call test-deriv test-to_string test-factory lib clean

define RUN_TEST
	@rm -rf $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(1) $(SOURCES) $(GTEST_LIBS) -o $(TEST_BIN) || \
		{ rm -rf $(BUILD_DIR); exit 1; }
	@$(TEST_BIN); status=$$?; rm -rf $(BUILD_DIR); exit $$status
endef

test:
	$(call RUN_TEST,$(TEST_SRCS))

test-call:
	$(call RUN_TEST,test/call_test.cpp)

test-deriv:
	$(call RUN_TEST,test/get_deriv_test.cpp)

test-to_string:
	$(call RUN_TEST,test/to_string_test.cpp)

test-factory:
	$(call RUN_TEST,test/factory_test.cpp)

lib: $(LIB)

$(LIB): $(LIB_OBJECTS)
	ar rcs $@ $^
	rm -f $(LIB_OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(LIB) $(LIB_OBJECTS)
