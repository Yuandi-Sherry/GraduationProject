#ifndef _CS_PLANE_
#define _CS_PLANE_
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class CrossSectionPlane
{
public:
	CrossSectionPlane();
	~CrossSectionPlane();
	void calVertices();
	void setCoeff(glm::vec4 newCoeff);
	void initVertexObject();
	void draw();
private:
	GLuint VAO, VBO, EBO;
	glm::vec4 coeff;
	GLfloat vertices[12] = {
		 100.0f + 4.38f,  100.0f + 201.899f, -200.0f,// top right
		 100.0f + 4.38f, -100.0f + 201.899f, 0.0f,// bottom right
		-100.0f + 4.38f, -100.0f + 201.899f, 200.0f,// bottom left
		-100.0f + 4.38f,  100.0f + 201.899f, 0.0f
	};
	//-4.38f, -201.899f,
	GLuint indices[6] = {
		 0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
};


#endif