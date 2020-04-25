INCLUDES = -I../include \
	 -I../build/deps/eigen-src

SRC = ../src/surface/halfedge_mesh.cpp \
	../src/utilities/disjoint_sets.cpp \
	../src/surface/polygon_soup_mesh.cpp

DEBUG = -g0
OPTIM = -O3 --llvm-opts 3 --llvm-lto 3 --closure 1
CXX = emcc
CXXFLAGS = $(OPTIM) $(DEBUG) -std=c++17 --llvm-lto 1 -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=0 --bind $(INCLUDES)
LDFLAGS = --llvm-lto 1 $(OPTIM) $(DEBUG) -s ALLOW_MEMORY_GROWTH=1 -s WASM=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=0 --bind -s --pre-js ./src/pre-js.js
PRODUCT = build/geometrycentral.js
WASM_PRODUCT = $(PRODUCT:.js=.wasm)

EMBIND_SRC = src/geometrycentral.cpp
OBJ = $(SRC:.cpp=_em.o) $(EMBIND_SRC:.cpp=_em.o)

all: $(PRODUCT)

$(PRODUCT): $(OBJ)
	mkdir -p build
	$(CXX) $(LDFLAGS) -o $@ $^

%_em.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

.PHONY: clean test
clean:
	rm -f $(OBJ) $(PRODUCT) $(WASM_PRODUCT)

test: all
	npm install
	npm run test

