#ifndef _ObJ_PARSER_H_
#define _ObJ_PARSER_H_
#pragma once

#include <vector>
#include "tgaimage.h"
#include "geometry.h"
class Mesh {
public:
	Mesh() {};
	~Mesh();
	std::vector<Vec3f> verts_;
	std::vector<Vec3f> texts_;
	std::vector<Vec3f> normals_;
	std::vector<Vec3i> vertexIndices;
	std::vector<Vec3i> textureIndices;
	std::vector<Vec3i> normalsIndices;

	TGAImage textureMap; 
	void loadTexture(const char* filename);
	Vec3f uv(int index, int nvert);
};

#include <fstream>
#include <sstream>
namespace OBJ {
	Mesh& buildMeshFromFile(Mesh& mesh, std::string &filename);
	bool fileExist(std::string &filename);
	void loadFileData(Mesh& mesh, std::ifstream &file);
	std::vector<std::string> splitStr(std::string &str, char delim);
};

#endif


