#ifndef _MYSPHERE_H_
#define _MYSPHERE_H_
#include <vector>
#include <glad/glad.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define PI 3.1415926
class MySphere
{
public:
	MySphere();
	~MySphere();
	void initVertexObject();
	void draw();
	std::vector<GLfloat>* getVertices() {
		return &vertices;
	}

	void setVertices(const std::vector<GLfloat> vertices) {
		this->vertices.assign(vertices.begin(), vertices.end());
	}
	std::vector<int>* getIndices() {
		return &indices;
	}
	void setIndices(const std::vector<int> indices) {
		this->indices.assign(indices.begin(), indices.end());
	}
private:
	void generateVertices();
	std::vector<GLfloat> vertices;
	std::vector<int> indices;
    glm::vec3 calNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	
	GLuint VAO, VBO, EBO;
};

#endif