CXX := g++
CXXFLAGS := -O3
LDFLAGS := -lpng

SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

steganography: $(OBJ_FILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp makeObjDir
	$(CXX) $(CXXFLAGS) -c -o $@ $<

makeObjDir:
	mkdir -p obj

clean:
	rm steganography
	rm -r obj