#include "modular.hpp"
#include <duktape.h>
#include <raylib.h>
typedef struct ImgWrapper{
	Image image;
	Texture2D texture;
}ImgWrapper;
duk_ret_t load(duk_context *ctx) {
	const char *path = duk_require_string(ctx, 0);
	Image image = LoadImage(path);
	Texture2D texture = LoadTextureFromImage(image);
	ImgWrapper *iw = new ImgWrapper();
	iw->image = image;
	iw->texture = texture;
	duk_push_pointer(ctx, iw);

	return 1;
}

duk_ret_t unloadImage(duk_context *ctx) {
	ImgWrapper *iw = (ImgWrapper *)duk_require_pointer(ctx, 0);
	Image *image = &iw->image;
	UnloadImage(*image);
	return 0;
}

duk_ret_t uloadTexture(duk_context *ctx) {
	ImgWrapper *iw = (ImgWrapper *)duk_require_pointer(ctx, 0);
	Texture2D texture = iw->texture;
	UnloadTexture(texture);
	return 0;
}

duk_ret_t drawTexture(duk_context *ctx) {
	ImgWrapper *iw = (ImgWrapper *)duk_require_pointer(ctx, 0);
	Texture2D *texture = &iw->texture;
	int x = duk_require_number(ctx, 1);
	int y = duk_require_number(ctx, 2);
	Color tint = getColor(ctx,3);
	DrawTexture(*texture, x, y, tint);
	return 0;
}

void initImg(duk_context *ctx) {
	duk_push_c_function(ctx, load, 1);
	duk_put_prop_string(ctx, -2, "LoadImageAndTexture");
	duk_push_c_function(ctx, unloadImage, 1);
	duk_put_prop_string(ctx, -2, "UnloadImage");
	duk_push_c_function(ctx, uloadTexture, 1);
	duk_put_prop_string(ctx, -2, "UnloadTexture");
	duk_push_c_function(ctx, drawTexture, 4);
	duk_put_prop_string(ctx, -2, "DrawTexture");
}
