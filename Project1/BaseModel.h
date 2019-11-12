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
	void initVertexObject() {
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
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(1);
			break;
		case LINE:
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
			glEnableVertexAttribArray(0);
		}
		glBindVertexArray(0); // Unbind VAO
	}

	void draw() {
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

	GLint getcolorID() {
		return colorID;
	}
private:
	std::vector<GLfloat> vertices;
	std::vector<GLint> indices;
	GLuint VAO, VBO;
	GLint colorID;
	PrimitiveType type;
};

#endif