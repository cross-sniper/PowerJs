#include <cstdio>
#include <duktape.h>

// Struct to encapsulate file pointer
struct File {
    FILE* ptr;
};

duk_ret_t open(duk_context *ctx) {
    const char* filename = duk_get_string(ctx, 0);
    const char* mode = duk_get_string(ctx, 1);
    FILE* f = fopen(filename, mode);
    if (!f) {
        // If opening the file fails, return null
        duk_push_null(ctx);
        return 1;
    }

    // Allocate memory for File struct
    File* file = (File*)duk_push_fixed_buffer(ctx, sizeof(File));
    file->ptr = f;

    return 1;
}

// Function to read data from a file
duk_ret_t read(duk_context *ctx) {
    File* file = (File*)duk_require_buffer(ctx, 0, NULL);
    if (!file || !file->ptr) {
        // If file pointer is null, return null
        duk_push_null(ctx);
        return 1;
    }

    // Read file contents
    fseek(file->ptr, 0, SEEK_END);
    size_t length = ftell(file->ptr);
    rewind(file->ptr);

    char* buffer = (char*)malloc(length + 1); // Allocate memory for buffer
    if (!buffer) {
        fclose(file->ptr);
        duk_push_null(ctx);
        return 1;
    }

    size_t bytesRead = fread(buffer, 1, length, file->ptr);
    buffer[bytesRead] = '\0'; // Null terminate string


    duk_push_lstring(ctx, buffer, bytesRead);
    free(buffer);

    return 1;
}

// Function to write data to a file
duk_ret_t write(duk_context *ctx) {
    File* file = (File*)duk_require_buffer(ctx, 0, NULL);
    const char* data = duk_get_string(ctx, 1);
    if (!file || !file->ptr) {
        // If file pointer is null, return false
        duk_push_false(ctx);
        return 1;
    }

    size_t bytesWritten = fwrite(data, 1, strlen(data), file->ptr);


    if (bytesWritten != strlen(data)) {
        duk_push_false(ctx);
        return 1;
    }

    duk_push_true(ctx);
    return 1;
}

duk_ret_t close(duk_context *ctx) {
    File* file = (File*)duk_require_buffer(ctx, 0, NULL);
    if (!file || !file->ptr) {
        return 0; // Nothing to close
    }
    fclose(file->ptr);
    return 0;
}

extern "C" duk_ret_t dukopen_file(duk_context *ctx) {
    duk_push_object(ctx); // Create a new object

    duk_push_c_function(ctx, open, 2);
    duk_put_prop_string(ctx, -2, "open");

    duk_push_c_function(ctx, read, 1);
    duk_put_prop_string(ctx, -2, "read");

    duk_push_c_function(ctx, write, 2);
    duk_put_prop_string(ctx, -2, "write");

    duk_push_c_function(ctx, close, 1);
    duk_put_prop_string(ctx, -2, "close");

    duk_put_global_string(ctx, "file"); // Assign the object to a global variable "file"
    return 0; // Return 0 to indicate success
}
