#include <raylib.h>
#include <duktape.h>

void pushColor(duk_context *ctx, Color color, const char *name) {
	duk_push_object(ctx);
	duk_push_int(ctx, color.r);
	duk_put_prop_string(ctx, -2, "r");
	duk_push_int(ctx, color.g);
	duk_put_prop_string(ctx, -2, "g");
	duk_push_int(ctx, color.b);
	duk_put_prop_string(ctx, -2, "b");
	duk_push_int(ctx, color.a);
	duk_put_prop_string(ctx, -2, "a");
	duk_put_global_string(ctx, name);
}

void initColors(duk_context *ctx) {
	pushColor(ctx, RED, "red");
	pushColor(ctx, GREEN, "green");
	pushColor(ctx, BLUE, "blue");
	pushColor(ctx, WHITE, "white");
	pushColor(ctx, BLACK, "black");
	pushColor(ctx, MAGENTA, "magenta");
	pushColor(ctx, YELLOW, "yellow");
	pushColor(ctx, ORANGE, "orange");
	pushColor(ctx, BROWN, "brown");
}
