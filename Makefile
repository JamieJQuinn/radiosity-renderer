CC=g++
CFLAGS=-c -Wall -I$(INCLUDE_DIR)
LDFLAGS=
SRC_DIR=src
BUILD_DIR=build
INCLUDE_DIR=include

SOURCES=$(wildcard $(SRC_DIR)/*.cpp)
OBJECTS=$(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
DEPENDENCIES=$(wildcard $(INCLUDE_DIR)/*.hpp)
EXECUTABLE=radiosity

TEST_DIR=test
TEST_SOURCES=$(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJECTS=$(patsubst $(TEST_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(TEST_SOURCES))

TEST_EXECUTABLE=radiosity_test

all: release

debug: CFLAGS += -DDEBUG -g
debug: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

release: CFLAGS += -DNDEBUG -O3
release: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

profile: CFLAGS += -pg
profile: LDFLAGS += -pg -no-pie
profile: release

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPENDENCIES)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: test
test: all $(BUILD_DIR)/$(TEST_EXECUTABLE)
	$(BUILD_DIR)/$(TEST_EXECUTABLE)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(LDFLAGS) $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS)) $(TEST_OBJECTS) -o $@
