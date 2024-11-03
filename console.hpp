#pragma once
#include <duk_config.h>
#include <duktape.h>
#include <iostream>
#include <regex>
#include <stdio.h>
#include <string>

namespace Console {
inline int Write(duk_context *ctx) {
  // Ensure there’s at least one argument passed
  if (duk_get_top(ctx) < 1) {
    printf("console.write: No arguments provided.\n");
    return 0;
  }

  // Print the argument as a string
  printf("%s\n", duk_to_string(ctx, 0));
  return 0;
}

inline int Read(duk_context *ctx) {
  std::string input;
  std::getline(std::cin, input);
  duk_push_string(ctx, input.c_str());
  return 1; // Return the input string
}

inline int Readf(duk_context *ctx) {
  if (duk_get_top(ctx) < 1) {
    printf("console.readf: No format string provided.\n");
    return 0;
  }

  const char *formatStr = duk_to_string(ctx, 0);
  std::string input;
  std::getline(std::cin, input);

  // Here, handle parsing based on `formatStr` if you have specific parsing
  // needs. This is a basic example where we just push the raw input.
  duk_push_string(ctx, input.c_str());
  return 1; // Return the formatted input
}

inline int Writef(duk_context *ctx) {
  if (duk_get_top(ctx) < 1) {
    printf("console.writef: No format string provided.\n");
    return 0;
  }

  const char *formatStr = duk_to_string(ctx, 0);
  std::string result(formatStr);
  std::regex exprPattern(R"(\{([^}]+)\})");
  std::smatch match;

  std::string evaluatedStr;
  auto start = result.cbegin();

  while (std::regex_search(start, result.cend(), match, exprPattern)) {
    evaluatedStr.append(start,
                        match[0].first); // Append text before `{expression}`

    std::string expression = match[1].str();
    duk_push_string(ctx, expression.c_str());

    if (duk_peval(ctx) != 0) {
      // If evaluation fails, insert error message
      evaluatedStr.append("[Error]");
    } else {
      evaluatedStr.append(duk_safe_to_string(ctx, -1));
    }
    duk_pop(ctx); // Clean up the stack

    start = match[0].second; // Move past the current match
  }

  evaluatedStr.append(start, result.cend()); // Append the rest of the string
  printf("%s\n", evaluatedStr.c_str());

  return 0;
}

inline void init(duk_context *ctx) {
  duk_push_object(ctx);

  duk_push_c_function(ctx, Console::Write, 1);
  duk_put_prop_string(ctx, -2, "write");

  duk_push_c_function(ctx, Console::Writef, 1);
  duk_put_prop_string(ctx, -2, "writef");

  duk_push_c_function(ctx, Console::Read, 0);
  duk_put_prop_string(ctx, -2, "read");

  duk_push_c_function(ctx, Console::Readf, 1);
  duk_put_prop_string(ctx, -2, "readf");

  duk_put_global_string(ctx, "console");
}
} // namespace Console
