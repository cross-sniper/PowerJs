/*About:
this is a raylib module for a js engine called "mix"
it is built ontop of duktape, and has a module api, that allows for
extensability

what this module does, is to provide a way to access the raylib api from the js
engine allowing you to make games using raylib + javascript(ecmascript, since
duktape is made for that)
*/
#include "modular.hpp" // for registerModule, duk_fn, duk_func
#include "camera.cpp"
#include "images.cpp"
#include "raylib.h"
#include <duk_config.h>
#include <duktape.h>
#include <vector>
Rectangle getRectangle(duk_context *ctx, int offset) {

  Rectangle r = {0, 0, 0, 0}; // Initialize rectangle to zero
  duk_get_prop_string(ctx, offset, "x");
  duk_get_prop_string(ctx, offset, "y");
  duk_get_prop_string(ctx, offset, "width");
  duk_get_prop_string(ctx, offset, "height");

  // Check if all properties exist and are numbers
  if (duk_is_number(ctx, -4) && duk_is_number(ctx, -3) &&
      duk_is_number(ctx, -2) && duk_is_number(ctx, -1)) {
    // Extract rectangle values
    r.x = duk_get_int(ctx, -4);
    r.y = duk_get_int(ctx, -3);
    r.width = duk_get_int(ctx, -2);
    r.height = duk_get_int(ctx, -1);
    return r;
  }
  printf("Error: Rectangle properties missing or not numbers\n");
  exit(1);
}

static duk_ret_t initwin(duk_context *ctx) {
  if (duk_get_top(ctx) != 3) {
    duk_error(ctx, DUK_ERR_TYPE_ERROR, "wrong number of arguments");
    return 0;
  }
  double w, h;
  const char *title;
  w = duk_get_number(ctx, 0);
  h = duk_get_number(ctx, 1);
  title = duk_get_string(ctx, 2);

  InitWindow(w, h, title);
  return 0;
}

static duk_ret_t closewin(duk_context *ctx) {
  CloseWindow();
  return 0;
}

static duk_ret_t winShouldClose(duk_context *ctx) {
  duk_push_boolean(ctx, WindowShouldClose());
  return 1;
}

static duk_ret_t clearBg(duk_context *ctx) {
  Color c = getColor(ctx, 0);
  ClearBackground(c);
  return 0;
}

static duk_ret_t drawfps(duk_context *ctx) {
  int x, y;
  x = duk_get_number(ctx, 0);
  y = duk_get_number(ctx, 1);
  DrawFPS(x, y);
  return 0;
}
static duk_ret_t setTargetFPS(duk_context *ctx) {
  SetTargetFPS(duk_get_number(ctx, 0));
  return 0;
}

static duk_ret_t isKeyDown(duk_context *ctx) {
  int key = duk_get_number(ctx, 0);
  duk_push_boolean(ctx, IsKeyDown(key));
  return 1;
}

static duk_ret_t drawText(duk_context *ctx) {
  const char *text = duk_get_string(ctx, 0);
  int x = duk_get_number(ctx, 1);
  int y = duk_get_number(ctx, 2);
  int s = duk_get_number(ctx, 3);
  Color c = getColor(ctx, 4);
  DrawText(text, x, y, s, c);
  return 0;
}

static duk_ret_t drawRectangle(duk_context *ctx) {
  Rectangle rec;
  rec.x = duk_get_number(ctx, 0);
  rec.y = duk_get_number(ctx, 1);
  rec.width = duk_get_number(ctx, 2);
  rec.height = duk_get_number(ctx, 3);
  Color c = getColor(ctx, 4);
  DrawRectangle(rec.x, rec.y, rec.width, rec.height, c);
  return 0;
}
static duk_ret_t drawLine(duk_context *ctx) {

  int x = duk_get_number(ctx, 0);
  int y = duk_get_number(ctx, 1);
  int x2 = duk_get_number(ctx, 2);
  int y2 = duk_get_number(ctx, 3);
  Color c = getColor(ctx, 4);
  DrawLine(x, y, x2, y2, c);
  return 0;
}

