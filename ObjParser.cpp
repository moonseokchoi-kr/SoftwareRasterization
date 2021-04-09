#include "ObjParser.h"
#include <iostream>
Mesh::~Mesh()
{
}


Mesh & OBJ::buildMeshFromFile(Mesh & mesh, std::string &filename)
{
	//파일이 존재하는지 검사
	if (fileExist(filename))
	{
		std::ifstream file;
		file.open(filename.c_str());

		//파일에 있는 점의 정보들을 가져옴
		loadFileData(mesh, file);

		//파일 닫기
		file.close();
	}
	return mesh;
}

bool OBJ::fileExist(std::string &filename)
{
	std::ifstream file(filename.c_str());
	return file.good();
}

void OBJ::loadFileData(Mesh& mesh, std::ifstream &file)
{
	std::string line, key, x, y, z;
	//float tempU, tempV, intpart;
	Vec3i indices[3];
	char delimeter = '/';

	while (!file.eof())
	{
		std::getline(file, line);
		std::istringstream iss(line);
		iss >> key;
		if (key == "v")
		{
			iss >> x >> y >> z;
			Vec3f vertex(std::stof(x), std::stof(y), std::stof(z));
			mesh.verts_.push_back(vertex);
		}
		else if (key == "vt")
		{
			iss >> x >> y;
			Vec3f text(std::stof(x), std::stof(y), 0);
	 		mesh.texts_.push_back(text);
		}
		else if (key == "vn")
		{
			iss >> x >> y >> z;
			Vec3f normal(std::stof(x), std::stof(y), std::stof(z));
			mesh.normals_.push_back(normal);
		}
		else if (key == "f")
		{
			
			//obj 파일에 face는 '/'로 구분되어있어 분리해주는 작업을 해야함
			iss >> x >> y >> z;

			std::vector<std::string> splitX = splitStr(x, delimeter);
			std::vector<std::string> splitY = splitStr(y, delimeter);
			std::vector<std::string> splitZ = splitStr(z, delimeter);
			
			for (int i = 0; i < splitX.size(); i++)
			{
				//f의 경우 시작이 1 이기 때문에 통일을 위해서 빼줘야함
				indices[i] = Vec3i(std::stoi(splitX[i])-1, std::stoi(splitY[i])-1, std::stoi(splitZ[i])-1);
			}

			mesh.vertexIndices.push_back(indices[0]);
			mesh.textureIndices.push_back(indices[1]);
			mesh.normalsIndices.push_back(indices[2]);
			
		}
	}
	std::cerr << "# v# " << mesh.verts_.size() << " f# " << mesh.vertexIndices.size() << " t# " << mesh.texts_.size()<< std::endl;
}

std::vector<std::string> OBJ::splitStr(std::string &str, char delim)
{
	std::stringstream ss(str);
	std::string token;
	std::vector<std::string> splitString;
	while (std::getline(ss, token, delim)) {
		if (token == "") {
			splitString.push_back("0");
		}
		else {
			splitString.push_back(token);
		}
	}
	return splitString;
}
