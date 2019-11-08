#ifndef BASEMODEL_H
#define BASEMODEL_H

#include <vector>
#include <iostream>
#include <glad/glad.h>

#define DEBUGMODE 0
class BaseModel
{
public:
	BaseModel(const char *cfilename, int color);
	~BaseModel();
	std::vector<GLfloat>* getVertices() {
		return &vertices;
	}

	std::vector<GLint>* getIndices() {
		return &indices;
	}
	void initVertexObject() {
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

		// Position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0); // Unbind VAO
	}

	void draw() {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
		glBindVertexArray(0);
	}

	GLint getColor() {
		return color;
	}
private:
	std::vector<GLfloat> vertices;
	std::vector<GLint> indices;
	GLuint VAO, VBO;
	GLint color;
};


BaseModel::BaseModel(const char *cfilename, int color)
{
	this->color = color;
	// GLfloat maxX = -1000.0f, maxY = -1000.0f, maxZ = -1000.0f, minX = 1000.0f, minY = 1000.0f, minZ = 1000.0f;

	if (!DEBUGMODE) {
		std::ifstream in(cfilename, std::ifstream::in | std::ifstream::binary);
		if (!in) {
			std::cout << "fail to open file " << cfilename << std::endl;
			return;
		}
		char str[80];
		in.read(str, 80);
		// record amount of triangles
		int triangles;
		in.read((char*)&triangles, sizeof(int));
		std::cout << "number of triangles in file is " << triangles << std::endl;
		if (triangles == 0)
			return;
		// read triangle mesh in the loop
		//triangles = 100;
		glm::vec3 vector1, vector2, normal, tmpTriangleEdge[3];
		for (int i = 0; i < triangles; i++)
		{
			float coorXYZ[12];
			in.read((char*)coorXYZ, 12 * sizeof(float));

			for (int j = 1; j < 4; j++)
			{
				tmpTriangleEdge[j - 1].x = coorXYZ[j * 3];

				tmpTriangleEdge[j - 1].y = coorXYZ[j * 3 + 1];

				tmpTriangleEdge[j - 1].z = coorXYZ[j * 3 + 2];


			}

			
			// 
			for (int j = 0; j < 3; j++) {
				vertices.push_back(tmpTriangleEdge[j].x);
				vertices.push_back(tmpTriangleEdge[j].y);
				vertices.push_back(tmpTriangleEdge[j].z);
				vertices.push_back(normal.x);
				vertices.push_back(normal.y);
				vertices.push_back(normal.z);
			}
			in.read((char*)coorXYZ, 2);
		}
		in.close();

		vector1 = tmpTriangleEdge[0] - tmpTriangleEdge[1];
		vector2 = tmpTriangleEdge[1] - tmpTriangleEdge[2];
		normal = glm::cross(vector1, vector2);
	}
	else {
		vertices.push_back(-87.8015f);
		vertices.push_back(205.733f);
		vertices.push_back(-291.746f);

		vertices.push_back(-87.7957f);
		vertices.push_back(206.229f);
		vertices.push_back(-291.765f);

		vertices.push_back(-87.3243f);
		vertices.push_back(206.186f);
		vertices.push_back(-291.762f);

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
	}
	/*std::cout << "max X:" << maxX << std::endl;
	std::cout << "min X:" << minX << std::endl;
	std::cout << "mean X:" << (maxX + minX) / 2 << std::endl;
	std::cout << "max Y:" << maxY << std::endl;
	std::cout << "min Y:" << minY << std::endl;
	std::cout << "mean Y:" << (maxY + minY) / 2 << std::endl;
	std::cout << "max Z:" << maxZ << std::endl;
	std::cout << "min Z:" << minZ << std::endl;
	std::cout << "mean Z:" << (maxZ + minZ) / 2 << std::endl;*/
	
}

BaseModel::~BaseModel()
{
}
#endif