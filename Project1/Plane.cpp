#include <vector>
#include "Plane.h"
Plane::Plane(const std::vector<GLfloat> &vertices, int colorID, PrimitiveType type):BaseModel(vertices, colorID, type)
{

}

Plane::~Plane()
{
}

void Plane::initVertexObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(0); // Unbind VAO
}
