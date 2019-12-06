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
	if (triangles == 0)
		return;
	// read triangle mesh in the loop
	glm::vec3 vector1, vector2, normal, tmpTriangleEdge[3];
	float texture[6] = {
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f
	};
	for (int i = 0; i < triangles; i++)
	{
		float coorXYZ[12];
		in.read((char*)coorXYZ, 12 * sizeof(float));

		for (int j = 1; j < 4; j++) // 三角形三个点的x,y,z
		{
			tmpTriangleEdge[j - 1].x = coorXYZ[j * 3];
			tmpTriangleEdge[j - 1].y = coorXYZ[j * 3 + 1];
			tmpTriangleEdge[j - 1].z = coorXYZ[j * 3 + 2];
		}
		// 计算法向量
		vector1 = tmpTriangleEdge[0] - tmpTriangleEdge[1];
		vector2 = tmpTriangleEdge[0] - tmpTriangleEdge[2];
		normal = glm::normalize(glm::cross(vector1, vector2));
		// 存入vertices
		for (int j = 0; j < 3; j++) {
			vertices.push_back(tmpTriangleEdge[j].x);
			vertices.push_back(tmpTriangleEdge[j].y);
			vertices.push_back(tmpTriangleEdge[j].z);
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
			//vertices.push_back(texture[j*2]);
			//vertices.push_back(texture[j * 2 + 1]);
		}
		in.read((char*)coorXYZ, 2);
	}
	in.close();

	
}

BaseModel::BaseModel(const std::vector<GLfloat> &vertices, int colorID, PrimitiveType type) {
	this->vertices.assign(vertices.begin(), vertices.end());
	this->type = type;
}
BaseModel::~BaseModel()
{
}
void BaseModel::initVertexObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

	// Position attribute
	switch (type) {
	case TRIANGLE:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 8* sizeof(GLfloat), (GLvoid*)0);
		//glEnableVertexAttribArray(2);
		break;
	case LINE:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0); // Unbind VAO
	
}

void BaseModel::initDepthBuffer() {
	
}

void BaseModel::draw(){
	glBindVertexArray(VAO);
	switch (type) {
	case TRIANGLE:
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
		break;
	case LINE:
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		break;
	}

	glBindVertexArray(0);
}