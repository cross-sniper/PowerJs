#pragma once
#include <cstdio>
#include <cstdlib>
#include <duktape.h>
// #include "fns.hpp"

inline char *readFile(const char *name) {
  FILE *file = fopen(name, "r");
  if (!file) {
    fprintf(stderr, "Failed to open file %s\n", name);
    exit(1);
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buff = (char *)malloc(size + 1);
  if (!buff) {
    fprintf(stderr, "Failed to allocate memory.\n");
    fclose(file);
    exit(1);
  }

  size_t bytesRead = fread(buff, 1, size, file);
  if (bytesRead != size) {
    fprintf(stderr, "Failed to read the complete file.\n");
    free(buff);
    fclose(file);
    exit(1);
  }
  buff[size] = '\0'; // Null-terminate the string
  fclose(file);
  return buff;
}

inline void handleError(int code, duk_context *ctx) {
  if (code != 0) {
    fprintf(stderr, "There was an error running the file: %s\n",
            duk_to_string(ctx, -1));
    exit(code);
  }
}
