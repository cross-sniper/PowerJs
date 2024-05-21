# Define compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -g -fPIC
LDLIBS = -lduktape
MODULE_PATH = ~/.mix/libs/

# Modules
MODULES = fetch file core example server os raylib

# Flags for specific modules
SDL_LIBS = -lSDL2
RAYLIB_LIBS = -lraylib

# Target: Compile a specific module
module_%: modules/%.cpp
	@echo "Compiling $<..."
	$(CXX) $< -o $(MODULE_PATH)$*.so -shared $(CXXFLAGS) $(LDLIBS) ${$(*)_LIBS}

# Target: Compile Raylib module
raylib-module: modules/raylib/raylib-module.cpp
	@echo "Building Raylib module..."
	cd raylib/src && make CUSTOM_CFLAGS=-fPIC
	$(CXX) $< -o $(MODULE_PATH)raylib.so -shared $(CXXFLAGS) $(LDLIBS) $(RAYLIB_LIBS) -L raylib/src -I raylib/src

# Target: Compile SDL module
sdl-module: modules/SDL/main.cpp
	@echo "Building SDL module..."
	$(CXX) $< -o $(MODULE_PATH)sdl.so -shared $(CXXFLAGS) $(LDLIBS) $(SDL_LIBS)

# Target: Build all modules
modules: $(addprefix module_,$(MODULES)) raylib-module sdl-module

# Target: Install
install: mix $(MODULE_PATH)
	@echo "Installing mix..."
	mkdir -p ~/.mix
	cp mix ~/.mix/

	# Check if the symlink already exists, and skip if it does
	if [ -L ~/bin/mix ]; then echo "Symlink already exists, skipping..."; else ln -s ~/.mix/mix ~/bin; fi

# Target: Create module directory
$(MODULE_PATH):
	mkdir -p $@

# Target: Clean
clean:
	rm -rf $(MODULE_PATH) mix
	cd raylib/src && make clean
