#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <string>
#include <SDL.h>
#include "buffer.h"
#include "tgaimage.h"
#include "ObjParser.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Mesh mesh;
float *zbuffer = NULL;
const int width = 800;
const int height = 800;
int depth = 255;


//Vec2f calculateCorrectUV(Vec3f*screenCoords, Vec3f *textCoords, Vec3f p);
//TGAColor interpolationTriangle(Vec3f* screenCoords, Vec3f *textCoords, Vec3f p, TGAImage& texture
/*
* 행렬을 벡터로 변환
*/
Vec3f m2v(Matrix m)
{
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);

}

/*
* 벡터를 행렬로
*/
Matrix v2m(Vec3f v) {
	Matrix m;
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}
Matrix viewport(int x, int y, int w, int h) {
	Matrix m = Matrix::identity();
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;

	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

/**
 * projectionMatrix생성
 */
Matrix projectionMatrix(float fov, float AR, float n, float f) {
	Matrix m;

	float tanHalfFOVInverse = 1 / std::tan((fov / 2)*(MY_PI / 180));

	m[0][0] = tanHalfFOVInverse;

	m[1][1] = AR * tanHalfFOVInverse;

	m[2][2] = n / (f - n);

	m[2][3] = (f*n) / (f - n);

	m[3][2] = -1;
	return m;
}
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) {
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

	for (int x = x0; x <= x1; x++) {
		float t = (x - x0) / (float)(x1 - x0);
		int y = y0 * (1. - t) + y1 * t;
		if (steep) {
			image.set(y, x, color);
		}
		else {
			image.set(x, y, color);
		}
	}
}
//삼각형과 한점의 무게중심을 찾습니다.
// https://fgiesen.wordpress.com/2013/02/06/the-barycentric-conspirac/
Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P) {
	Vec3f s[2];
	for (int i = 2; i--; ) {
		s[i][0] = C[i] - A[i];
		s[i][1] = B[i] - A[i];
		s[i][2] = A[i] - P[i];
	}
	Vec3f u = cross(s[0], s[1]);
	if (std::abs(u[2]) > 1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
		return Vec3f(1.f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	return Vec3f(-1, 1, 1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

//월드좌표를 스크린좌표로 클리핑합니다.
Vec3f world2screen(Vec3f v) {
	return Vec3f(int((v.x + 1.f)*width / 2.f + .5f), int((v.y + 1.f)*height / 2.f + .5f), v.z);
}

/**
 * triangle
 *
 * use the barycentric conspiracy
 *
 * 1. 삼각형의 끝점을 내부에 두는 상자를 계산합니다.
 * 2. 상자의 x의 최솟값, y의 최솟값에서 시작하여 주어진 삼각형에 대한 점P의 무게중심을 찾습니다.
 * 3. 그 값중 하나라도 0이 있다면 패스합니다
 * 4. 아닐경우 zbuffer값을 계산하여 zbuffer에 저장한후 zbuffer에 따라 픽셀에 색을 칠합니다.
 */
void triangle(Vec3f *pts, Vec3f *textureCoord, float *zbuffer, Buffer<Uint32> *buffer, Mesh& mesh, float intensity) {
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(buffer->mWidth - 1, buffer->mHeight - 1);
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			bboxmin[j] = std::max(0.f, std::min(bboxmin[j], pts[i][j]));
			bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
		}
	}
	Vec3f P;
	for (P.x = bboxmin.x; P.x <= bboxmax.x; P.x++) {
		for (P.y = bboxmin.y; P.y <= bboxmax.y; P.y++) {
			Vec3f bc_screen = barycentric(pts[0], pts[1], pts[2], P);
			Vec3f correctUV = textureCoord[0] * bc_screen.x + textureCoord[1] * bc_screen.y + textureCoord[2] * bc_screen.z;
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y*width)] < P.z) {
				zbuffer[int(P.x + P.y*width)] = P.z;
				//texture mapping
				TGAColor temp(mesh.textureMap.get(correctUV.x,correctUV.y));
				TGAColor finalColor(temp.r*intensity, temp.g*intensity, temp.b*intensity);
				//TGAColor finalColor(intensity * 255, intensity * 255, intensity * 255);
				(*buffer)(P.x,P.y) = finalColor.val;
			}
		}
	}
}

