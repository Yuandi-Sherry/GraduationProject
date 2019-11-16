#include "CrossSectionPlane.h"
#include <iostream>
CrossSectionPlane::CrossSectionPlane()
{
	this->coeff = glm::vec4(1, 1, 1, 1);
}

CrossSectionPlane::~CrossSectionPlane()
{
}

void CrossSectionPlane::initVertexObject() {
	//std::cout << "square init " << std::endl;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	/*glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);*/
}

void CrossSectionPlane::setCoeff(glm::vec4 newCoeff) {
	//std::cout << "newCoeff " << newCoeff.x << " " << newCoeff.y << " " << newCoeff.z << " " << newCoeff.w << std::endl;
	this->coeff = newCoeff;
}
void CrossSectionPlane::draw() {
	calVertices();
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CrossSectionPlane::calVertices() {

	for (int i = 0; i < 4; i++) {
		vertices[i * 3 + 2] = (-coeff[3] - coeff[0] * vertices[i * 3] - coeff[1] * vertices[i * 3 + 1]) / coeff[2];
		std::cout << vertices[i * 3 + 2] << " ";
	}
	std::cout << std::endl;

}