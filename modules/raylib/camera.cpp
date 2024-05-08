#include "../../modular.hpp"
#include <raylib.h>

Camera2D cam;

int getArg(duk_context *ctx, int offset, const char *name) {
  int arg = 0;
  // Check if the argument at 'offset' is an object
  if (!duk_is_object(ctx, offset)) {
    // Handle error: Argument is not an object
    printf("Error: Argument at offset %d is not an object\n", offset);
    // print the type
    printf("Type: %d\n", duk_get_type(ctx, offset));
    return arg;
  }

  // Get the 'r', 'g', 'b', and 'a' properties from the object
  duk_get_prop_string(ctx, offset, name);
  // Check if all properties exist and are numbers
  if (duk_is_number(ctx, -1)) {
    // Extract RGBA values
    arg = duk_get_int(ctx, -1);
  } else {
    // Handle error: Properties missing or not numbers
    printf("Error: properties missing or not numbers\n");
    exit(1);
  }

  // Pop values from the stack
  duk_pop_n(ctx, 1);

  return arg;
}
duk_ret_t initCamera(duk_context *ctx) {
  cam = (Camera2D){
      {
          (float)getArg(ctx, 0, "offsetX"),
          (float)getArg(ctx, 0, "offsetY"),
      },
      {
          (float)getArg(ctx, 0, "targetX"),
          (float)getArg(ctx, 0, "targetY"),
      },
      (float)getArg(ctx, 0, "rotation"),
      (float)getArg(ctx, 0, "zoom"),
  };
  return 0;
}

duk_ret_t updateCameraTarget(duk_context *ctx) {
  Vector2 target;
  target.x = float(getArg(ctx, 0, "x"));
  target.y = float(getArg(ctx, 0, "y"));
  cam.target = target;
  return 0;
}
duk_ret_t updateCameraOffset(duk_context *ctx) {
  Vector2 offset;
  offset.x = float(getArg(ctx, 0, "x"));
  offset.y = float(getArg(ctx, 0, "y"));
  cam.offset = offset;
  return 0;
}
duk_ret_t beginCam(duk_context *ctx) {
  BeginMode2D(cam);
  return 0;
}
duk_ret_t endCam(duk_context *ctx) {
  EndMode2D();
  return 0;
}

duk_ret_t getScreenSize(duk_context *ctx) {
  duk_push_object(ctx);
  duk_push_number(ctx, GetScreenWidth());
  duk_put_prop_string(ctx, -2, "width");
  duk_push_number(ctx, GetScreenHeight());
  duk_put_prop_string(ctx, -2, "height");
  return 1;
}
void CamInit(duk_context *ctx) {
  duk_push_c_function(ctx, initCamera, 1);
  duk_put_prop_string(ctx, -2, "initCamera");

  duk_push_c_function(ctx, updateCameraTarget, 1);
  duk_put_prop_string(ctx, -2, "updateCameraTarget");

  duk_push_c_function(ctx, updateCameraOffset, 1);
  duk_put_prop_string(ctx, -2, "updateCameraOffset");

  duk_push_c_function(ctx, getScreenSize, 0);
  duk_put_prop_string(ctx, -2, "getScreenSize");

  duk_push_c_function(ctx, beginCam, 0);
  duk_put_prop_string(ctx, -2, "BeginCam");

  duk_push_c_function(ctx, endCam, 0);
  duk_put_prop_string(ctx, -2, "EndCam");
}
