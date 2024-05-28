#include <duk_config.h>
#include <duktape.h>

// safeEval implementation
duk_ret_t safeEval(duk_context *ctx) {
    // Create a new heap/context for safe evaluation
    duk_context *safeEnv = duk_create_heap_default();
    if (!safeEnv) {
        return DUK_RET_ERROR;
    }

    // Get the string to evaluate from the first argument
    const char *script = duk_get_string(ctx, 0);
    if (!script) {
        duk_destroy_heap(safeEnv);
        return DUK_RET_TYPE_ERROR;
    }

    // Get the object with properties to expose from the second argument
    if (!duk_is_object(ctx, 1)) {
        duk_destroy_heap(safeEnv);
        return DUK_RET_TYPE_ERROR;
    }

    // Push the global object in the safe environment
    duk_push_global_object(safeEnv);

    // Transfer properties from the provided object to the global object
    duk_enum(ctx, 1, DUK_ENUM_OWN_PROPERTIES_ONLY);
    while (duk_next(ctx, -1, 1)) {
        // [key, value] on main ctx
        duk_dup(ctx, -2); // key
        duk_dup(ctx, -2); // value
        duk_put_prop(safeEnv, -3); // global[key] = value in safeEnv
        duk_pop_2(ctx); // pop key and value from main ctx
    }
    duk_pop(ctx); // pop enumerator

    // Evaluate the script in the safe environment
    duk_push_string(safeEnv, script);
    if (duk_peval(safeEnv) != 0) {
        // If there is an error, copy the error to the main context
        duk_push_error_object(ctx, DUK_ERR_ERROR, "Error: %s", duk_safe_to_string(safeEnv, -1));
        duk_destroy_heap(safeEnv);
        return DUK_RET_ERROR;
    }

    // Copy the result back to the main context
    duk_xmove_top(ctx, safeEnv, 1);

    // Clean up
    duk_destroy_heap(safeEnv);

    return 1; // One return value
}

// Register the core module and the safeEval function
extern "C" void dukopen_core(duk_context *ctx) {
	duk_push_c_function(ctx, safeEval, 2);
	duk_put_global_string(ctx, "safeEval");
}