static duk_ret_t beginDrawing(duk_context *ctx) {
  BeginDrawing();
  return 0;
}
static duk_ret_t endDrawing(duk_context *ctx) {
  EndDrawing();
  return 0;
}
static duk_ret_t drawRectLines(duk_context *ctx) {
  Rectangle rec;
  rec.x = duk_get_number(ctx, 0);
  rec.y = duk_get_number(ctx, 1);
  rec.width = duk_get_number(ctx, 2);
  rec.height = duk_get_number(ctx, 3);
  Color c = getColor(ctx, 4);
  DrawRectangleLines(rec.x, rec.y, rec.width, rec.height, c);
  return 0;
}
static duk_ret_t drawCircle(duk_context *ctx) {
  int x = duk_get_number(ctx, 0);
  int y = duk_get_number(ctx, 1);
  int radius = duk_get_number(ctx, 2);
  Color c = getColor(ctx, 3);
  DrawCircle(x, y, radius, c);
  return 0;
}

static duk_ret_t getFrameTime(duk_context *ctx) {
  duk_push_number(ctx, GetFrameTime());
  return 1;
}
static duk_ret_t getMousePos(duk_context *ctx) {
  duk_push_object(ctx);
  duk_push_number(ctx, GetMouseX());
  duk_put_prop_string(ctx, -2, "x");
  duk_push_number(ctx, GetMouseY());
  duk_put_prop_string(ctx, -2, "y");
  return 1;
}

static duk_ret_t isMouseButtonDown(duk_context *ctx) {
  int button = duk_get_number(ctx, 0);
  duk_push_boolean(ctx, IsMouseButtonDown(button));
  return 1;
}
static duk_ret_t isMouseButtonPressed(duk_context *ctx) {
  int button = duk_get_number(ctx, 0);
  duk_push_boolean(ctx, IsMouseButtonPressed(button));
  return 1;
}

static duk_ret_t isKeyPressed(duk_context *ctx) {
  int button = duk_get_number(ctx, 0);
  duk_push_boolean(ctx, IsKeyPressed(button));
  return 1;
}
static duk_ret_t getMouseX(duk_context *ctx) {
  duk_push_number(ctx, GetMouseX());
  return 1;
}
static duk_ret_t getMouseY(duk_context *ctx) {
  duk_push_number(ctx, GetMouseY());
  return 1;
}

duk_func fns[] = {{"InitWindow", initwin},
                  {"CloseWindow", closewin},
                  {"WindowShouldClose", winShouldClose},
                  {"ClearBackground", clearBg},
                  {"DrawFPS", drawfps},
                  {"GetFrameTime", getFrameTime},
                  {"SetTargetFPS", setTargetFPS},
                  {"BeginDrawing", beginDrawing},
                  {"EndDrawing", endDrawing},
                  {"DrawText", drawText},
                  {"DrawLine", drawLine},
                  {"DrawRectangle", drawRectangle},
                  {"IsKeyDown", isKeyDown},
                  {"IsKeyPressed", isKeyPressed},
                  {"DrawRectangleLines", drawRectLines},
                  {"DrawCircle", drawCircle},
                  {NULL, NULL}};

