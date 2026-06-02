CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -Wpedantic -Werror -Iinclude -MMD -MP
BUILD_DIR := build
SRC_DIR := src
TEST_DIR := tests
EXAMPLE_DIR := examples

LIB_OBJS := $(BUILD_DIR)/thread_pool.o
EXAMPLE_OBJS := $(BUILD_DIR)/demo.o
TEST_OBJS := $(BUILD_DIR)/thread_pool_test.o
DEPS := $(LIB_OBJS:.o=.d) $(EXAMPLE_OBJS:.o=.d) $(TEST_OBJS:.o=.d)

.PHONY: all demo test clean

all: demo test

demo: $(BUILD_DIR)/demo

test: $(BUILD_DIR)/thread_pool_test
	./$(BUILD_DIR)/thread_pool_test

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/demo: $(LIB_OBJS) $(EXAMPLE_OBJS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/thread_pool_test: $(LIB_OBJS) $(TEST_OBJS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(BUILD_DIR)/thread_pool.o: $(SRC_DIR)/thread_pool.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/demo.o: $(EXAMPLE_DIR)/demo.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/thread_pool_test.o: $(TEST_DIR)/thread_pool_test.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

-include $(DEPS)
