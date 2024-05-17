extra_flags=
module_path = ~/.mix/libs/
mix:main.cpp std.cpp
	mkdir -p ${module_path}
	g++ main.cpp -o mix -lduktape ${extra_flags}
py:
	g++ main.cpp -o mix.so `pkg-config --cflags --libs python-3.12-embed` -lduktape ${extra_flags} -fPIC -shared -DPY_MOD

module:
	if [ ! -d $(module_path) ]; then mkdir -p $(module_path); fi
	g++ modules/fetch.cpp -o $(module_path)fetch.so -shared -lduktape -lcurl -fPIC ${extra_flags}
	g++ modules/file.cpp -o $(module_path)file.so -shared -lduktape -lcurl -fPIC ${extra_flags}
	g++ modules/core.cpp -o $(module_path)core.so -shared -lduktape -fPIC ${extra_flags}
	g++ modules/example.cpp -o $(module_path)example.so -shared -lduktape -fPIC ${extra_flags}
	g++ modules/server.cpp -o $(module_path)server.so -lduktape -shared -fPIC

raylib-module:mix modules/raylib/raylib-module.cpp
	echo "this is still beeing worked on"
	cd raylib/src && make CUSTOM_CFLAGS=-fPIC
	g++ modules/raylib/raylib-module.cpp -o $(module_path)raylib.so -shared -lduktape -lraylib -fPIC -L raylib/src -I raylib/src ${extra_flags}

sdl-module:modules/SDL/*
	g++ modules/SDL/main.cpp -o $(module_path)sdl.so -shared -lduktape -lSDL2

all:mix modules raylib-module sdl-module


install:all ~/bin
	mkdir -p ~/.mix
	cp mix ~/.mix/

	# Check if the symlink already exists, and skip if it does
	if [ -L ~/bin/mix ]; then echo "Symlink already exists, skipping...";else ln -s ~/.mix/mix ~/bin ;fi
