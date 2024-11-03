#pragma once

#include "fns.hpp"
#include <dlfcn.h>
#include <duktape.h>
#include <stdio.h>
#include <stdlib.h> // Include for exit()

namespace module {
// TODO: add more functions to module
inline int load(duk_context *ctx) {
  char *buff = readFile(duk_to_string(ctx, 0));
  if (!buff) {
    // Handle the case where reading the file failed
    printf("Error reading file\n");
    exit(1);
  }

  int r = duk_peval_string(ctx, buff);
  free(buff);
  handleError(r, ctx);
  return 0;
}

int loadNative(duk_context *ctx) {
  const char *name = duk_to_string(ctx, 0);
  void *mod = dlopen(name, RTLD_LAZY);
  if (!mod) { // Check if dlopen failed
    printf("Could not load native module: %s\n", dlerror());
    exit(1);
  }

  // Attempt to get the init function
  int (*init)(duk_context *) = (int (*)(duk_context *))dlsym(mod, "init");
  if (!init) { // Check if dlsym failed
    printf("Could not find init function: %s\n", dlerror());
    dlclose(mod);
    exit(1);
  }

  // by having the init function return a int, we allow you to do "var math =
  // loadNative('math.so')"
  int r = (int)init(ctx);
  dlclose(mod); // Close the module after use
  return r;
}

inline void init(duk_context *ctx) {
  duk_push_c_function(ctx, load, 1);
  duk_put_global_string(ctx, "load");

  duk_push_c_function(ctx, loadNative, 1);
  duk_put_global_string(ctx, "loadNative");
}

} // namespace module
