#include <iostream>
#include <SDL.h>
#include <cmath>
#include <algorithm>
#include "ObjParser.h"
#include "buffer.h"

//#include "lib/tgaimage.h"
void drawLine(int x0, int x1, int y0, int y1, Uint32 color, Buffer<Uint32> *pixels);
void drawWireframe(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels);
void triangle(Vec3f *vertices , Uint32 color, Buffer<Uint32> *pixels, Buffer<float> *zBuffer);
void triBoundBox(int &maxX, int &maxY, int &minX, int &minY, Vec3f *vertices, Buffer<Uint32> *pixels);
int edge(const Vec3f& a, const Vec3f& b, const Vec3f& c);
void rasterize(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels, Buffer<float> *zBuffer);
bool backFaceCulling(Vec3f *wordCoords, Vec3f &v);

Vec3f world2Screen(Vec3f v);
static const int WIDTH = 800;
static const int HEIGHT = 800;
Vec3f light_dir = Vec3f(-1, 0, 0);
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
	Buffer<float> *zBuffer = new Buffer<float>(WIDTH, HEIGHT, new float[WIDTH*HEIGHT]);
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

		rasterize(mesh, white, pixels, zBuffer);

		//drawWireframe(mesh, red, pixels);
		//픽셀버퍼를 surface 로 복사 4를 곱하는 이유는 픽셀버퍼는 색상포맷의 영향을 받음
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
void rasterize(Mesh& mesh, Uint32 color, Buffer<Uint32> *pixels, Buffer<float> *zBuffer)
{
	
	for (int i = 0; i < mesh.vertexIndices.size(); i++)
	{
		Vec3f screenCoords[3];
		Vec3i face = mesh.vertexIndices[i];
		Vec3f worldCoords[3];
		for (int j = 0; j < 3; j++) { screenCoords[j] = world2Screen(mesh.verts_[face.raw[j]]); worldCoords[j] = mesh.verts_[face.raw[j]]; }
		triangle(screenCoords, SDL_MapRGBA(mappingFormat, rand()/0xff, rand() / 0xff, rand() / 0xff, 0xff), pixels, zBuffer);
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


void triangle(Vec3f *vertices, Uint32 color, Buffer<Uint32> *pixels, Buffer<float> *zbuffer)
{
	//bounding box를 계산
	int minX, minY, maxX, maxY;

	triBoundBox(maxX, maxY, minX, minY,vertices,pixels);
	
	float area, depth;
	area= edge(vertices[0], vertices[1], vertices[2]);
	if (area <= 0);
	area = 1 / area;

	//Triangle setup
	Vec3f zVals(vertices[0].z, vertices[1].z, vertices[2].z);
	float A01 = vertices[0].y - vertices[1].y, B01 = vertices[1].x - vertices[0].x;
	float A12 = vertices[1].y - vertices[2].y, B12 = vertices[2].x - vertices[1].x;
	float A20 = vertices[2].y - vertices[0].y, B20 = vertices[0].x - vertices[2].x;

	//무게중심좌표 최솟값에서 시작
	Vec3f p(minX, minY, 0);

	//선분의 벡터
	Vec3f w, w_row;

	w_row.x = edge(vertices[1], vertices[2], p);
	w_row.y = edge(vertices[2], vertices[0], p);
	w_row.z = edge(vertices[0], vertices[1], p);
	
	//Rasterize	
	for (int y = minY; y <= maxY; y++)
	{
		w.x = w_row.x;
		w.y = w_row.y;
		w.z = w_row.z;
		for (int x = minX; x <= maxX; x++)
		{
			if (w.x >= 0 && w.y >= 0 && w.z >= 0) 
			{
				depth = (w*area)*(zVals);
				if ((*zbuffer)(x, y) < depth && depth <= 1.0)
				{
					(*zbuffer)(x, y) = depth;

					(*pixels)(x, y) = color;
				}
				
			}

				

			//우측으로 한계단
			w.x += A12;
			w.y += A20;
			w.z += A01;
		}
		//한 행 진행
		w_row.x += B12;
		w_row.y += B20;
		w_row.z += B01;

	}
}

bool backFaceCulling(Vec3f *worldCoords, Vec3f &light)
{
	Vec3f n;
	n.cross(worldCoords[2] - worldCoords[0], worldCoords[1] - worldCoords[0]);
	n.normalize();
	int intensity = n * light;
	return intensity > 0;
}
//월드좌표->스크린
Vec3f world2Screen(Vec3f v)
{
	return Vec3f(int((v.x + 1.f)*WIDTH / 2.f + .5f), int((-v.y + 1.f)*HEIGHT / 2.f + .5f), v.z);
}

//선분계산
int edge(const Vec3f& a, const Vec3f& b, const Vec3f& c)
{
	return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

//삼각형을 칠하는 상자를 설정
void triBoundBox(int &maxX, int &maxY, int &minX, int &minY, Vec3f *vertices, Buffer<Uint32> *pixels)
{
	maxX = std::max({ vertices[0].x,vertices[1].x,vertices[2].x });
	maxY = std::max({ vertices[0].y,vertices[1].y,vertices[2].y });
	
	minX = std::min({ vertices[0].x,vertices[1].x,vertices[2].x });
	minY = std::min({ vertices[0].y,vertices[1].y,vertices[2].y });

	//화면에대한 클리핑
	maxX = std::min(maxX, pixels->mWidth - 1);
	minX = std::max(minX, 0);

	maxY = std::min(maxY, pixels->mHeight - 1);
	minY = std::max(minY, 0);

	
}

