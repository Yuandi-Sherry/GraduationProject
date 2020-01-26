#include "MyLine.h"

MyLine::MyLine()
{
}


MyLine::MyLine(std::vector<GLfloat>& inVec)
{
	this->vertices.assign(inVec.begin(), inVec.end());
	initVertexObject();
}

MyLine::~MyLine()
{
}

void MyLine::setVertices(std::vector<GLfloat> inVec) {
	this->vertices.assign(inVec.begin(), inVec.end());
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0); // Unbind VAO

	std::cout << "vertices.size in MyLine " << vertices.size() << std::endl;
}

void MyLine::displayLine(std::vector<GLfloat>& vec) {
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	glBufferData(GL_ARRAY_BUFFER, vec.size() * sizeof(GLfloat), &(vec.front()), GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glDrawArrays(GL_LINE_STRIP, 0, 2); 
}


void MyLine::initVertexObject() {
	if (vertices.size() != 0) {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
	}
	

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0); // Unbind VAO

	

}
void MyLine::draw() {
	glBindVertexArray(VAO);
	// glDrawArrays(GL_LINE_LOOP, 0, vertices.size()/3);
	glDrawArrays(GL_LINE_STRIP, 0, vertices.size() / 3);
	glBindVertexArray(0);
}