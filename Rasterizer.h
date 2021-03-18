#ifndef _RASTERIZER_H_
#define _RASTERIZER_H_
#pragma once

#include "vector.h"
#include "buffer.h"
#include <SDL.h>
class Rasterizer
{
public:
	static void drawLine(Vec3f& x, Vec3f&y, const Uint32 &color, Buffer<Uint32> *pixelBuffer);
	static void drawWireFrame(Vec3f* vertices, Buffer<Uint32> *pixelBuffer);
	static void drawTri(Vec3f* vertices, Buffer<Uint32> *pixelShader, Buffer<float> *zBuffer);
private:
	Rasterizer() {};

	static const Uint32 PIXEL_FORMAT = SDL_PIXELFORMAT_RGB888;
	static const SDL_PixelFormat* mappingFormat;

	static const Uint32 white;
	static const Uint32 red;
	static const Uint32 green;
	static const Uint32 blue;

};
#endif


