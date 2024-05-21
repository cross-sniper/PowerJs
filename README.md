# PowerJs engine
formaly: mix js

# Attention
powerjs currently does not work on windows,
reason: we currently use unix-like file paths for some things(like module loading),
so until we can make it work with windows' file paths,
use it under wsl, or on a unix-like system

## What is powerjs?
powerjs is a simple JavaScript engine built with Duktape,
designed primarily for game development.
It provides a module API for extending its functionality.

# Building powerjs
to build powerjs, git clone this repo, then cd into it,
afterwords, make sure you have duktape(and any lib needed by the modules(see makefile)),
and run `make`.

If all went well,
you should have a `powerjs` binary on the root folder of the project
simply do `./powerjs <file>.js` to run it

# Module API

## how it works
the module api has two ways of functioning, per-project, or system-wide

### per-project
to make the api work per project, the shared library you are trying to call needs to be on the same directory you ran powerjs from

### system-wide
you need to make sure the module is not in the same folder you ran powerjs from, and is within `~/.powerjs/modules`, powerjs will handle the rest by itself


To define a module in C++, you can follow this pattern:

```cpp
#include "../modular.hpp"
static int example(duk_context* ctx){
	std::cout << "this is a example module" << std::endl;
	return 0;
}

duk_func fns[] = {
	{"example",example,"example module"},
	{NULL, NULL, "NULL"}
};
extern "C" duk_ret_t dukopen_example(duk_context* ctx)
{
	registerModule(ctx, fns);
	setGlobalModule(ctx, "example");
	return 1;
}

```

And here's how you would use the "example" module in JavaScript:

```javascript
require("example")
example.example()
```

This will output `"this is a example module"` to the console.


# included modules
there are 3(as of 22:28 29-03-2024)

- example:
  - the example included in this document

- fetch:
  - a simple implementation of JS's fetch api, using curl
- file:
  - a file handling api, with `open`, `read`, `close`, `write`

