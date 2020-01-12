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
	BaseModel(const char *cfilename, glm::vec3 color);
	BaseModel(const std::vector<GLfloat> &, glm::vec3 color);
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
	std::vector<glm::vec3>* getVoxels() {
		return &voxelPos;
	}
	void initDepthBuffer();
	void voxelization();
	GLuint getStep() {
		return step;
	}
protected:
	std::vector<GLfloat> vertices;
	std::vector<int> indices;
	GLuint VAO, VBO, EBO;
	glm::vec3 color;
	// voxel
	GLfloat xMax = -10000.0f, xMin = 10000.0f, yMax = -10000.0f, yMin = 10000.0f, zMax = -10000.0f, zMin = 10000.0f;
	const GLfloat step = 5;
	GLuint m_cntBuffer;
	std::vector<glm::vec3> voxelPos;
};

#endif