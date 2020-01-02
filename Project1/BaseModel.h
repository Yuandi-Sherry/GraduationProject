#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


enum PrimitiveType {
	TRIANGLE,
	LINE
};
extern const GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
class BaseModel
{
public:
	BaseModel(const char *cfilename, glm::vec3 color, PrimitiveType type);
	BaseModel(const std::vector<GLfloat> &, glm::vec3 color, PrimitiveType type);
	BaseModel(PrimitiveType type) {
		this->type = type;
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

	void initDepthBuffer();

protected:
	std::vector<GLfloat> vertices;
	std::vector<int> indices;
	GLuint VAO, VBO, EBO;
	glm::vec3 color;
	PrimitiveType type;
};

#endif