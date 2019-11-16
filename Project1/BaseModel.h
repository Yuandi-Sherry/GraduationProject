#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <vector>
#include <iostream>
#include <glad/glad.h>

enum PrimitiveType {
	TRIANGLE,
	LINE
};
class BaseModel
{
public:
	BaseModel(const char *cfilename, int colorID, PrimitiveType type);
	BaseModel(const std::vector<GLfloat> &, int colorID, PrimitiveType type);
	BaseModel(PrimitiveType type) {
		this->type = type;
	}
	~BaseModel();
	std::vector<GLfloat>* getVertices() {
		return &vertices;
	}
	void initVertexObject();

	void draw();

	GLint getcolorID() {
		return colorID;
	}
protected:
	std::vector<GLfloat> vertices;
	std::vector<GLint> indices;
	GLuint VAO, VBO;
	GLint colorID;
	PrimitiveType type;
};

#endif