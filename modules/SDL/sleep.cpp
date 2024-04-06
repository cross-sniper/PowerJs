#include <duktape.h>
#include <SDL2/SDL.h>

static duk_ret_t sleep(duk_context *ctx) {
    double fps = duk_require_number(ctx, 0); // Retrieve the argument (fps) from the stack
    Uint32 frameDuration = (Uint32)(1000.0 / fps); // Calculate the frame duration in milliseconds
    SDL_Delay(frameDuration); // Pause execution for the calculated frame duration
    return 0; // Sleep function doesn't return anything
}
