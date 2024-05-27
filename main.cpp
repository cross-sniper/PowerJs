#include "std.cpp"
#include "extras.cpp"
#include "modules/raylib.cpp"
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
static duk_ret_t print(duk_context *ctx) {
  int nargs = duk_get_top(ctx); // Get the number of arguments

  for (int i = 0; i < nargs; ++i) {
    switch (duk_get_type(ctx, i)) {
    case DUK_TYPE_NONE:
      std::cout << "none";
      break;
    case DUK_TYPE_UNDEFINED:
      std::cout << "undefined";
      break;
    case DUK_TYPE_NULL:
      std::cout << "null";
      break;
    case DUK_TYPE_BOOLEAN:
      std::cout << (duk_to_boolean(ctx, i) ? "true" : "false");
      break;
    case DUK_TYPE_NUMBER:
      std::cout << duk_to_number(ctx, i);
      break;
    case DUK_TYPE_STRING:
      std::cout << duk_safe_to_string(ctx, i);
      break;
    case DUK_TYPE_OBJECT: {
      duk_enum(ctx, i,
               DUK_ENUM_OWN_PROPERTIES_ONLY); // Enumerate own properties
      std::cout << "[\n  ";
      int count = 0;
      while (duk_next(ctx, -1, 1)) {
        if (count > 0) {
          std::cout << ", "; // Add comma if not the first property
        }
        // TODO: print them in green, like on node
        std::cout << "\033[1;32m\"" << duk_safe_to_string(ctx, -2) << "\": \""
                  << duk_safe_to_string(ctx, -1) << "\"\033[0m";
        duk_pop_2(ctx); // Pop key and value
        count++;
      }
      std::cout << "\n]";
      duk_pop(ctx); // Pop the enumerator
      break;
    }
    case DUK_TYPE_BUFFER:
      std::cout << "[buffer]";
      break;
    case DUK_TYPE_POINTER:
      std::cout << "[pointer]";
      break;
    case DUK_TYPE_LIGHTFUNC:
      std::cout << "[lightfunc]";
      break;
    default:
      std::cout << "[unknown]";
      break;
    }
    if (i < nargs - 1) {
      std::cout << " "; // Separate arguments with space
    }
  }

  std::cout << std::endl; // Print a newline
  return 0;
}

static duk_ret_t input(duk_context *ctx) {
  const char *prompt = duk_get_string(ctx, 0);
  std::string in;
  std::cout << prompt << std::flush; // Print the prompt

  // Check if there's input available
  if (std::cin.peek() == '\n') {
    std::cin.ignore(); // Ignore the newline character
  } else {
    std::getline(std::cin, in); // Read the input
  }

  duk_push_string(ctx, in.c_str()); // Push input string to the Duktape stack
  return 1;
}

static duk_ret_t quit(duk_context *ctx) {
  int exit_code = 0;
  if (duk_get_top(ctx) == 1) {
    exit_code = duk_get_number(ctx, 1);
  }
  exit(exit_code);
  return 0;
}

static duk_ret_t require(duk_context *ctx) {
  // load a file and execute it
  const char *module_name = duk_require_string(ctx, 0);
  std::string code = read(module_name);
  int res = duk_peval_string(ctx, code.c_str());
  if (res != 0) {
    std::cerr << "Error evaluating JavaScript: " << duk_safe_to_string(ctx, -1)
              << std::endl;
    std::cerr << "module:" << module_name << std::endl;
    exit(1);
  }
  return 1;
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
  dukopen_raylib(context);

  duk_push_c_function(context, require, 1);
  duk_put_global_string(context, "require");
  duk_push_c_function(context, input, 1);
  duk_put_global_string(context, "input");
  // Register the print function
  duk_push_c_function(context, print, 1);
  duk_put_global_string(context, "print");
  duk_push_c_function(context, quit, DUK_VARARGS);
  duk_put_global_string(context, "quit");

  initExtras(context);
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
  }
  else{
    // Evaluate the main script
    res = duk_peval_string(context,"main(powerjs.args.length, powerjs.args)");
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
