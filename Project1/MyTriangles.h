#ifndef _MY_TRIANGLES_H_
#define _MY_TRIANGLES_H_
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <glad/glad.h>

class MyTriangles
{
public:
	MyTriangles(const std::vector<GLfloat>& vertices, glm::vec3 color = glm::vec3(0.0f,0.0f,0.0f));
	~MyTriangles();
	void initVertexObject();
	void draw();
private:
	std::vector<GLfloat> vertices;
	GLuint VAO, VBO;
	glm::vec3 color;
};


#endif