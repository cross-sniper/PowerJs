#include <cstdio>
#include <dirent.h>
#include <duktape.h>
#include <string>
#include <sys/types.h>
#include <vector>

bool exists(const char *name) {
  FILE *f = fopen(name, "rb");
  if (!f) {
    return false;
  }
  fclose(f);
  return true;
}

std::string readfile(const char *fname) {
  FILE *f =
      fopen(fname, "rb"); // Open file in binary mode for better portability
  if (!f) {
    perror("Error opening file");
    return "";
  }

  fseek(f, 0, SEEK_END); // Seek to the end of file
  long size = ftell(f);  // Get file size
  fseek(f, 0, SEEK_SET); // Seek back to the beginning

  std::string buffer;
  buffer.resize(size); // Resize buffer to fit file contents
  size_t bytesRead = fread(&buffer[0], 1, size, f); // Read file into buffer
  fclose(f);

  if (bytesRead != size) {
    perror("Error reading file");
    return "";
  }

  return buffer;
}

duk_ret_t read(duk_context *ctx) {
  const char *name = duk_get_string(ctx, 0);

#ifdef EXP
  std::string v = readfile(name);
#else
  FILE *fp = fopen(name, "rb");
  if (!fp) {
    perror("Error opening file");
    return 0;
  }

  std::string v;
  char buf[1024];
  size_t bytesRead;
  while ((bytesRead = fread(buf, 1, sizeof(buf), fp)) > 0) {
    v.append(buf, bytesRead);
  }

  fclose(fp);
#endif

  duk_push_lstring(ctx, v.c_str(), v.length());
  return 1;
}

duk_ret_t listDir(duk_context *ctx) {
  const char *name = duk_get_string(ctx, 0);

  DIR *dir;
  struct dirent *ent;

  if ((dir = opendir(name)) != nullptr) {
    std::vector<std::string> files;
    while ((ent = readdir(dir)) != nullptr) {
      files.push_back(ent->d_name);
    }
    closedir(dir);

    // Push the directory listing onto the Duktape stack as an array
    duk_idx_t arr_idx = duk_push_array(ctx);
    for (size_t i = 0; i < files.size(); ++i) {
      duk_push_string(ctx, files[i].c_str());
      duk_put_prop_index(ctx, arr_idx, static_cast<duk_uarridx_t>(i));
    }

    return 1;
  } else {
    // Failed to open directory
    perror("Error opening directory");
    return 0;
  }
}

duk_ret_t write(duk_context *ctx) {
  const char *name = duk_get_string(ctx, 0);
  const char *data = duk_get_string(ctx, 1);
  FILE *fp = fopen(name, "wb");
  if (!fp) {
    perror("Error opening file");
    return 0;
  }

  size_t data_length = strlen(data);
  size_t bytes_written = fwrite(data, 1, data_length, fp);
  if (bytes_written != data_length) {
    perror("Error writing to file");
    fclose(fp);
    return 0;
  }
  fclose(fp);
  return 0;
}

duk_ret_t existsOk(duk_context *ctx) {
  const char *name = duk_get_string(ctx, 0);
  bool v = exists(name);
  duk_push_boolean(ctx, v);
  return 1;
}

extern "C" duk_ret_t dukopen_file(duk_context *ctx) {
  duk_push_object(ctx); // Create a new object

  duk_push_c_function(ctx, read, 1);
  duk_put_prop_string(ctx, -2, "read");

  duk_push_c_function(ctx, existsOk, 1);
  duk_put_prop_string(ctx, -2, "exists");

  duk_push_c_function(ctx, listDir, 1);
  duk_put_prop_string(ctx, -2, "listDir");

  duk_push_c_function(ctx, write, 2);
  duk_put_prop_string(ctx, -2, "write");

  duk_put_global_string(
      ctx, "file"); // Assign the object to a global variable "file"
  return 0;         // Return 0 to indicate success
}
