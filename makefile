extra_flags=
mix:main.cpp std.cpp
	g++ main.cpp -o mix -lduktape ${extra_flags}

modules:mix modular.hpp
	g++ modules/fetch.cpp -o ~/.mix/modules/fetch.so -shared -lduktape -lcurl -fPIC ${extra_flags}
	g++ modules/file.cpp -o ~/.mix/modules/file.so -shared -lduktape -lcurl -fPIC ${extra_flags}
	g++ modules/core.cpp -o ~/.mix/modules/core.so -shared -lduktape -fPIC ${extra_flags}
	g++ modules/example.cpp -o ~/.mix/modules/example.so -shared -lduktape -fPIC ${extra_flags}

raylib-module:mix modules/raylib-module.cpp
	echo "this is still beeing worked on"
	cd raylib/src && make CUSTOM_CFLAGS=-fPIC
	g++ modules/raylib-module.cpp -o ~/.mix/modules/raylib.so -shared -lduktape -lraylib -fPIC -L raylib/src -I raylib/src ${extra_flags}
sdl-module:modules/SDL/*
	g++ modules/SDL/main.cpp -o ~/.mix/modules/sdl.so -shared -lduktape -lSDL2

all:mix modules raylib-module sdl-module

install:all ~/bin
	mkdir -p ~/.mix
	cp mix ~/.mix/

	# Check if the symlink already exists, and skip if it does
	if [ -L ~/bin/mix ]; then ln -s ~/.mix/mix ~/bin;else echo "Symlink already exists, skipping...";fi
