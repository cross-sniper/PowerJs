#include <iostream>
#include <duktape.h>
typedef int (*duk_fn) (duk_context *ctx);

// Structure to represent a function in the module
typedef struct duk_func{
    const char *name;
    duk_fn func;
} duk_func;

// Function to register a module with its functions
inline void registerModule(duk_context *ctx, const char* name, duk_func* functions) {
    // Create a new object to represent the module
    duk_push_object(ctx);

    // Register each function in the module object
    for (int i = 0; functions[i].name != NULL; ++i) {
        duk_push_c_function(ctx, functions[i].func, DUK_VARARGS);
        duk_put_prop_string(ctx, -2, functions[i].name);
    }

    // Define the module with the specified name in the global object
    duk_put_global_string(ctx, name);
}
