#pragma once
#ifndef RAY_HELPER_HPP
#define RAY_HELPER_HPP
#include <iostream>
#include <duktape.h>
#include <stdexcept>
#include <raylib.h>

inline Color getColor(duk_context *ctx, int offset) {
  Color c = {0, 0, 0, 255}; // Initialize color to black with full opacity

  // Check if the argument at 'offset' is an object
  if (!duk_is_object(ctx, offset)) {
    // Handle error: Argument is not an object
    printf("Error: Argument at offset %d is not an object\n", offset);
    // print the type
    printf("Type: %d\n", duk_get_type(ctx, offset));
    exit(1);
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
    exit(1);
  }

  // Pop values from the stack
  duk_pop_n(ctx, 4);

  return c;
}

inline void setGlobalIntVal(duk_context *ctx, const char* name, int val){
  duk_push_number(ctx, val);
  duk_put_global_string(ctx, name);
}

#endif
