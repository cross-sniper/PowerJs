#include <duk_config.h>
#include <duktape.h>

duk_ret_t Vec2(duk_context *ctx) {
  float x = duk_get_number(ctx, 0);
  float y = duk_get_number(ctx, 1);
  duk_push_object(ctx);
  duk_push_number(ctx, x);
  duk_put_prop_string(ctx, -2, "x");
  duk_push_number(ctx, y);
  duk_put_prop_string(ctx, -2, "y");

  return 1;
}

void initExtras(duk_context *ctx) {
  duk_push_c_function(ctx, Vec2, 2);
  duk_put_global_string(ctx, "Vec2");
}
