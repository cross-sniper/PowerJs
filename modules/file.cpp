#include <cstdio>
#include <duktape.h>
#include <string>

std::string readfile(const char* fname) {
    FILE* f = fopen(fname, "rb"); // Open file in binary mode for better portability
    if (!f) {
        perror("Error opening file");
        return "";
    }

    fseek(f, 0, SEEK_END); // Seek to the end of file
    long size = ftell(f); // Get file size
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

duk_ret_t read(duk_context* ctx) {
    const char* name = duk_get_string(ctx, 0);

#ifdef EXP
    std::string v = readfile(name);
#else
    FILE* fp = fopen(name, "rb");
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

extern "C" duk_ret_t dukopen_file(duk_context* ctx) {
    duk_push_object(ctx); // Create a new object

    duk_push_c_function(ctx, read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_put_global_string(ctx, "file"); // Assign the object to a global variable "file"
    return 0; // Return 0 to indicate success
}
