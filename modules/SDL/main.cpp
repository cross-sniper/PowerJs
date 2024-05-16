#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_render.h>
#include <duktape.h>

// Function to create an SDL window
static duk_ret_t sdl_create_window(duk_context *ctx) {
  int width = duk_require_int(ctx, 0);
  int height = duk_require_int(ctx, 1);
  const char *title = duk_require_string(ctx, 2);

  // Initialize SDL video subsystem
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    duk_error(ctx, DUK_ERR_ERROR, "Failed to initialize SDL video: %s",
              SDL_GetError());
  }
  // Check if either width or height is 0
  if (width == 0 || height == 0) {
    // Get the screen dimensions
    SDL_DisplayMode dm;
    if (SDL_GetCurrentDisplayMode(0, &dm) != 0) {
      duk_error(ctx, DUK_ERR_ERROR, "Failed to get display mode\n%s\n",
                SDL_GetError());
    }

    // Use screen width/height if either width or height is 0
    if (width == 0) {
      width = dm.w;
    }
    if (height == 0) {
      height = dm.h;
    }
  }

  // Create SDL window
  SDL_Window *window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       width, height, SDL_WINDOW_SHOWN);
  if (window == NULL) {
    duk_error(ctx, DUK_ERR_ERROR, "Failed to create SDL window");
  }

  // Return the pointer to the SDL_Window as lightuserdata
  duk_push_pointer(ctx, window);
  return 1;
}

// Function to destroy an SDL window
static duk_ret_t sdl_destroy_window(duk_context *ctx) {
  SDL_Window *window = (SDL_Window *)duk_require_pointer(ctx, 0);
  SDL_DestroyWindow(window);
  return 0;
}

// Function to create an SDL renderer
static duk_ret_t sdl_create_renderer(duk_context *ctx) {
  SDL_Window *window = (SDL_Window *)duk_require_pointer(ctx, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (renderer == NULL) {
    duk_error(ctx, DUK_ERR_ERROR, "Failed to create SDL renderer");
  }
  duk_push_pointer(ctx, renderer);
  return 1;
}

// Function to destroy an SDL renderer
static duk_ret_t sdl_destroy_renderer(duk_context *ctx) {
  SDL_Renderer *renderer = (SDL_Renderer *)duk_require_pointer(ctx, 0);
  SDL_DestroyRenderer(renderer);
  return 0;
}

// Function to draw a filled rectangle
static duk_ret_t sdl_draw_filled_rect(duk_context *ctx) {
  SDL_Renderer *renderer = (SDL_Renderer *)duk_require_pointer(ctx, 0);
  int x = duk_require_int(ctx, 1);
  int y = duk_require_int(ctx, 2);
  int width = duk_require_int(ctx, 3);
  int height = duk_require_int(ctx, 4);

  SDL_Rect rect = {x, y, width, height};
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red color
  SDL_RenderFillRect(renderer, &rect);

  return 0;
}

// Function to draw a filled circle
static duk_ret_t sdl_draw_filled_circle(duk_context *ctx) {
  SDL_Renderer *renderer = (SDL_Renderer *)duk_require_pointer(ctx, 0);
  int x = duk_require_int(ctx, 1);
  int y = duk_require_int(ctx, 2);
  int radius = duk_require_int(ctx, 3);

  for (int i = 0; i < radius * 2; i++) {
    for (int j = 0; j < radius * 2; j++) {
      int dx = radius - i;
      int dy = radius - j;
      if ((dx * dx + dy * dy) <= (radius * radius)) {
        SDL_RenderDrawPoint(renderer, x + dx, y + dy);
      }
    }
  }

  return 0;
}

// Function to present the renderer
static duk_ret_t sdl_present_renderer(duk_context *ctx) {
  SDL_Renderer *renderer = (SDL_Renderer *)duk_require_pointer(ctx, 0);
  SDL_RenderPresent(renderer);
  return 0;
}
static duk_ret_t renderClear(duk_context *ctx) {
  SDL_Renderer *renderer = (SDL_Renderer *)duk_require_pointer(ctx, 0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  return 0;
}
// Function to poll SDL events
static duk_ret_t sdl_pool_events(duk_context *ctx) {
  SDL_Event event;
  if (SDL_PollEvent(&event)) {
    duk_push_object(ctx);

    // Common event properties
    duk_push_number(ctx, (int)event.type);
    duk_put_prop_string(ctx, -2, "type");

    // Handle different types of events
    switch (event.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      // Keyboard events
      duk_push_string(ctx, SDL_GetKeyName(event.key.keysym.sym));
      duk_put_prop_string(ctx, -2, "key");
      return 1;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
      // Mouse button events
      duk_push_number(ctx, event.button.button);
      duk_put_prop_string(ctx, -2, "button");
      duk_push_number(ctx, event.button.x);
      duk_put_prop_string(ctx, -2, "x");
      duk_push_number(ctx, event.button.y);
      duk_put_prop_string(ctx, -2, "y");
      return 1;
    case SDL_MOUSEMOTION:
      // Mouse motion event
      duk_push_number(ctx, event.motion.x);
      duk_put_prop_string(ctx, -2, "x");
      duk_push_number(ctx, event.motion.y);
      duk_put_prop_string(ctx, -2, "y");
      duk_push_number(ctx, event.motion.xrel);
      duk_put_prop_string(ctx, -2, "xrel");
      duk_push_number(ctx, event.motion.yrel);
      duk_put_prop_string(ctx, -2, "yrel");
      return 1;
    default:
      // Other events
      return 1;
    }
  }
  return 0;
}

// Register SDL functions into the Duktape context
#include "sleep.cpp"
static const duk_function_list_entry sdl_functions[] = {
    {"createWindow", sdl_create_window, 3},
    {"destroyWindow", sdl_destroy_window, 1},
    {"createRenderer", sdl_create_renderer, 1},
    {"destroyRenderer", sdl_destroy_renderer, 1},
    {"drawFilledRect", sdl_draw_filled_rect, 5},
    {"renderClear", renderClear, 1},
    {"sleep", sleep, 1},
    {"drawFilledCircle", sdl_draw_filled_circle, 4},
    {"presentRenderer", sdl_present_renderer, 1},
    {"poolEvents", sdl_pool_events, 0},
    {NULL, NULL, 0}};
#include "events.cpp"
#include "keys.cpp"

// Module initialization function
extern "C" void dukopen_sdl(duk_context *ctx) {
  duk_push_object(ctx);
  duk_put_function_list(ctx, -1, sdl_functions);
  initEvents(ctx);
  initKeys(ctx);
  duk_put_global_string(ctx, "SDL");
}
