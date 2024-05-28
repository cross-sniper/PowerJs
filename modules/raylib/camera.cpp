#include "modular.hpp"
#include <raylib.h>
#include <duk_config.h>

Camera2D cam2d;
Camera3D cam3d;

int getArg(duk_context *ctx, int offset, const char *name) {
  int arg = 0;
  // Check if the argument at 'offset' is an object
  if (!duk_is_object(ctx, offset)) {
    // Handle error: Argument is not an object
    printf("Error: Argument at offset %d is not an object\n", offset);
    // print the type
    printf("Type: %d\n", duk_get_type(ctx, offset));
    exit(1);
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

duk_ret_t initCamera2D(duk_context *ctx) {
  cam2d = (Camera2D){
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
duk_ret_t initCamera3D(duk_context *ctx) {
  cam3d = (Camera3D){
      {
          (float)getArg(ctx, 0, "positionX"),
          (float)getArg(ctx, 0, "positionY"),
          (float)getArg(ctx, 0, "positionZ"),
      },
      {
        (float)getArg(ctx, 0,"targetX"),
        (float)getArg(ctx, 0,"targetY"),
        (float)getArg(ctx, 0,"targetZ"),
      },
      {
        0,1,0
      },90,
      CAMERA_FIRST_PERSON
  };
  return 0;
}

duk_ret_t updateCameraTarget2D(duk_context *ctx) {
  Vector2 target;
  target.x = float(getArg(ctx, 0, "x"));
  target.y = float(getArg(ctx, 0, "y"));
  cam2d.target = target;
  return 0;
}

duk_ret_t updateCameraOffset2D(duk_context *ctx) {
  Vector2 offset;
  offset.x = float(getArg(ctx, 0, "x"));
  offset.y = float(getArg(ctx, 0, "y"));
  cam2d.offset = offset;
  return 0;
}

duk_ret_t beginCam2D(duk_context *ctx) {
  BeginMode2D(cam2d);
  return 0;
}
duk_ret_t endCam2D(duk_context *ctx) {
  EndMode2D();
  return 0;
}
duk_ret_t beginCam3D(duk_context *ctx) {
  BeginMode3D(cam3d);
  return 0;
}
duk_ret_t endCam3D(duk_context *ctx) {
  EndMode3D();
  return 0;
}


duk_ret_t updateCameraPosition3D(duk_context *ctx) {
    Vector3 position;
    position.x = (float)getArg(ctx, 0, "positionX");
    position.y = (float)getArg(ctx, 0, "positionY");
    position.z = (float)getArg(ctx, 0, "positionZ");
    cam3d.position = position;
    return 0;
}

duk_ret_t setCameraTarget3D(duk_context *ctx){
    Vector3 target;
    target.x = (float)getArg(ctx, 0, "targetX");
    target.y = (float)getArg(ctx, 0, "targetY");
    target.z = (float)getArg(ctx, 0, "targetZ");
    cam3d.target = target;
  return 0;
}

duk_ret_t updateCameraMode3D(duk_context *ctx) {
    int projection =duk_get_number(ctx, 0);
    cam3d.projection = projection;
    return 0;
}


/*
// Camera system modes
typedef enum {
    CAMERA_CUSTOM = 0,              // Custom camera
    CAMERA_FREE,                    // Free camera
    CAMERA_ORBITAL,                 // Orbital camera
    CAMERA_FIRST_PERSON,            // First person camera
    CAMERA_THIRD_PERSON             // Third person camera
} CameraMode;

*/
void initCamera3dVars(duk_context* ctx){
  duk_push_number(ctx, CAMERA_CUSTOM);
  duk_put_global_string(ctx, "CAMERA_CUSTOM");
  duk_push_number(ctx, CAMERA_FREE);
  duk_put_global_string(ctx, "CAMERA_FREE");
  duk_push_number(ctx, CAMERA_ORBITAL);
  duk_put_global_string(ctx, "CAMERA_ORBITAL");
  duk_push_number(ctx, CAMERA_FIRST_PERSON);
  duk_put_global_string(ctx, "CAMERA_FIRST_PERSON");
  duk_push_number(ctx, CAMERA_THIRD_PERSON);
  duk_put_global_string(ctx, "CAMERA_THIRD_PERSON");
}

duk_ret_t getScreenSize(duk_context *ctx) {
  duk_push_object(ctx);
  duk_push_number(ctx, GetScreenWidth());
  duk_put_prop_string(ctx, -2, "width");
  duk_push_number(ctx, GetScreenHeight());
  duk_put_prop_string(ctx, -2, "height");
  return 1;
}

// TODO: some more functions for 3d camera
void CamInit(duk_context *ctx) {
  duk_push_c_function(ctx, initCamera2D, 1);
  duk_put_global_string(ctx, "initCamera2D");

  duk_push_c_function(ctx, initCamera3D, 1);
  duk_put_global_string(ctx, "initCamera3D");

  duk_push_c_function(ctx, updateCameraTarget2D, 1);
  duk_put_global_string(ctx, "updateCameraTarget2D");

  duk_push_c_function(ctx, updateCameraOffset2D, 1);
  duk_put_global_string(ctx, "updateCameraOffset2D");

  duk_push_c_function(ctx, getScreenSize, 0);
  duk_put_global_string(ctx, "GetScreenSize");

  duk_push_c_function(ctx, beginCam2D, 0);
  duk_put_global_string(ctx, "useCamera2D");

  duk_push_c_function(ctx, endCam2D, 0);
  duk_put_global_string(ctx, "stopCamera2D");

  duk_push_c_function(ctx, beginCam3D, 0);
  duk_put_global_string(ctx, "useCamera3D");

  duk_push_c_function(ctx, endCam3D, 0);
  duk_put_global_string(ctx, "stopCamera3D");

  duk_push_c_function(ctx, updateCameraPosition3D, 1);
  duk_put_global_string(ctx, "updateCameraPosition3D");

  duk_push_c_function(ctx, setCameraTarget3D, 1);
  duk_put_global_string(ctx, "updateCameraRotation3D");

  duk_push_c_function(ctx, updateCameraMode3D, 1);
  duk_put_global_string(ctx, "updateCameraMode3D");

  initCamera3dVars(ctx);
}
