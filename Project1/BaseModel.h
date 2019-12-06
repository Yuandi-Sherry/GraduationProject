#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <vector>
#include <iostream>
#include <glad/glad.h>

enum PrimitiveType {
	TRIANGLE,
	LINE
};
extern const GLuint SHADOW_WIDTH, SHADOW_HEIGHT;
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

	void initDepthBuffer();

protected:
	std::vector<GLfloat> vertices;
	GLuint VAO, VBO;
	GLint colorID;
	PrimitiveType type;
};

#endif