#include "BaseModel.h"
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

#define FIRST 0

BaseModel::BaseModel(const char *cfilename, glm::vec3 color, PrimitiveType type)
{
	this->color = color;
	this->type = type;
	if (FIRST == 1) {
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
		if (triangles == 0)
			return;
		// read triangle mesh in the loop
		glm::vec3 vector1, vector2, normal, tmpTriangleEdge[3];
		// 存顶点坐标和多个数组下标的映射关系
		std::unordered_map< std::string, std::vector<int>> mapping;
		for (int i = 0; i < 200; i++)
		{
			float coorXYZ[12];
			in.read((char*)coorXYZ, 12 * sizeof(float));
			std::ostringstream ss;
			std::string key;
			for (int j = 1; j < 4; j++) // 三角形三个点的x,y,z
			{
				ss << coorXYZ[j * 3];
				std::string s1(ss.str());
				ss << coorXYZ[j * 3 + 1];
				std::string s2(ss.str());
				ss << coorXYZ[j * 3 + 2];
				std::string s3(ss.str());
				// generate key
				key = s1 + s2 + s3;
				tmpTriangleEdge[j - 1].x = coorXYZ[j * 3];
				tmpTriangleEdge[j - 1].y = coorXYZ[j * 3 + 1];
				tmpTriangleEdge[j - 1].z = coorXYZ[j * 3 + 2];
				// 查找key是否存在
				std::unordered_map< std::string, std::vector<int>> ::const_iterator foundKey = mapping.find(key);
				if (foundKey == mapping.end()) {
					std::vector<int> tmp = { i * 18 + (j - 1) * 6 }; // 顶点的X坐标的下标
					mapping.insert({ key, tmp });
				}
				else {
					std::vector<int> tmp = foundKey->second;
					tmp.push_back(i * 18 + (j - 1) * 6);
					mapping[key] = tmp;
				}
			}
			// 计算法向量
			vector1 = tmpTriangleEdge[0] - tmpTriangleEdge[1];
			vector2 = tmpTriangleEdge[0] - tmpTriangleEdge[2];
			normal = glm::normalize(glm::cross(vector1, vector2));
			// 存入vertices
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

		// 更新顶点法向量
		std::cout << "更新顶点法向量" << std::endl;
		int i = 0;
		for (std::unordered_map<std::string, std::vector<int>>::iterator it = mapping.begin(); it != mapping.end(); it++ , i++) {
			if (i % 100 == 0) {
				std::cout << i*100/(float)mapping.size()  << "%" << std::endl;
			}
			
			// 计算平均法向量
			glm::vec3 normal(0.0f, 0.0f, 0.0f);
			for (int j = 0; j < it->second.size(); j++) {
				normal.x += vertices[it->second[j]];
				normal.y += vertices[it->second[j] + 1];
				normal.z += vertices[it->second[j] + 2];
			}
			normal /= sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2));
			for (int j = 0; j < it->second.size(); j++) {
				vertices[it->second[j] + 3] = normal.x;
				vertices[it->second[j] + 4] = normal.y;
				vertices[it->second[j] + 5] = normal.z;
			}
		}
		std::string outName(cfilename);
		outName = outName[0] + "_normal.stl";
		std::ofstream out("test.stl", std::ofstream::out | std::ofstream::binary);
		out << str;
		out << triangles;
		for (int i = 0; i < vertices.size(); i++) {
			out << vertices[i];
		}
		out.close();
	}
	else if (FIRST == 2) {
		std::cout << "first= 2" << std::endl;
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
		if (triangles == 0)
			return;
		// read triangle mesh in the loop
		glm::vec3 vector1, vector2, normal, tmpTriangleEdge[3];
		// 存顶点坐标和多个数组下标的映射关系
		std::unordered_map< std::string, std::vector<int>> mapping;
		for (int i = 0; i < triangles; i++)
		{
			float coorXYZ[12];
			in.read((char*)coorXYZ, 12 * sizeof(float));
			std::string key;
			normal = glm::vec3(coorXYZ[0], coorXYZ[1], coorXYZ[2]);
			for (int j = 1; j < 4; j++) // 三角形三个点的x,y,z
			{
				key.assign((char*)(coorXYZ + j * 3), ((char*)(coorXYZ + j * 3)) + 3 * sizeof(float));
				
				vertices.push_back(coorXYZ[j * 3]);
				vertices.push_back(coorXYZ[j * 3 + 1]);
				vertices.push_back(coorXYZ[j * 3 + 2]);
				vertices.push_back(normal.x);
				vertices.push_back(normal.y);
				vertices.push_back(normal.z);
				std::unordered_map< std::string, std::vector<int>> ::const_iterator foundKey = mapping.find(key);
				if (foundKey == mapping.end()) {
					std::vector<int> tmp = { i * 18 + (j - 1) * 6 }; // 顶点的X坐标的下标
					mapping.insert({ key, tmp });
				}
				else {
					//std::cout << "bingo" << std::endl;
					std::vector<int> tmp = foundKey->second;
					tmp.push_back(i * 18 + (j - 1) * 6);
					mapping[key] = tmp;
				}
			}

			in.read((char*)coorXYZ, 2);
		}
		in.close();
	
		/*(for (std::unordered_map<std::string, std::vector<int>>::iterator it = mapping.begin(); it != mapping.end(); it++) {
			if (it->second.size() > 1) {
				std::cout << "bingo" << std::endl;
			}
			/*std::cout << it->first <<": ";
			for (int j = 0; j < it->second.size(); j++) {
				std::cout << it->second[j] << " ";
			}
			std::cout << std::endl;
		}*/
		std::cout << "更新顶点法向量" << std::endl;
		int k = 0;
		for (std::unordered_map<std::string, std::vector<int>>::iterator it = mapping.begin(); it != mapping.end(); it++, k++) {
			if (k % 100 == 0) {
				std::cout << k * 100 / (float)mapping.size() << "%" << std::endl;
			}
			// 计算平均法向量
			glm::vec3 normal(0.0f, 0.0f, 0.0f);
			for (int j = 0; j < it->second.size(); j++) {
				normal.x += vertices[it->second[j] + 3];
				normal.y += vertices[it->second[j] + 4];
				normal.z += vertices[it->second[j] + 5];
			}
			normal /= sqrt(pow(normal.x, 2) + pow(normal.y, 2) + pow(normal.z, 2));
			for (int j = 0; j < it->second.size(); j++) {
				vertices[it->second[j] + 3] = normal.x;
				vertices[it->second[j] + 4] = normal.y;
				vertices[it->second[j] + 5] = normal.z;
			}
		}
		std::string outName(cfilename, cfilename + strlen(cfilename));
		//std::cout << "test name " << outName << std::endl  << std::endl;
		outName.insert(strlen(cfilename) - 4, "_normal");
		//std::cout << "test name " << outName << std::endl << std::endl;
		std::ofstream out(outName.c_str() , std::ofstream::out | std::ofstream::binary);
		out.write((char*)str, 80);
		// out << str;
		out.write((char*)&triangles, sizeof(int));
		// out << triangles;
		std::cout << "triangles " << triangles << std::endl;
		for (int i = 0; i < vertices.size(); i++) {
			out.write((char*)&vertices[i], sizeof(GLfloat));
		}
		out.close();
	}
	else {
		std::cout << "读取normal.std" << std::endl;
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
		if (triangles == 0) {
			std::cout << "triangles == 0 " << std::endl;
			return;
		}
		else {
			std::cout << "triangles " << triangles << std::endl;
		}
			
		// read triangle mesh in the loop
		// 存顶点坐标和多个数组下标的映射关系
		//std::unordered_map< std::string, std::vector<int>> mapping;
		for (int i = 0; i < triangles; i++)
		{
			float coorXYZ[18];
			in.read((char*)coorXYZ, 18 * sizeof(float));
			for (int j = 0; j < 3; j++) // 三角形三个点的x,y,z
			{
				vertices.push_back(coorXYZ[j * 6]);
				vertices.push_back(coorXYZ[j * 6 + 1]);
				vertices.push_back(coorXYZ[j * 6 + 2]);
				vertices.push_back(coorXYZ[j * 6 + 3]);
				vertices.push_back(coorXYZ[j * 6 + 4]);
				vertices.push_back(coorXYZ[j * 6 + 5]);
			}
		}
		in.close();
	}
	
	
}

BaseModel::BaseModel(const std::vector<GLfloat> &vertices, glm::vec3 color, PrimitiveType type) {
	this->vertices.assign(vertices.begin(), vertices.end());
	this->type = type;
}
BaseModel::~BaseModel()
{
}
void BaseModel::initVertexObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

	// Position attribute
	switch (type) {
	case TRIANGLE:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 8* sizeof(GLfloat), (GLvoid*)0);
		//glEnableVertexAttribArray(2);
		break;
	case LINE:
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0); // Unbind VAO
	
}

void BaseModel::initDepthBuffer() {
	
}

void BaseModel::draw(){
	glBindVertexArray(VAO);
	switch (type) {
	case TRIANGLE:
		glDrawArrays(GL_TRIANGLES, 0, vertices.size() * 3);
		break;
	case LINE:
		glDrawArrays(GL_LINE_STRIP, 0, 2);
		break;
	}

	glBindVertexArray(0);
}