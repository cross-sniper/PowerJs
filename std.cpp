#include <duk_config.h>
#include <duktape.h>
#include <string>
#include <iostream>

// mix's std lib
namespace mix {
    duk_ret_t Atoi(duk_context *context) {
        const char *str = duk_get_string(context, 0);
        if (!str) {
            std::cerr << "Invalid input string" << std::endl;
            return 0;
        }
        int n = atoi(str);
        duk_push_number(context, n);
        return 1;
    }

    duk_ret_t To_string(duk_context *context) {
        double num = duk_get_number(context, 0);
        std::string s = std::to_string(num);
        duk_push_string(context, s.c_str());
        return 1;
    }
};
duk_ret_t type(duk_context *ctx) {
    // Get the type of the first argument passed to the function
    duk_idx_t idx = 1; // Index of the first argument
    const char *type_name = duk_safe_to_string(ctx, duk_get_type(ctx, idx));
    
    // Push the type name onto the stack
    duk_push_string(ctx, type_name);

    // Return the result (the type name)
    return 1;
}


void init_std(duk_context *context) {
    // Push argv as a variable
    duk_push_object(context);

    duk_push_c_function(context, mix::Atoi, 1);
    duk_put_prop_string(context, -2, "atoi");

    duk_push_c_function(context, mix::To_string, 1);
    duk_put_prop_string(context, -2, "to_string");

    duk_push_c_function(context, type,1);
    duk_put_prop_string(context, -2 ,"typeof");
    duk_put_global_string(context, "std");
}
