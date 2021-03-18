#include "Rasterizer.h"

void Rasterizer::drawLine(Vec3f & v0, Vec3f & v1, const Uint32 & color, Buffer<Uint32>* pixelBuffer)
{
	if (v0.x > v1.x) std::swap(v0, v1);

}

void Rasterizer::drawWireFrame(Vec3f * vertices, Buffer<Uint32>* pixelBuffer)
{
}

void Rasterizer::drawTri(Vec3f * vertices, Buffer<Uint32>* pixelShader, Buffer<float>* zBuffer)
{
}
X`