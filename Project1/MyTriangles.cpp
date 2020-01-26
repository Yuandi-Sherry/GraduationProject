#include "MyTriangles.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <glad/glad.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "utils.h"
MyTriangles::MyTriangles(const std::vector<GLfloat>& vertices, glm::vec3 color) {
	this->vertices.assign(vertices.begin(), vertices.end());
	this->color - color;
}

MyTriangles::~MyTriangles()
{
	//delete [] markVoxel;
}
void MyTriangles::initVertexObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0); // Unbind VAO
}

void MyTriangles::draw() {
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size()/3);
	glBindVertexArray(0);
}
