mix:main.cpp std.cpp
	g++ main.cpp -o mix -lduktape

modules:mix modular.hpp
	g++ modules/fetch.cpp -o ~/.mix/modules/fetch.so -shared -lduktape -lcurl -fPIC
	g++ modules/file.cpp -o ~/.mix/modules/file.so -shared -lduktape -lcurl -fPIC
	g++ modules/example.cpp -o ~/.mix/modules/example.so -shared -lduktape -fPIC

raylib-module:mix modules/raylib-module.cpp
	echo "this is still beeing worked on"
	cd raylib/src && make CUSTOM_CFLAGS=-fPIC
	g++ modules/raylib-module.cpp -o ~/.mix/modules/raylib.so -shared -lduktape -lraylib -fPIC -L raylib/src -I raylib/src

all:mix modules raylib-module

install:all ~/bin
	cp mix ~/bin