# mix js engine

## What is mix?
Mix is a simple JavaScript engine built with Duktape, designed primarily for game development. It provides a module API for extending its functionality.

To define a module in C++, you can follow this pattern:

```cpp
#include "modular.hpp"

static int example(duk_context* ctx){
  std::cout << "this is a example module" << std::endl;
  return 0;
}

duk_func fns[] = {
  {"example",example},
  {NULL, NULL}
};

extern "C" duk_ret_t dukopen_example(duk_context* ctx)
{
  registerModule(ctx, "example", fns);
  return 1;
}
```

And here's how you would use the "example" module in JavaScript:

```javascript
require("example")
example.example()
```

This will output `"this is a example module"` to the console.
