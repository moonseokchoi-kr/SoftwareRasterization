#include <vector>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <limits>
#include <string>
#include "tgaimage.h"
#include "ObjParser.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red = TGAColor(255, 0, 0, 255);
Mesh mesh;
const int width = 800;
const int height = 800;

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
	return Vec3f(int((v.x + 1.)*width / 2.f + .5f), int((v.y + 1.)*height / 2.f + .5f), v.z);
}

/**
 * triangle
 *
 * use the barycentric conspiracy
 *
 * 1. 삼각형의 끝점을 내부에 두는 상자를 계산합니다.
 * 2. 상자의 x의 최솟값, y의 최솟값에서 시작하여 주어진 삼각형에 대한 점P의 무게중심을 찾습니다.
 * 3. 그 값중 하나라도 0이 있다면 패스합니다
 * 4. 아닐경우 zbuffer값을 계산하여 zbuffer에 저장한후 픽셀에 색을 칠합니다.
 */
void triangle(Vec3f *pts, float *zbuffer, TGAImage &image, TGAColor color) {
	Vec2f bboxmin(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
	Vec2f clamp(image.get_width() - 1, image.get_height() - 1);
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
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) continue;
			P.z = 0;
			for (int i = 0; i < 3; i++) P.z += pts[i][2] * bc_screen[i];
			if (zbuffer[int(P.x + P.y*width)] < P.z) {
				zbuffer[int(P.x + P.y*width)] = P.z;
				image.set(P.x, P.y, color);
			}
		}
	}
}

/**
 * rasterize
 *
 * 1. calculate coords
 * 2. draw triangle
 */
void rasterize(Mesh& mesh, TGAColor color, TGAImage &image, float *zBuffer)
{
	Vec3f light_dir(-1.f, 0.f, 0.f);
	for (int i = 0; i < mesh.vertexIndices.size(); i++)
	{
		Vec3f screenCoords[3];
		Vec3i face = mesh.vertexIndices[i];
		//Vec3f normal = mesh.normalsIndices[i];
		for (int j = 0; j < 3; j++) { screenCoords[j] = world2screen(mesh.verts_[face[j]]);}
		
		triangle(screenCoords,zBuffer,image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}
}


/**
 * texture
 * 1. load texture image file
 * 2. calculate coords
 * 3. save the color
 */








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
	

	float *zbuffer = new float[width*height];
	for (int i = width * height; i--; zbuffer[i] = -std::numeric_limits<float>::max());

	TGAImage image(width, height, TGAImage::RGB);
	/*for (int i = 0; i < mesh.vertexIndices.size(); i++) {
		Vec3i face = mesh.vertexIndices[i];
		Vec3f pts[3];
		for (int i = 0; i < 3; i++) pts[i] = world2screen(mesh.verts_[face[i]]);
		triangle(pts, zbuffer, image, TGAColor(rand() % 255, rand() % 255, rand() % 255, 255));
	}*/
	rasterize(mesh, white, image, zbuffer);

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	return 0;
}