#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"

extern const GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
class BaseModel
{
public:
	static glm::vec3 modelCenter;
	BaseModel(const char *cfilename, glm::vec3 color);
	BaseModel(const std::vector<GLfloat> &, glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f));
	BaseModel() {
	}
	~BaseModel();
	std::vector<GLfloat>* getVertices() {
		return &vertices;
	}
	void setVertices(const std::vector<GLfloat> vers) {
		this->vertices.assign(vers.begin(), vers.end());
	}
	std::vector<int>* getIndices() {
		return &indices;
	}
	void setIndices(const std::vector<int> ind) {
		this->indices.assign(ind.begin(), ind.end());
	}
	void initVertexObject();

	void draw();
	glm::vec3 getColor() {
		return color;
	}
	std::vector<glm::vec3>* getVoxelsPos() {
		return &voxelPos;
	}
	std::vector<glm::vec3>* getVoxelsIndex() {
		return &voxelIndex;
	}
	glm::vec3 getResolution() {
		return resolution;
	}
	void initDepthBuffer();
	void voxelization();
	GLuint getStep() {
		return step;
	}
	int* markVoxel; // 0->挖去的，1-> 原有的，2->挖去位置周围的
	glm::vec3 boxMin;
protected:
	std::vector<GLfloat> vertices;
	std::vector<int> indices;
	GLuint VAO, VBO, EBO;
	glm::vec3 color;
	// voxel
	GLfloat xMax = -10000.0f, xMin = 10000.0f, yMax = -10000.0f, yMin = 10000.0f, zMax = -10000.0f, zMin = 10000.0f;
	const GLfloat step = 3;
	GLuint m_cntBuffer;
	std::vector<glm::vec3> voxelPos;
	std::vector<glm::vec3> voxelIndex;
	glm::vec3 resolution;
	
};

#endif