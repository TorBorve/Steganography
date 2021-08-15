CXX := g++
CXXFLAGS := ...

SRC_DIR := src
OBJ_DIR := obj
SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC_FILES))

steganography: $(OBJ_FILES)
	$(CXX) -o $@ $^ -lpng

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp makeObjDir
	$(CXX) -c -o $@ $<

makeObjDir:
	mkdir -p obj

clean:
	rm steganography
	rm -r obj