CXX ?= clang++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -Iinclude -O2
BUILD_DIR := build
SRC := src/anomaly_detector.cpp src/cell.cpp src/scheduler.cpp src/simulation.cpp src/telemetry.cpp
TESTS := test_anomaly_detector test_scheduler test_simulation test_telemetry

.PHONY: all test clean demo

all: $(BUILD_DIR)/ranedge-sim

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/ranedge-sim: $(SRC) src/main.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) src/main.cpp -o $@

$(BUILD_DIR)/%: tests/%.cpp $(SRC) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(SRC) $< -o $@

test: $(addprefix $(BUILD_DIR)/,$(TESTS))
	@for test in $(TESTS); do ./$(BUILD_DIR)/$$test; done

demo: $(BUILD_DIR)/ranedge-sim
	./$(BUILD_DIR)/ranedge-sim --ticks 10

clean:
	rm -rf $(BUILD_DIR)
