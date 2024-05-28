# Define compiler and flags
CXX = g++
CXXFLAGS = -g -fPIC
LDLIBS = -lduktape -lraylib -lcurl -lm -lpthread
INSTALL_DIR = ~/.powerjs/
BIN_DIR = ~/bin

power: main.cpp
	g++ main.cpp -o powerjs $(CXXFLAGS) $(LDLIBS) $(RAYLIB_LIBS)


# Target: Install
install: power $(INSTALL_DIR)
	@echo "Installing powerjs..."
	mkdir -p  $(INSTALL_DIR)
	cp powerjs $(INSTALL_DIR)

	# Check if the symlink already exists, and skip if it does
	if [ -L ~/bin/powerjs ]; then echo "Symlink already exists, skipping..."; else ln -s ~/.powerjs/powerjs ~/bin; fi

# Target: Create module directory
$(INSTALL_DIR):
	mkdir -p $@

# Target: Clean
clean:
	rm -rf $(INSTALL_DIR) powerjs

