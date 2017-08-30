# ======= OPTIONS =======
#RADIOSITY_MODE=GATHERING
RADIOSITY_MODE=SHOOTING
FORM_FACTOR_CALCULATION=PROGRESSIVE
#FORM_FACTOR_CALCULATION=NOT_PROGRESSIVE
HEMICUBE_NEAR_PLANE=0.01f
HEMICUBE_GRID_SIZE=512
DIFF_TO_TOTAL_CUTOFF=0.01f
MAX_PASSES=32
#========================

OPTIONS=-D$(RADIOSITY_MODE) -D$(FORM_FACTOR_CALCULATION) -DHEMICUBE_GRID_SIZE=$(HEMICUBE_GRID_SIZE) -DHEMICUBE_NEAR_PLANE=$(HEMICUBE_NEAR_PLANE) -DDIFF_TO_TOTAL_CUTOFF=$(DIFF_TO_TOTAL_CUTOFF) -DMAX_PASSES=$(MAX_PASSES)

CC=g++
CFLAGS=-c -Wall -I$(INCLUDE_DIR) $(OPTIONS) -std=c++11
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

opengl: LDFLAGS += -lglfw -lGLEW -lGL
opengl: CFLAGS += -DOPENGL
opengl: release

debug-opengl: CFLAGS += -DOPENGL
debug-opengl: LDFLAGS += -lglfw -lGLEW -lGL
debug-opengl: debug

profile-opengl: CFLAGS += -DOPENGL
profile-opengl: LDFLAGS += -lglfw -lGLEW -lGL
profile-opengl: profile

profile-debug-opengl: CFLAGS += -pg
profile-debug-opengl: LDFLAGS += -pg -no-pie
profile-debug-opengl: debug-opengl

debug: CFLAGS += -DDEBUG -g
debug: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

release: CFLAGS += -fopenmp -DNDEBUG -O3
release: LDFLAGS += -fopenmp
release: $(BUILD_DIR) $(BUILD_DIR)/$(EXECUTABLE)

profile: CFLAGS += -pg
profile: LDFLAGS += -pg -no-pie
profile: release

$(BUILD_DIR)/$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEPENDENCIES)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: test
test: CFLAGS += -DDEBUG -g
test: debug $(BUILD_DIR)/$(TEST_EXECUTABLE)
	$(BUILD_DIR)/$(TEST_EXECUTABLE)

$(BUILD_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TEST_EXECUTABLE): $(TEST_OBJECTS) $(OBJECTS)
	$(CC) $(filter-out $(BUILD_DIR)/main.o, $(OBJECTS)) $(TEST_OBJECTS) -o $@ $(LDFLAGS)
