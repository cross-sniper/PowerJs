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
/*on ducktape.h
#define DUK_TYPE_MIN                      0U
#define DUK_TYPE_NONE                     0U
#define DUK_TYPE_UNDEFINED                1U
#define DUK_TYPE_NULL                     2U
#define DUK_TYPE_BOOLEAN                  3U
#define DUK_TYPE_NUMBER                   4U
#define DUK_TYPE_STRING                   5U
#define DUK_TYPE_OBJECT                   6U
#define DUK_TYPE_BUFFER                   7U
#define DUK_TYPE_POINTER                  8U
#define DUK_TYPE_LIGHTFUNC                9U
#define DUK_TYPE_MAX                      9U

*/
    
	duk_ret_t type(duk_context *ctx) {
	    // Get the type of the first argument passed to the function
	    auto type_id = duk_get_type(ctx, 0);

	    // Map the type ID to a string representation
	    const char *type_name = "";
	    switch (type_id) {
	        case DUK_TYPE_NONE:       type_name = "none"; break;
	        case DUK_TYPE_UNDEFINED:  type_name = "undefined"; break;
	        case DUK_TYPE_NULL:       type_name = "null"; break;
	        case DUK_TYPE_BOOLEAN:    type_name = "boolean"; break;
	        case DUK_TYPE_NUMBER:     type_name = "number"; break;
	        case DUK_TYPE_STRING:     type_name = "string"; break;
	        case DUK_TYPE_OBJECT:     type_name = "object"; break;
	        case DUK_TYPE_BUFFER:     type_name = "buffer"; break;
	        case DUK_TYPE_POINTER:    type_name = "pointer"; break;
	        case DUK_TYPE_LIGHTFUNC:  type_name = "lightfunc"; break;
	        default:                  type_name = "unknown"; break;
	    }

	    // Push the type name onto the stack
	    duk_push_string(ctx, type_name);

	    // Return the result (the type name)
	    return 1;
	}

};


void init_std(duk_context *context) {
    // Push argv as a variable
    duk_push_object(context);

    duk_push_c_function(context, mix::Atoi, 1);
    duk_put_prop_string(context, -2, "atoi");

    duk_push_c_function(context, mix::To_string, 1);
    duk_put_prop_string(context, -2, "to_string");

    duk_push_c_function(context, mix::type,1);
    duk_put_prop_string(context, -2 ,"typeof");
    duk_put_global_string(context, "std");
}
