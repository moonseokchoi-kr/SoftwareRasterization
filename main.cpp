#include <iostream>
#include <SDL.h>
#include <cmath>
#include <algorithm>
#include "ObjParser.h"
#include "buffer.h"

//#include "lib/tgaimage.h"
void drawLine(int x0, int x1, int y0, int y1, Uint32 color, Buffer<Uint32> *pixels);
void drawWireframe(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels);
void triangle(Vec3f *vertices , Uint32 color, Buffer<Uint32> *pixels);
void triBoundBox(int &maxX, int &maxY, int &minX, int &minY, Vec3f *vertices, Buffer<Uint32> *pixels);
int edge(const Vec3f& a, const Vec3f& b, const Vec3f& c);
void rasterize(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels);
static const int WIDTH = 800;
static const int HEIGHT = 800;
const Vec3f light_dir = Vec3f(-1, 0, 0);
SDL_PixelFormat* mappingFormat(SDL_AllocFormat(SDL_PIXELFORMAT_RGB888));
Uint32 black = SDL_MapRGBA(mappingFormat, 0x00, 0x00, 0x00,0xFF);
Uint32 white = SDL_MapRGBA(mappingFormat, 0xff, 0xff, 0xff, 0xff);
Uint32 red = SDL_MapRGBA(mappingFormat, 0xff, 0x00, 0x00, 0xff);

//void drawLine(int x0, int x1, int y0, int y1, TGAColor color, TGAImage &image);
//void drawWireframe(Mesh&mesh, TGAColor color, TGAImage &image);
//const TGAColor white_t = TGAColor(255, 255, 255, 255);
//const TGAColor red_t = TGAColor(255, 0, 0, 255);

int main(int argc, char ** argv)
{
	bool quit = false;
	SDL_Event event;
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window * window = SDL_CreateWindow("SoftWare Renderer",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);

	SDL_Surface * surface = SDL_GetWindowSurface(window);
	Buffer<Uint32> *pixels = new Buffer<Uint32>(WIDTH, HEIGHT, new Uint32[WIDTH * HEIGHT*4]);
	pixels->clear();
	Mesh mesh;
	std::string  filename = "./testfile/african_head.obj";
	mesh = OBJ::buildMeshFromFile(mesh, filename);
	//Fill the surface black
	SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF));
	while (!quit)
	{
		
		SDL_WaitEvent(&event);
		
		switch (event.type)
		{
		case SDL_QUIT:
			quit = true;
			break;
		}
		//Allows surface editing
		SDL_LockSurface(surface);

		rasterize(mesh, white, pixels);

		//drawWireframe(mesh, red, pixels);
		//�ȼ����۸� surface �� ���� 4�� ���ϴ� ������ �ȼ����۴� ���������� ������ ����
		memcpy(surface->pixels, pixels->buffer, pixels->mHeight*pixels->mWidth*4);
		SDL_UnlockSurface(surface);

		//Update the surface
		SDL_UpdateWindowSurface(window);
	}

	delete pixels;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
/*
int main()
{
	TGAImage image(WIDTH, HEIGHT, TGAImage::RGB);
	std::string filename = "./testfile/african_head.obj";
	Mesh mesh; 
	mesh= OBJ::buildMeshFromFile(mesh, filename);

	drawWireframe(mesh, white,image);

	image.flip_vertically();
	image.write_tga_file("output.tga");

	return 0;
}
*/
void drawLine(int x0, int x1, int y0, int y1, Uint32 color, Buffer<Uint32> *pixels)
{
	//Bresenham�� ���׸��� �˰���
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
void rasterize(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels)
{
	
	for (int i = 0; i < mesh.vertexIndices.size(); i++)
	{
		Vec3f screenCoords[3];
		Vec3f worldCoords[3];
		Vec3i face = mesh.vertexIndices[i];
		for (int j = 0; j < 3; j++)
		{
			Vec3f world_coord = mesh.verts_[face.raw[j]];

			screenCoords[j] = Vec3f((world_coord.x + 1.f)*WIDTH / 2.f, (-world_coord.y + 1.f)*HEIGHT / 2.f,0);
			worldCoords[j] = world_coord;
		}
		Vec3f n;
		n = n.cross(worldCoords[2] - worldCoords[0], worldCoords[1] - worldCoords[0]);
		float intensity = n * light_dir;
		if(intensity>0)
			triangle(screenCoords, color, pixels);
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
			Vec3f v1 = mesh.verts_[face.raw[(j + 1) % 3]];

			int x0 = (v0.x + 1.) * WIDTH / 2;
			int x1 = (v1.x + 1.) * WIDTH / 2;
			int y0 = (-v0.y + 1.) * HEIGHT / 2;
			int y1 = (-v1.y + 1.) * HEIGHT / 2;
			drawLine(x0, x1, y0, y1, color, pixels);
		}
	}
}

void triangle(Vec3f *vertices, Uint32 color, Buffer<Uint32> *pixels)
{
	//bounding box�� ���
	int minX, minY, maxX, maxY;

	triBoundBox(maxX, maxY, minX, minY,vertices,pixels);

	//Triangle setup
	int A01 = vertices[0].y - vertices[1].y, B01 = vertices[1].x - vertices[0].x;
	int A12 = vertices[0].y - vertices[2].y, B12 = vertices[2].x - vertices[1].x;
	int A20 = vertices[0].y - vertices[2].y, B20 = vertices[2].x - vertices[0].x;

	//�����߽���ǥ �ּڰ����� ����
	Vec3f p(minX, minY, 0);

	int w0_row = edge(vertices[1], vertices[2], p);
	int w1_row = edge(vertices[2], vertices[0], p);
	int w2_row = edge(vertices[0], vertices[1], p);
	//Rasterize	

	for (int y = minY; y <= maxY; y++)
	{
		int w0 = w0_row;
		int w1 = w1_row;
		int w2 = w2_row;
		for (int x = minX; x <= maxX; x++)
		{
			if (w0 >= 0 && w1 >= 0 && w2 >= 0)
				(*pixels)(x, y) = color;

			//�������� �Ѱ��
			w0 += A12;
			w1 += A20;
			w2 += A01;
		}
		//�� �� ����
		w0_row += B12;
		w1_row += B20;
		w2_row += B01;

	}
}
//���а��
int edge(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
	return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

//�ﰢ���� ĥ�ϴ� ���ڸ� ����
void triBoundBox(int &maxX, int &maxY, int &minX, int &minY, Vec3f *vertices, Buffer<Uint32> *pixels)
{
	maxX = std::max({ vertices[0].x,vertices[1].x,vertices[2].x });
	maxY = std::max({ vertices[0].y,vertices[1].y,vertices[2].y });
	
	minX = std::min({ vertices[0].x,vertices[1].x,vertices[2].x });
	minY = std::min({ vertices[0].y,vertices[1].y,vertices[2].y });

	//ȭ�鿡���� Ŭ����
	maxX = std::min(maxX, pixels->mWidth - 1);
	minX = std::max(minX, 0);

	maxY = std::min(maxY, pixels->mHeight - 1);
	minY = std::max(minY, 0);

	
}

