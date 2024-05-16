#include <duktape.h>

duk_ret_t exec(duk_context *ctx){
	const char* string = duk_get_string(ctx, 0);
	int res = system(string);
	duk_push_int(ctx, res);
	return 1;
}

extern "C" void dukopen_os(duk_context *ctx){
	duk_push_object(ctx);
	duk_push_c_function(ctx, exec, 1);
	duk_put_prop_string(ctx, -2, "exec");
	duk_put_global_string(ctx, "os");
}