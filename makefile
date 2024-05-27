# Define compiler and flags
CXX = g++
CXXFLAGS = -g -fPIC
LDLIBS = -lduktape
MODULE_PATH = ~/.powerjs/libs/
INSTALL_DIR = ~/.powerjs/
BIN_DIR = ~/bin

# Modules
MODULES = fetch file core example server os

RAYLIB_LIBS = -lraylib
power:
	g++ main.cpp -o powerjs -lduktape $(CXXFLAGS) $(LDLIBS) $(RAYLIB_LIBS)

# Target: Compile a specific module
module_%: modules/%.cpp
	@echo "Compiling $<..."
	$(CXX) $< -o $(MODULE_PATH)$*.so -shared $(CXXFLAGS) $(LDLIBS) ${$(*)_LIBS}


# Target: Build all modules
modules: power $(addprefix module_,$(MODULES))

# Target: Install
install: powerjs $(MODULE_PATH)
	@echo "Installing powerjs..."
	mkdir -p  $(INSTALL_DIR)
	cp powerjs $(INSTALL_DIR)

	# Check if the symlink already exists, and skip if it does
	if [ -L ~/bin/powerjs ]; then echo "Symlink already exists, skipping..."; else ln -s ~/.powerjs/powerjs ~/bin; fi

# Target: Create module directory
$(MODULE_PATH):
	mkdir -p $@

# Target: Clean
clean:
	rm -rf $(MODULE_PATH) powerjs

