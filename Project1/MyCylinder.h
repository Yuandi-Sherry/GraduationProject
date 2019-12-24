#ifndef _MyCylinder_H_
#define _MyCylinder_H_
#include <vector>
#include <glad/glad.h>
#include <cmath>
#define PI 3.14
class MyCylinder
{
public:
	MyCylinder() {
		sidecount = 40; 
		radius = 2;
		length = 2000;
	}
	~MyCylinder() {

	}
	void initVertexObject() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	void draw() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
		glBindVertexArray(0);
	}
	void generateVertices() {
		// generate two circles
		for (int i = 0; i <= sidecount; i++) {
			vertices.push_back(radius * sin((2 * PI * i / sidecount))); //x
			vertices.push_back(length / 2); //y
			vertices.push_back(radius * cos((2 * PI * i / sidecount))); //z
			// normal
			vertices.push_back(0); //x
			vertices.push_back(1); //y
			vertices.push_back(0); //z

			vertices.push_back(radius * sin((2 * PI * (i + 1) / sidecount))); //x
			vertices.push_back(length / 2); //y
			vertices.push_back(radius * cos((2 * PI * (i + 1) / sidecount))); //z
			// normal
			vertices.push_back(0); //x
			vertices.push_back(1); //y
			vertices.push_back(0); //z

			vertices.push_back(0); //x
			vertices.push_back(length / 2); //y
			vertices.push_back(0); //z
			// normal
			vertices.push_back(0); //x
			vertices.push_back(1); //y
			vertices.push_back(0); //z

		}
		for (int i = 0; i <= sidecount; i++) {
			vertices.push_back(radius * sin((2 * PI * i / sidecount))); //x
			vertices.push_back(-length / 2); //y
			vertices.push_back(radius * cos((2 * PI * i / sidecount))); //z
			
			vertices.push_back(0); //x
			vertices.push_back(-1); //y
			vertices.push_back(0); //z

			vertices.push_back(radius * sin((2 * PI * (i + 1) / sidecount))); //x
			vertices.push_back(-length / 2); //y
			vertices.push_back(radius * cos((2 * PI * (i + 1) / sidecount))); //z

			vertices.push_back(0); //x
			vertices.push_back(-1); //y
			vertices.push_back(0); //z

			vertices.push_back(0); //x
			vertices.push_back(-length / 2); //y
			vertices.push_back(0); //z

			vertices.push_back(0); //x
			vertices.push_back(-1); //y
			vertices.push_back(0); //z
		}
		// roll
		glm::vec3 normal, vec1, vec2;
		for (int i = 0; i <= sidecount; i++) {
			// 6 point
			vec1 = glm::vec3(radius * sin((2 * PI * i / sidecount)) - radius * sin((2 * PI * i / sidecount)), length / 2 -(-length / 2), radius * cos((2 * PI * i / sidecount)) - radius * cos((2 * PI * i / sidecount)));
			vec2 = glm::vec3(radius * sin((2 * PI * i / sidecount)) - radius * sin((2 * PI * (i + 1) / sidecount)), length / 2 - length / 2, radius * cos((2 * PI * i / sidecount)) - radius * cos((2 * PI * (i + 1) / sidecount)));
			normal = glm::normalize(glm::cross(vec1, vec2));
			// 
			// left top
			vertices.push_back(radius * sin((2 * PI * i / sidecount))); //x
			vertices.push_back(length / 2); //y
			vertices.push_back(radius * cos((2 * PI * i / sidecount))); //z
			// 
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
			// left button
			vertices.push_back(radius * sin((2 * PI * i / sidecount))); //x
			vertices.push_back(-length / 2); //y
			vertices.push_back(radius * cos((2 * PI * i / sidecount))); //z

			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
			// right top
			vertices.push_back(radius * sin((2 * PI * (i + 1) / sidecount))); //x
			vertices.push_back(length / 2); //y
			vertices.push_back(radius * cos((2 * PI * (i + 1) / sidecount))); //z

			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
			// left button
			vertices.push_back(radius * sin((2 * PI * i / sidecount))); //x
			vertices.push_back(-length / 2); //y
			vertices.push_back(radius * cos((2 * PI * i / sidecount))); //z
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
			// right top
			vertices.push_back(radius * sin((2 * PI * (i + 1) / sidecount))); //x
			vertices.push_back(length / 2); //y
			vertices.push_back(radius * cos((2 * PI * (i + 1) / sidecount))); //z
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
			// right button
			vertices.push_back(radius * sin((2 * PI * (i + 1) / sidecount))); //x
			vertices.push_back(-length / 2); //y
			vertices.push_back(radius * cos((2 * PI * (i + 1) / sidecount))); //z
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
		}
		std::cout << "vertices.szie()" << vertices.size() << std::endl;
	}

	std::vector<GLfloat> getVertices() {
		return vertices;
	}
private:
	std::vector<GLfloat> vertices;
	int sidecount; // to approach a circle
	int radius;
	float length;
	GLuint VAO, VBO;

};

#endif // !_MyCylinder_H_
