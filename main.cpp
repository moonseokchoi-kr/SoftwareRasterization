#include <iostream>
#include <SDL.h>
#include <algorithm>
#include "ObjParser.h"
#include "buffer.h"
void drawLine(int x0, int x1, int y0, int y1, Uint32 color, Buffer<Uint32> *pixels);
void drawWireframe(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels);
static const int WIDTH = 800;
static const int HEIGHT = 800;
SDL_PixelFormat* mappingFormat(SDL_AllocFormat(SDL_PIXELFORMAT_RGB888));
Uint32 black = SDL_MapRGBA(mappingFormat, 0x00, 0x00, 0x00,0xFF);
Uint32 white = SDL_MapRGBA(mappingFormat, 0xff, 0xff, 0xff, 0xff);

int main(int argc, char ** argv)
{
	bool quit = false;
	SDL_Event event;
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window * window = SDL_CreateWindow("SoftWare Renderer",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);

	SDL_Surface * surface = SDL_GetWindowSurface(window);
	
	Buffer<Uint32> *pixels = new Buffer<Uint32>(WIDTH, HEIGHT, new Uint32[WIDTH * HEIGHT]);

	Mesh mesh;
	std::string  filename = "./testfile/african_head.obj";
	mesh = OBJ::buildMeshFromFile(mesh, filename);
	while (!quit)
	{
		//Fill the surface white
		SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x00, 0x00, 0x00));

		SDL_WaitEvent(&event);
		
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
		
		drawWireframe(mesh, white, pixels);

		//Allows surface editing
		SDL_LockSurface(surface);

		//픽셀버퍼를 surface 로 복사
		memcpy(surface->pixels, pixels->buffer, pixels->mHeight*pixels->mWidth);
		SDL_UnlockSurface(surface);

		//Update the surface
		SDL_UpdateWindowSurface(window);
	}

	delete[] pixels;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void drawLine(int x0, int x1, int y0, int y1, Uint32 color, Buffer<Uint32> *pixels)
{
	//Bresenham의 선그리기 알고리즘
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			(*pixels)(y, x) = color;
		}
		else {
			(*pixels)(x, y) = color;
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}



}

void drawWireframe(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels)
{
	for (int i = 0; i < mesh.vertexIndices.size(); i++)
	{
		Vec3i face = mesh.vertexIndices[i];
		for (int j = 0; j < 3; j++)
		{
			Vec3f v0 = mesh.verts_[face.raw[j]];
			Vec3f v1 = mesh.verts_[face.raw[(j+1)%3]];

			int x0 = (v0.x + 1.) * WIDTH / 2;
			int x1 = (v1.x + 1.) * WIDTH / 2;
			int y0 = (-v0.y + 1.) * HEIGHT / 2;
			int y1 = (-v1.y + 1.) * HEIGHT / 2;
			drawLine(x0, y0, x1, y1, color, pixels);
		}
	}
}
