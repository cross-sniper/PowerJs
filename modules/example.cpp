#include "../modular.hpp"
static int example(duk_context* ctx){
	std::cout << "this is a example module" << std::endl;
	return 0;
}

duk_func fns[] = {
	{"example",example},
	{NULL, NULL}
};
extern "C" duk_ret_t dukopen_example(duk_context* ctx)
{
	registerModule(ctx, fns);
	setGlobalModule(ctx, "example");
	return 1;
}
