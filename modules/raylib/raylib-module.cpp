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
#include "ray-colors.cpp"

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
static duk_ret_t drawTriangle(duk_context *ctx) {
	float a1,a2, b1,b2, c1,c2;
  a1 = getArg(ctx, 0, "x");
  a2 = getArg(ctx, 0, "y");

  b1 = getArg(ctx, 1, "x");
  b2 = getArg(ctx, 1, "y");

  c1 = getArg(ctx, 2, "x");
  c2 = getArg(ctx, 2, "y");

	Color c = getColor(ctx, 3);

  DrawTriangle({a1,a2},{b1,b2},{c1,c2},c);
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

duk_func raylibFunctions[] = {{"InitWindow", initwin, "initializes the window"},
                  {"CloseWindow", closewin, "closes the window"},
                  {"WindowShouldClose", winShouldClose, "returns if the window should close"},
                  {"ClearBackground", clearBg, "clears the background with a given color"},
                  {"DrawFPS", drawfps, "shows the current fps at a give x,y coordnate"},
                  {"GetFrameTime", getFrameTime,"returns the current dt(delta time)"},
                  {"SetTargetFPS", setTargetFPS, "makes raylib try to stay at a given fps"},
                  {"BeginDrawing", beginDrawing, "begins the drawing process"},
                  {"EndDrawing", endDrawing, "ends the drawing process"},
                  {"DrawText", drawText, "draws text on the screen"},
                  {"DrawLine", drawLine, "draws a line"},
                  {"DrawRectangle", drawRectangle, "draws a rectangle"},
                  {"DrawTriangle", drawTriangle, "draws a triangle"},
                  {"IsKeyDown", isKeyDown, "checks if a key is held down"},
                  {"IsKeyPressed", isKeyPressed,"checks if a key is pressed"},
                  {"DrawRectangleLines", drawRectLines, "draws a rectangle outline"},
                  {"DrawCircle", drawCircle, "draws a circle"},
                  {NULL, NULL, "end of module header"}};

void init_raylib_keys(duk_context *L) {
  // Alphanumeric keys
  setGlobalIntVal(L, "KEY_APOSTROPHE", KEY_APOSTROPHE);
  setGlobalIntVal(L, "KEY_COMMA", KEY_COMMA);
  setGlobalIntVal(L, "KEY_MINUS", KEY_MINUS);
  setGlobalIntVal(L, "KEY_PERIOD", KEY_PERIOD);
  setGlobalIntVal(L, "KEY_SLASH", KEY_SLASH);
  setGlobalIntVal(L, "KEY_ZERO", KEY_ZERO);
  setGlobalIntVal(L, "KEY_ONE", KEY_ONE);
  setGlobalIntVal(L, "KEY_TWO", KEY_TWO);
  setGlobalIntVal(L, "KEY_THREE", KEY_THREE);
  setGlobalIntVal(L, "KEY_FOUR", KEY_FOUR);
  setGlobalIntVal(L, "KEY_FIVE", KEY_FIVE);
  setGlobalIntVal(L, "KEY_SIX", KEY_SIX);
  setGlobalIntVal(L, "KEY_SEVEN", KEY_SEVEN);
  setGlobalIntVal(L, "KEY_EIGHT", KEY_EIGHT);
  setGlobalIntVal(L, "KEY_NINE", KEY_NINE);
  setGlobalIntVal(L, "KEY_SEMICOLON", KEY_SEMICOLON);
  setGlobalIntVal(L, "KEY_EQUAL", KEY_EQUAL);
  setGlobalIntVal(L, "KEY_A", KEY_A);
  setGlobalIntVal(L, "KEY_B", KEY_B);
  setGlobalIntVal(L, "KEY_C", KEY_C);
  setGlobalIntVal(L, "KEY_D", KEY_D);
  setGlobalIntVal(L, "KEY_E", KEY_E);
  setGlobalIntVal(L, "KEY_F", KEY_F);
  setGlobalIntVal(L, "KEY_G", KEY_G);
  setGlobalIntVal(L, "KEY_H", KEY_H);
  setGlobalIntVal(L, "KEY_I", KEY_I);
  setGlobalIntVal(L, "KEY_J", KEY_J);
  setGlobalIntVal(L, "KEY_K", KEY_K);
  setGlobalIntVal(L, "KEY_L", KEY_L);
  setGlobalIntVal(L, "KEY_M", KEY_M);
  setGlobalIntVal(L, "KEY_N", KEY_N);
  setGlobalIntVal(L, "KEY_O", KEY_O);
  setGlobalIntVal(L, "KEY_P", KEY_P);
  setGlobalIntVal(L, "KEY_Q", KEY_Q);
  setGlobalIntVal(L, "KEY_R", KEY_R);
  setGlobalIntVal(L, "KEY_S", KEY_S);
  setGlobalIntVal(L, "KEY_T", KEY_T);
  setGlobalIntVal(L, "KEY_U", KEY_U);
  setGlobalIntVal(L, "KEY_V", KEY_V);
  setGlobalIntVal(L, "KEY_W", KEY_W);
  setGlobalIntVal(L, "KEY_X", KEY_X);
  setGlobalIntVal(L, "KEY_Y", KEY_Y);
  setGlobalIntVal(L, "KEY_Z", KEY_Z);
  setGlobalIntVal(L, "KEY_LEFT_BRACKET", KEY_LEFT_BRACKET);
  setGlobalIntVal(L, "KEY_BACKSLASH", KEY_BACKSLASH);
  setGlobalIntVal(L, "KEY_RIGHT_BRACKET", KEY_RIGHT_BRACKET);
  setGlobalIntVal(L, "KEY_GRAVE", KEY_GRAVE);

  // Function keys
  setGlobalIntVal(L, "KEY_SPACE", KEY_SPACE);
  setGlobalIntVal(L, "KEY_ESCAPE", KEY_ESCAPE);
  setGlobalIntVal(L, "KEY_ENTER", KEY_ENTER);
  setGlobalIntVal(L, "KEY_TAB", KEY_TAB);
  setGlobalIntVal(L, "KEY_BACKSPACE", KEY_BACKSPACE);
  setGlobalIntVal(L, "KEY_INSERT", KEY_INSERT);
  setGlobalIntVal(L, "KEY_DELETE", KEY_DELETE);
  setGlobalIntVal(L, "KEY_RIGHT", KEY_RIGHT);
  setGlobalIntVal(L, "KEY_LEFT", KEY_LEFT);
  setGlobalIntVal(L, "KEY_DOWN", KEY_DOWN);
  setGlobalIntVal(L, "KEY_UP", KEY_UP);
  setGlobalIntVal(L, "KEY_PAGE_UP", KEY_PAGE_UP);
  setGlobalIntVal(L, "KEY_PAGE_DOWN", KEY_PAGE_DOWN);
  setGlobalIntVal(L, "KEY_HOME", KEY_HOME);
  setGlobalIntVal(L, "KEY_END", KEY_END);
  setGlobalIntVal(L, "KEY_CAPS_LOCK", KEY_CAPS_LOCK);
  setGlobalIntVal(L, "KEY_SCROLL_LOCK", KEY_SCROLL_LOCK);
  setGlobalIntVal(L, "KEY_NUM_LOCK", KEY_NUM_LOCK);
  setGlobalIntVal(L, "KEY_PRINT_SCREEN", KEY_PRINT_SCREEN);
  setGlobalIntVal(L, "KEY_PAUSE", KEY_PAUSE);
  setGlobalIntVal(L, "KEY_F1", KEY_F1);
  setGlobalIntVal(L, "KEY_F2", KEY_F2);
  setGlobalIntVal(L, "KEY_F3", KEY_F3);
  setGlobalIntVal(L, "KEY_F4", KEY_F4);
  setGlobalIntVal(L, "KEY_F5", KEY_F5);
  setGlobalIntVal(L, "KEY_F6", KEY_F6);
  setGlobalIntVal(L, "KEY_F7", KEY_F7);
  setGlobalIntVal(L, "KEY_F8", KEY_F8);
  setGlobalIntVal(L, "KEY_F9", KEY_F9);
  setGlobalIntVal(L, "KEY_F10", KEY_F10);
  setGlobalIntVal(L, "KEY_F11", KEY_F11);
  setGlobalIntVal(L, "KEY_F12", KEY_F12);
  setGlobalIntVal(L, "KEY_LEFT_SHIFT", KEY_LEFT_SHIFT);
  setGlobalIntVal(L, "KEY_LEFT_CONTROL", KEY_LEFT_CONTROL);
  setGlobalIntVal(L, "KEY_LEFT_ALT", KEY_LEFT_ALT);
  setGlobalIntVal(L, "KEY_LEFT_SUPER", KEY_LEFT_SUPER);
  setGlobalIntVal(L, "KEY_RIGHT_SHIFT", KEY_RIGHT_SHIFT);
  setGlobalIntVal(L, "KEY_RIGHT_CONTROL", KEY_RIGHT_CONTROL);
  setGlobalIntVal(L, "KEY_RIGHT_ALT", KEY_RIGHT_ALT);
  setGlobalIntVal(L, "KEY_RIGHT_SUPER", KEY_RIGHT_SUPER);
  setGlobalIntVal(L, "KEY_KB_MENU", KEY_KB_MENU);

  // Keypad keys
  setGlobalIntVal(L, "KEY_KP_0", KEY_KP_0);
  setGlobalIntVal(L, "KEY_KP_1", KEY_KP_1);
  setGlobalIntVal(L, "KEY_KP_2", KEY_KP_2);
  setGlobalIntVal(L, "KEY_KP_3", KEY_KP_3);
  setGlobalIntVal(L, "KEY_KP_4", KEY_KP_4);
  setGlobalIntVal(L, "KEY_KP_5", KEY_KP_5);
  setGlobalIntVal(L, "KEY_KP_6", KEY_KP_6);
  setGlobalIntVal(L, "KEY_KP_7", KEY_KP_7);
  setGlobalIntVal(L, "KEY_KP_8", KEY_KP_8);
  setGlobalIntVal(L, "KEY_KP_9", KEY_KP_9);
  setGlobalIntVal(L, "KEY_KP_DECIMAL", KEY_KP_DECIMAL);
  setGlobalIntVal(L, "KEY_KP_DIVIDE", KEY_KP_DIVIDE);
  setGlobalIntVal(L, "KEY_KP_MULTIPLY", KEY_KP_MULTIPLY);
  setGlobalIntVal(L, "KEY_KP_SUBTRACT", KEY_KP_SUBTRACT);
  setGlobalIntVal(L, "KEY_KP_ADD", KEY_KP_ADD);
  setGlobalIntVal(L, "KEY_KP_ENTER", KEY_KP_ENTER);
  setGlobalIntVal(L, "KEY_KP_EQUAL", KEY_KP_EQUAL);

  // Android key buttons
  setGlobalIntVal(L, "KEY_BACK", KEY_BACK);
  setGlobalIntVal(L, "KEY_MENU", KEY_MENU);
  setGlobalIntVal(L, "KEY_VOLUME_UP", KEY_VOLUME_UP);
  setGlobalIntVal(L, "KEY_VOLUME_DOWN", KEY_VOLUME_DOWN);
}
void initMouse(duk_context *L) {
  setGlobalIntVal(L, "MOUSE_LEFT_BUTTON", MOUSE_LEFT_BUTTON);
  setGlobalIntVal(L, "MOUSE_RIGHT_BUTTON", MOUSE_RIGHT_BUTTON);
  setGlobalIntVal(L, "MOUSE_MIDDLE_BUTTON", MOUSE_MIDDLE_BUTTON);

  duk_push_c_function(L, getMouseX, 0);
  // Set a property on the function to store the function description (useful for error messages)
  duk_push_string(L, "returns the mouse X position");
  duk_put_prop_string(L, -2, "__desc");
  duk_put_global_string(L, "GetMouseX");


  duk_push_c_function(L, getMouseY, 0);
  duk_push_string(L, "returns the mouse Y position");
  duk_put_prop_string(L, -2, "__desc");
  duk_put_global_string(L, "GetMouseY");

  duk_push_c_function(L, isMouseButtonPressed, 1);
  duk_push_string(L, "returns true if a mouse button has been pressed");
  duk_put_prop_string(L, -2, "__desc");
  duk_put_global_string(L, "IsMouseButtonPressed");

  duk_push_c_function(L, isMouseButtonDown, 1);
  duk_push_string(L, "returns true if a mouse button is currently down");
  duk_put_prop_string(L, -2, "__desc");
  duk_put_global_string(L, "IsMouseButtonDown");

  duk_push_c_function(L, getMousePos, 0);
  duk_push_string(L, "returns the mouse position");
  duk_put_prop_string(L, -2, "__desc");
  duk_put_global_string(L, "GetMousePosition");
}

extern "C" duk_ret_t dukopen_raylib(duk_context *ctx) {
  registerModule(ctx, raylibFunctions);
  init_raylib_keys(ctx);
  initMouse(ctx);
  CamInit(ctx);
  initImg(ctx);
  initColors(ctx);

  return 1;
}
