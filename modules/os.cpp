#include <duktape.h>

duk_ret_t exec(duk_context *ctx){
	const char* string = duk_get_string(ctx, 0);
	int res = system(string);
	duk_push_int(ctx, res);
	return 1;
}

extern "C" void dukopen_os(duk_context *ctx){
	duk_push_object(ctx);

	// adds exec, and some simple help text
	// don't know how, but this works, nor do i care to figure it out
	duk_push_object(ctx);
	duk_push_c_function(ctx, exec, 1);
	duk_push_string(ctx, "executes a command");
	duk_put_prop_string(ctx, -2, "_explain");

	// this sets the whole object to be the exec function, while also adding exec._explain
	// yes, its some weird sourcery, but it works
	duk_put_prop_string(ctx, -2, "exec");


	// add ._explain
	duk_put_global_string(ctx, "os");
}
