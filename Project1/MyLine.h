#ifndef _MY_LINE_H_
#define _MY_LINE_H_
#include <iostream>
#include <vector>
// glad
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class MyLine
{
public:
	MyLine(std::vector<GLfloat> &inVec);
	MyLine();
	~MyLine();
	void initVertexObject();
	void draw();
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 200.0f);
	void setVertices(std::vector<GLfloat> inVec);
	void displayLine(std::vector<GLfloat>& vec);
private:
	std::vector<GLfloat> vertices = {
		-0.5f, -0.5f, -10.0f,
		0.5f, -0.5f, -10.0f,
		-0.5f, -0.5f, -10.0f,
		0.0f, 0.5f, -10.0f,
		0.5f, -0.5f, -10.0f,
		0.0f, 0.5f, -10.0f
	};
	GLuint VAO, VBO;
};


#endif // !_MY_LINE_H_