/*
//인접한세정점 좌표와의 거리에 비례하여 값을 혼합하는것
Vec3f BarycentricCoordinates(Vec3f*screenCoords, Vec3f p)
{
	float baryA = (textCoords[1].y-textCoords[2].y)*(p.x-textCoords[2].x)+(textCoords[2].x - textCoords[1].x)*(p.y - textCoords[2].y)/
		(textCoords[1].y - textCoords[2].y)*(textCoords[0].x - textCoords[2].x) + (textCoords[2].x - textCoords[1].x)*(textCoords[0].y - textCoords[2].y);
	float baryB = (textCoords[2].y - textCoords[0].y)*(p.x - textCoords[2].x) + (textCoords[0].x - textCoords[2].x)*(p.y - textCoords[2].y) /
		(textCoords[1].y - textCoords[2].y)*(textCoords[0].x - textCoords[2].x) + (textCoords[2].x - textCoords[1].x)*(textCoords[0].y - textCoords[2].y);
	float baryC = 1 - baryA - baryB;

	Vec3f correctUV = textCoords[0]* baryA +  textCoords[1] * baryB +  textCoords[2]* baryC;

	return Vec3f(baryA, baryB, baryC);
}
*/

/**
 * rasterize
 *
 * 1. calculate coordinate
 * 2. draw triangle
 * 3. texture on image
 */
void rasterize(Mesh& mesh, Buffer<Uint32> *buffer,  TGAColor color,float *zBuffer)
{
	Vec3f light_dir(0, 0, -1);
	//Matrix projection = projectionMatrix(50, 0.1, 100, width / (float)height);
	Matrix projection = Matrix::identity();
	Matrix viewPort = viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	Vec3f camera(0, 0, 3);
	projection[3][2] = -1.f / camera.z;

	for (int i = 0; i < mesh.vertexIndices.size(); i++)
	{
		//vertex
		Vec3f screenCoords[3];
		Vec3i face = mesh.vertexIndices[i];
		Vec3f worldCoords[3];
		//texture
		Vec3f textCoords[3];
		//Vec3f normal = mesh.normalsIndices[i];
		for (int j = 0; j < 3; j++) 
		{ 
			worldCoords[j] = mesh.verts_[face[j]];
			
			//screenCoords[j] = m2v(ViewPort*Projection*v2m(worldCoords[j]));
			screenCoords[j] = m2v(viewPort*v2m(worldCoords[j]));
			
			
		}

		Vec3f n = cross(worldCoords[2] - worldCoords[0], worldCoords[1] - worldCoords[0]);
		n.normalize();
		float intensity = n * light_dir;
		if (intensity > 0) {
			for(int j=0; j<3; j++)
				textCoords[j] = mesh.uv(i,j);
			triangle(screenCoords, textCoords, zBuffer, buffer, mesh, intensity);
		}
			
		//setTexture(screenCoords, textCoords, image, texture);
	}
}



 /**
  * rendered object
  *
  *
  * 1. read obj file
  * 2. clear zbuffer
  * 3. rasterize
  * 4. plotting the object
  */
int main(int argc, char** argv) {
	
	std::string filename = "./testfile/african_head.obj";
	mesh = OBJ::buildMeshFromFile(mesh,filename);
	

	zbuffer = new float[width*height];
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	Buffer<Uint32> *pixels = new Buffer<Uint32>(width, height, new Uint32[width * height * 4]);
	
	pixels->clear();

	//TGAImage image(width, height, TGAImage::RGB);

	mesh.loadTexture("./testfile/african_head_diffuse.tga");

	//SDL2 Setting
	bool quit = false;
	SDL_Event event;
	SDL_Init(SDL_INIT_VIDEO);

	SDL_Window * window = SDL_CreateWindow("SoftWare Renderer",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);

	SDL_Surface * surface = SDL_GetWindowSurface(window);

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

		rasterize(mesh, pixels,  white, zbuffer);

		//픽셀버퍼를 surface 로 복사 4를 곱하는 이유는 픽셀버퍼는 색상포맷의 영향을 받음
		memcpy(surface->pixels, pixels->buffer, pixels->mHeight*pixels->mWidth * 4);
		SDL_UnlockSurface(surface);

		//Update the surface
		SDL_UpdateWindowSurface(window);
	}

	delete pixels;

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;



	
	/*for (int i = 0; i < mesh.vertexIndices.size(); i++) {
		Vec3i face = mesh.vertexIndices[i];
		Vec3f pts[3];
		for (int i = 0; i < 3; i++) pts[i] = world2screen(mesh.verts_[face[i]]);
		triangle(pts, zbuffer, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}*/


	//image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	//image.write_tga_file("output.tga");
	return 0;
}