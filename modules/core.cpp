#include <duk_config.h>
#include <duktape.h>

#ifdef EXP
static duk_ret_t eval(duk_context *ctx){
	const char* string = duk_to_string(ctx, 0);
	if(duk_peval_string(ctx, string) != 0)
	{
		printf("eval err: \n%s\n", duk_to_string(ctx, -1));
		exit(1);
	}
	return 0;
}



extern "C" void dukopen_core(duk_context*ctx){
	duk_push_c_function(ctx,eval,1);
	duk_put_global_string(ctx,"eval");
}
#else
extern "C" void dukopen_core(duk_context* ctx){
	printf("core: this module was not built with -DEXP, thus it may not work\n");
	printf("core is considered experimental, since it aims to bring \"unsafe\" functions, like eval, into mix\n");
	exit(1);
}
#endif