void init_raylib_keys(duk_context *L) {
  // Alphanumeric keys
  setObjectIntVal(L, "KEY_APOSTROPHE", KEY_APOSTROPHE);
  setObjectIntVal(L, "KEY_COMMA", KEY_COMMA);
  setObjectIntVal(L, "KEY_MINUS", KEY_MINUS);
  setObjectIntVal(L, "KEY_PERIOD", KEY_PERIOD);
  setObjectIntVal(L, "KEY_SLASH", KEY_SLASH);
  setObjectIntVal(L, "KEY_ZERO", KEY_ZERO);
  setObjectIntVal(L, "KEY_ONE", KEY_ONE);
  setObjectIntVal(L, "KEY_TWO", KEY_TWO);
  setObjectIntVal(L, "KEY_THREE", KEY_THREE);
  setObjectIntVal(L, "KEY_FOUR", KEY_FOUR);
  setObjectIntVal(L, "KEY_FIVE", KEY_FIVE);
  setObjectIntVal(L, "KEY_SIX", KEY_SIX);
  setObjectIntVal(L, "KEY_SEVEN", KEY_SEVEN);
  setObjectIntVal(L, "KEY_EIGHT", KEY_EIGHT);
  setObjectIntVal(L, "KEY_NINE", KEY_NINE);
  setObjectIntVal(L, "KEY_SEMICOLON", KEY_SEMICOLON);
  setObjectIntVal(L, "KEY_EQUAL", KEY_EQUAL);
  setObjectIntVal(L, "KEY_A", KEY_A);
  setObjectIntVal(L, "KEY_B", KEY_B);
  setObjectIntVal(L, "KEY_C", KEY_C);
  setObjectIntVal(L, "KEY_D", KEY_D);
  setObjectIntVal(L, "KEY_E", KEY_E);
  setObjectIntVal(L, "KEY_F", KEY_F);
  setObjectIntVal(L, "KEY_G", KEY_G);
  setObjectIntVal(L, "KEY_H", KEY_H);
  setObjectIntVal(L, "KEY_I", KEY_I);
  setObjectIntVal(L, "KEY_J", KEY_J);
  setObjectIntVal(L, "KEY_K", KEY_K);
  setObjectIntVal(L, "KEY_L", KEY_L);
  setObjectIntVal(L, "KEY_M", KEY_M);
  setObjectIntVal(L, "KEY_N", KEY_N);
  setObjectIntVal(L, "KEY_O", KEY_O);
  setObjectIntVal(L, "KEY_P", KEY_P);
  setObjectIntVal(L, "KEY_Q", KEY_Q);
  setObjectIntVal(L, "KEY_R", KEY_R);
  setObjectIntVal(L, "KEY_S", KEY_S);
  setObjectIntVal(L, "KEY_T", KEY_T);
  setObjectIntVal(L, "KEY_U", KEY_U);
  setObjectIntVal(L, "KEY_V", KEY_V);
  setObjectIntVal(L, "KEY_W", KEY_W);
  setObjectIntVal(L, "KEY_X", KEY_X);
  setObjectIntVal(L, "KEY_Y", KEY_Y);
  setObjectIntVal(L, "KEY_Z", KEY_Z);
  setObjectIntVal(L, "KEY_LEFT_BRACKET", KEY_LEFT_BRACKET);
  setObjectIntVal(L, "KEY_BACKSLASH", KEY_BACKSLASH);
  setObjectIntVal(L, "KEY_RIGHT_BRACKET", KEY_RIGHT_BRACKET);
  setObjectIntVal(L, "KEY_GRAVE", KEY_GRAVE);

  // Function keys
  setObjectIntVal(L, "KEY_SPACE", KEY_SPACE);
  setObjectIntVal(L, "KEY_ESCAPE", KEY_ESCAPE);
  setObjectIntVal(L, "KEY_ENTER", KEY_ENTER);
  setObjectIntVal(L, "KEY_TAB", KEY_TAB);
  setObjectIntVal(L, "KEY_BACKSPACE", KEY_BACKSPACE);
  setObjectIntVal(L, "KEY_INSERT", KEY_INSERT);
  setObjectIntVal(L, "KEY_DELETE", KEY_DELETE);
  setObjectIntVal(L, "KEY_RIGHT", KEY_RIGHT);
  setObjectIntVal(L, "KEY_LEFT", KEY_LEFT);
  setObjectIntVal(L, "KEY_DOWN", KEY_DOWN);
  setObjectIntVal(L, "KEY_UP", KEY_UP);
  setObjectIntVal(L, "KEY_PAGE_UP", KEY_PAGE_UP);
  setObjectIntVal(L, "KEY_PAGE_DOWN", KEY_PAGE_DOWN);
  setObjectIntVal(L, "KEY_HOME", KEY_HOME);
  setObjectIntVal(L, "KEY_END", KEY_END);
  setObjectIntVal(L, "KEY_CAPS_LOCK", KEY_CAPS_LOCK);
  setObjectIntVal(L, "KEY_SCROLL_LOCK", KEY_SCROLL_LOCK);
  setObjectIntVal(L, "KEY_NUM_LOCK", KEY_NUM_LOCK);
  setObjectIntVal(L, "KEY_PRINT_SCREEN", KEY_PRINT_SCREEN);
  setObjectIntVal(L, "KEY_PAUSE", KEY_PAUSE);
  setObjectIntVal(L, "KEY_F1", KEY_F1);
  setObjectIntVal(L, "KEY_F2", KEY_F2);
  setObjectIntVal(L, "KEY_F3", KEY_F3);
  setObjectIntVal(L, "KEY_F4", KEY_F4);
  setObjectIntVal(L, "KEY_F5", KEY_F5);
  setObjectIntVal(L, "KEY_F6", KEY_F6);
  setObjectIntVal(L, "KEY_F7", KEY_F7);
  setObjectIntVal(L, "KEY_F8", KEY_F8);
  setObjectIntVal(L, "KEY_F9", KEY_F9);
  setObjectIntVal(L, "KEY_F10", KEY_F10);
  setObjectIntVal(L, "KEY_F11", KEY_F11);
  setObjectIntVal(L, "KEY_F12", KEY_F12);
  setObjectIntVal(L, "KEY_LEFT_SHIFT", KEY_LEFT_SHIFT);
  setObjectIntVal(L, "KEY_LEFT_CONTROL", KEY_LEFT_CONTROL);
  setObjectIntVal(L, "KEY_LEFT_ALT", KEY_LEFT_ALT);
  setObjectIntVal(L, "KEY_LEFT_SUPER", KEY_LEFT_SUPER);
  setObjectIntVal(L, "KEY_RIGHT_SHIFT", KEY_RIGHT_SHIFT);
  setObjectIntVal(L, "KEY_RIGHT_CONTROL", KEY_RIGHT_CONTROL);
  setObjectIntVal(L, "KEY_RIGHT_ALT", KEY_RIGHT_ALT);
  setObjectIntVal(L, "KEY_RIGHT_SUPER", KEY_RIGHT_SUPER);
  setObjectIntVal(L, "KEY_KB_MENU", KEY_KB_MENU);

  // Keypad keys
  setObjectIntVal(L, "KEY_KP_0", KEY_KP_0);
  setObjectIntVal(L, "KEY_KP_1", KEY_KP_1);
  setObjectIntVal(L, "KEY_KP_2", KEY_KP_2);
  setObjectIntVal(L, "KEY_KP_3", KEY_KP_3);
  setObjectIntVal(L, "KEY_KP_4", KEY_KP_4);
  setObjectIntVal(L, "KEY_KP_5", KEY_KP_5);
  setObjectIntVal(L, "KEY_KP_6", KEY_KP_6);
  setObjectIntVal(L, "KEY_KP_7", KEY_KP_7);
  setObjectIntVal(L, "KEY_KP_8", KEY_KP_8);
  setObjectIntVal(L, "KEY_KP_9", KEY_KP_9);
  setObjectIntVal(L, "KEY_KP_DECIMAL", KEY_KP_DECIMAL);
  setObjectIntVal(L, "KEY_KP_DIVIDE", KEY_KP_DIVIDE);
  setObjectIntVal(L, "KEY_KP_MULTIPLY", KEY_KP_MULTIPLY);
  setObjectIntVal(L, "KEY_KP_SUBTRACT", KEY_KP_SUBTRACT);
  setObjectIntVal(L, "KEY_KP_ADD", KEY_KP_ADD);
  setObjectIntVal(L, "KEY_KP_ENTER", KEY_KP_ENTER);
  setObjectIntVal(L, "KEY_KP_EQUAL", KEY_KP_EQUAL);

  // Android key buttons
  setObjectIntVal(L, "KEY_BACK", KEY_BACK);
  setObjectIntVal(L, "KEY_MENU", KEY_MENU);
  setObjectIntVal(L, "KEY_VOLUME_UP", KEY_VOLUME_UP);
  setObjectIntVal(L, "KEY_VOLUME_DOWN", KEY_VOLUME_DOWN);
}
void initMouse(duk_context *L) {
  setObjectIntVal(L, "MOUSE_LEFT_BUTTON", MOUSE_LEFT_BUTTON);
  setObjectIntVal(L, "MOUSE_RIGHT_BUTTON", MOUSE_RIGHT_BUTTON);
  setObjectIntVal(L, "MOUSE_MIDDLE_BUTTON", MOUSE_MIDDLE_BUTTON);

  duk_push_c_function(L, getMouseX, 0);
  duk_put_prop_string(L, -2, "GetMouseX");

  duk_push_c_function(L, getMouseY, 0);
  duk_put_prop_string(L, -2, "GetMouseY");

  duk_push_c_function(L, isMouseButtonPressed, 1);
  duk_put_prop_string(L, -2, "IsMouseButtonPressed");

  duk_push_c_function(L, isMouseButtonDown, 1);
  duk_put_prop_string(L, -2, "IsMouseButtonDown");

  duk_push_c_function(L, getMousePos, 0);
  duk_put_prop_string(L, -2, "GetMousePosition");
}

extern "C" duk_ret_t dukopen_raylib(duk_context *ctx) {
  registerModule(ctx, fns);
  init_raylib_keys(ctx);
  initMouse(ctx);
  CamInit(ctx);
  initImg(ctx);
  setGlobalModule(ctx, "raylib");
  return 1;
}
