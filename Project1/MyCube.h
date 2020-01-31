#ifndef _MYCUBE_H_
#define _MYCUBE_H_
#include <iostream>
// glad
#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class MyCube
{
public:
	MyCube();
	~MyCube();
	void initVertexObject();
	void draw();
	glm::vec3 Position = glm::vec3(0.0f, 0.0f, 200.0f);
private:
	GLuint VAO, VBO;
	GLfloat vertices[216] = {
	   -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
		1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
	   -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f,
	   -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f,

	   -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
		1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	   -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	   -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,

	   -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f,

		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
		1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,
		1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,
		1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f,

	   -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
		1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
		1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
	   -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f,
	   -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f,

	   -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
		1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	   -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	   -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f
	};

};
#endif