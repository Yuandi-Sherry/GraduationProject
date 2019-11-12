#include "BaseModel.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <glad/glad.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
BaseModel::BaseModel(const char *cfilename, int colorID, PrimitiveType type)
{
	this->colorID = colorID;
	this->type = type;
	std::ifstream in(cfilename, std::ifstream::in | std::ifstream::binary);
	if (!in) {
		std::cout << "fail to open file " << cfilename << std::endl;
		return;
	}
	char str[80];
	in.read(str, 80);
	// record amount of triangles
	int triangles;
	in.read((char*)&triangles, sizeof(int));
	std::cout << "number of triangles in file is " << triangles << std::endl;
	if (triangles == 0)
		return;
	// read triangle mesh in the loop
	//triangles = 100;
	glm::vec3 vector1, vector2, normal, tmpTriangleEdge[3];
	for (int i = 0; i < triangles; i++)
	{
		float coorXYZ[12];
		in.read((char*)coorXYZ, 12 * sizeof(float));

		for (int j = 1; j < 4; j++)
		{
			tmpTriangleEdge[j - 1].x = coorXYZ[j * 3];

			tmpTriangleEdge[j - 1].y = coorXYZ[j * 3 + 1];

			tmpTriangleEdge[j - 1].z = coorXYZ[j * 3 + 2];


		}


		// 
		for (int j = 0; j < 3; j++) {
			vertices.push_back(tmpTriangleEdge[j].x);
			vertices.push_back(tmpTriangleEdge[j].y);
			vertices.push_back(tmpTriangleEdge[j].z);
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
		}
		in.read((char*)coorXYZ, 2);
	}
	in.close();

	vector1 = tmpTriangleEdge[0] - tmpTriangleEdge[1];
	vector2 = tmpTriangleEdge[1] - tmpTriangleEdge[2];
	normal = glm::cross(vector1, vector2);
}

BaseModel::BaseModel(const std::vector<GLfloat> &vertices, int colorID, PrimitiveType type) {
	this->vertices.assign(vertices.begin(), vertices.end());
	this->type = type;
}
BaseModel::~BaseModel()
{
}