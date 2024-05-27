#pragma once
#ifndef MODULAR_HPP
#define MODULAR_HPP
#include <iostream>
#include <duktape.h>
#include <stdexcept>
#include "../../raylib/src/raylib.h"

typedef int (*duk_fn) (duk_context *ctx);

// Structure to represent a function in the module
typedef struct duk_func {
    const char *name;
    duk_fn func;
    const char* description;
} duk_func;

inline Color getColor(duk_context *ctx, int offset) {
  Color c = {0, 0, 0, 255}; // Initialize color to black with full opacity

  // Check if the argument at 'offset' is an object
  if (!duk_is_object(ctx, offset)) {
    // Handle error: Argument is not an object
    printf("Error: Argument at offset %d is not an object\n", offset);
    // print the type
    printf("Type: %d\n", duk_get_type(ctx, offset));
    return c;
  }

  // Get the 'r', 'g', 'b', and 'a' properties from the object
  duk_get_prop_string(ctx, offset, "r");
  duk_get_prop_string(ctx, offset, "g");
  duk_get_prop_string(ctx, offset, "b");
  duk_get_prop_string(ctx, offset, "a");

  // Check if all properties exist and are numbers
  if (duk_is_number(ctx, -4) && duk_is_number(ctx, -3) &&
      duk_is_number(ctx, -2) && duk_is_number(ctx, -1)) {
    // Extract RGBA values
    c.r = duk_get_int(ctx, -4);
    c.g = duk_get_int(ctx, -3);
    c.b = duk_get_int(ctx, -2);
    c.a = duk_get_int(ctx, -1);
  } else {
    // Handle error: Properties missing or not numbers
    printf("Error: RGBA properties missing or not numbers\n");
  }

  // Pop values from the stack
  duk_pop_n(ctx, 4);

  return c;
}
typedef int (*duk_fn) (duk_context *ctx);


// Function to register a module with its functions
inline void registerModule(duk_context *ctx, duk_func* functions) {

    // Register each function in the module object
    for (int i = 0; functions[i].name != NULL; ++i) {
        duk_push_c_function(ctx, functions[i].func, DUK_VARARGS);

        // Set a property on the function to store the function name (useful for error messages)
        duk_push_string(ctx, functions[i].name);
        duk_put_prop_string(ctx, -2, "__name");

        // Set a property on the function to store the function description (useful for error messages)
        duk_push_string(ctx, functions[i].description);
        duk_put_prop_string(ctx, -2, "__desc");

        duk_put_global_string(ctx, functions[i].name);
    }

    // Define the module with the specified name in the global object
}

inline void setObjectIntVal(duk_context *ctx, const char *name, int val) {
    duk_push_number(ctx, val);
    duk_put_global_string(ctx, name);
    
}
inline void setObjectStringVal(duk_context *ctx, const char *name, const char *val) {
    duk_push_string(ctx, val);
    duk_put_global_string(ctx, name);
}

#endif