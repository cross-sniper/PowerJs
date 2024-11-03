#include "console.hpp"
#include "extras.cpp"
#include "m-loader.hpp"
#include "modules/core.cpp"
#include "modules/fetch.cpp"
#include "modules/file.cpp"
#include "modules/os.cpp"
#include "modules/raylib.cpp"
#include "modules/server.cpp"
#include "std.cpp"

#include <cstring>
#include <dlfcn.h> // For dynamic loading of shared object files
#include <duk_config.h>
#include <duktape.h>
#include <fstream>
#include <iostream>
#include <string>

std::string read(const char *name) {
  std::string str;
  std::ifstream file(name); // Open the file
  if (!file.is_open()) {
    std::cerr << "Error opening file: " << name << std::endl;
    exit(1);
  }

  std::string line;
  bool firstLine = true;
  while (std::getline(file, line)) { // Read file line by line
    // Check if it's the first line and if it starts with "#!"
    if (firstLine) {
      firstLine = false;
      if (line.rfind("#!", 0) == 0) {
        continue; // Skip this line
      }
    }
    // Append the line to the string
    str += line;
    str += "\n"; // Add newline character since std::getline consumes it
  }

  file.close(); // Close the file
  return str;
}

// Define a typedef for the function signature of the Duktape module open
// function
typedef void (*duk_module_open_function)(duk_context *);

static duk_ret_t quit(duk_context *ctx) {
  int exit_code = 0;
  if (duk_get_top(ctx) == 1) {
    exit_code = duk_get_number(ctx, 1);
  }
  exit(exit_code);
  return 0;
}

int sys(duk_context *context) {
  const char *cmd = duk_to_string(context, 0);
  system(cmd);
  return 0;
}

void initpowerjs(duk_context *context, int argc, const char *argv[]) {
  // Push argv as a variable
  duk_push_object(context);
  duk_push_array(context);
  for (int i = 0; i < argc; ++i) {
    duk_push_string(context, argv[i]);
    duk_put_prop_index(context, -2, i);
  }
  duk_put_prop_string(context, -2, "args");

  duk_put_global_string(context, "powerjs");
  init_std(context);

  duk_push_c_function(context, quit, DUK_VARARGS);
  duk_put_global_string(context, "quit");

  initExtras(context);
  dukopen_core(context);
  dukopen_raylib(context);
  dukopen_file(context);
  dukopen_os(context);
  dukopen_server(context);
  module::init(context);
  Console::init(context);
  duk_push_c_function(context, sys, 1);
  duk_put_global_string(context, "system");
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    printf("pass a file, %s <file>\n", argv[0]);
    exit(1);
  }
  int EXIT_CODE = 0;
  duk_context *context = duk_create_heap_default();
  if (!context) {
    std::cerr << "Error creating Duktape context." << std::endl;
    return 1;
  }
  initpowerjs(context, argc, argv);
  // Read the content of the main JavaScript file
  if ((strcmp(argv[1], ".") == 0) || (strcmp(argv[1], "./") == 0) ||
      (strcmp(argv[1], "..") == 0)) {
    argv[1] = "main.js";
  }
  std::string scriptContent = read(argv[1]);

  // Evaluate the main script
  int res = duk_peval_string(context, scriptContent.c_str());
  if (res != 0) {
    // Handle error
    std::cerr << "Error evaluating JavaScript: "
              << duk_safe_to_string(context, -1) << std::endl;
    EXIT_CODE = 1;
  } else {
    // Evaluate the main script
    res = duk_peval_string(context, "main(powerjs.args.length, powerjs.args)");
    if (res != 0) {
      // Handle error
      std::cerr << "Error evaluating JavaScript: "
                << duk_safe_to_string(context, -1) << std::endl;
      EXIT_CODE = 1;
    }
  }

  // Clean up
  duk_destroy_heap(context);
  return EXIT_CODE;
}
