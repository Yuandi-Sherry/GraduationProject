#include "MySphere.h"
#include <iostream>
#include <cstdlib>
#include <cmath> 
void MySphere::generateVertices() {
	GLfloat theta = 0.0f, phi = 0.0f;
	int thetaNum = 20, phiNum = 20;
	GLfloat thetaStep =  PI / thetaNum, phiStep = 2* PI / phiNum;
	GLfloat radius = 1.0f;
	// 最上面的点
	GLfloat x = radius * cos(phi) * sin(theta);	// 记得转化精度
	GLfloat y = radius * sin(phi) * sin(theta);
	GLfloat z = radius * cos(theta);

	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	glm::vec3 normal = glm::normalize(glm::vec3(x, y, z));
	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);
	// 后续的
	for (float j = 1; j < thetaNum; j++)
	{
		for (float i = 0; i < phiNum; i++)
		{
			phi = phiStep * i;
			theta = thetaStep * j; // 起点都是轴指向的方向。根据右手定则决定转向，只要转向相同，那么两个就合适
			x = radius * cos(phi) * sin(theta);	// 记得转化精度
			y = radius * sin(phi) * sin(theta);
			z =  radius* cos(theta);

			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			normal = glm::normalize(glm::vec3(x,y,z));
			if (abs(x - 0.0f) < 0.001f && abs(y - 0.0f) < 0.001f && abs(z - 0.0f) < 0.001f) {
				std::cout << "法向量为0" << std::endl;
			}
			vertices.push_back(normal.x);
			vertices.push_back(normal.y);
			vertices.push_back(normal.z);
		}
	}
	// 最后的点
	phi = phiStep * phiNum;
	theta = thetaStep * thetaNum;
	x = radius * cos(phi) * sin(theta);	// 记得转化精度
	y = radius * sin(phi) * sin(theta);
	z = radius * cos(theta);
	vertices.push_back(x);
	vertices.push_back(y);
	vertices.push_back(z);
	normal = glm::normalize(glm::vec3(x, y, z));
	vertices.push_back(normal.x);
	vertices.push_back(normal.y);
	vertices.push_back(normal.z);


	int only = vertices.size();
	std::cout << "vertices.size() in ball " << vertices.size() /6 << std::endl;

	
	// 第一行的indices
	// 1 ~ 20
	for (int i = 1; i < phiNum; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i+1);
	}
	indices.push_back(0);
	indices.push_back(phiNum);
	indices.push_back(1);

	for (int j = 0; j < thetaNum - 1; j++) {// 19
		for (int i = 0; i < phiNum - 1; i++) { // 20
			indices.push_back(j* phiNum + i + 1);
			indices.push_back((j+1) * phiNum + i + 1);
			indices.push_back((j + 1) * phiNum + i + 2);

			indices.push_back(j * phiNum + i + 1);
			indices.push_back(j * phiNum + i + 2);
			indices.push_back((j + 1) * phiNum + i + 2);
		}
		// 第一个和最后一个的闭合: i = 0 和 i = 19
		indices.push_back(j * phiNum + 19 + 1);
		indices.push_back((j + 1) * phiNum + 19 + 1);
		indices.push_back((j + 1) * phiNum + 1);

		indices.push_back(j * phiNum + 19 + 1);
		indices.push_back(j * phiNum + 1);
		indices.push_back((j + 1) * phiNum + 1);
	}

	// 和最后一个点的连接
	for (int i = 0; i < phiNum - 1; i++) {
		indices.push_back(vertices.size()/6 - 1);
		indices.push_back((thetaNum-2) * phiNum+i+1);
		indices.push_back((thetaNum -2) * phiNum+i+2);
	}
	indices.push_back(vertices.size() / 6 - 1);
	indices.push_back((thetaNum - 2) * phiNum + 1);
	indices.push_back((thetaNum - 2) * phiNum + 19 + 1);
	// for(int i = 0; i < )
	/*int num = (int)((only / (3 * thetaNum)) * 2);
	for (int x = 0; x < phiNum / 2;)
	{
		for (int y = 0; y < thetaNum; y++)
		{
			indices.push_back(y + x * phiNum);
			indices.push_back(y + x * phiNum + 1);
			indices.push_back(y + x * phiNum + phiNum);
			indices.push_back(y + x * phiNum + phiNum + 1);
			indices.push_back(y + x * phiNum + phiNum);
			indices.push_back(y + x * phiNum + 1);
		}
		x = x + 1;
	}

	// 检测是否有重复坐标
	int repulicate = 0;
	for (int i = 0; i < vertices.size(); i+=6) {
		bool flag = false;
		for (int j = i+6; j < vertices.size(); j+=6) {
			/* (std::abs(vertices[i] - vertices[j]) < 0.000000001f
				&& std::abs(vertices[i+1] - vertices[j+1]) < 0.000000001f
				&& std::abs(vertices[i+2] - vertices[j+2]) < 0.000000001f) {
				flag = true;
				break;
			}
			if (vertices[i] == vertices[j] && vertices[i + 1] == vertices[j + 1] && vertices[i + 2] == vertices[j + 2]) {
				flag = true;
				break;
			}
		}
		if (flag) {
			std::cout << "重复坐标值：" << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << std::endl;
			repulicate++;
		}
		
	}
	std::cout << "球体表面重复坐标" << repulicate << "顶点个数" << vertices.size()/6 << std::endl;*/
}

glm::vec3 MySphere::calNormal(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
	glm::vec3 vec1 = p1 - p2;
	glm::vec3 vec2 = p3 - p2;
	glm::vec3 ans = glm::cross(vec1, vec2);
	return glm::normalize(ans);
}
MySphere::MySphere()
{
	generateVertices();
}

MySphere::~MySphere()
{
}
void MySphere::draw() {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void MySphere::initVertexObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);

	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
}
