#include "Line.h"
Line::Line(const std::vector<GLfloat> &vertices, glm::vec3 color, PrimitiveType type) : BaseModel(vertices, color, type)
{
}

Line:: ~Line()
{
}

GLfloat Line::getDistance() {
	return sqrt(pow((*getVertices())[0] - (*getVertices())[3], 2) + pow((*getVertices())[1] - (*getVertices())[4], 2) + pow((*getVertices())[2] - (*getVertices())[5], 2));
}

void Line::initVertexObject() {
	
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