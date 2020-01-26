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

#include "utils.h"

#define FIRST 0

BaseModel::BaseModel(const char *cfilename, glm::vec3 color)
{
	this->color = color;
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
		/*int index = 0;
		std::unordered_map< std::string, int> mapping; // 顶点和index
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
		
		for (int i = 0; i < triangles; i++)
		{
			
			float coorXYZ[18];
			in.read((char*)coorXYZ, 18 * sizeof(float));
			for (int j = 0; j < 3; j++) // 三角形三个点的x,y,z
			{
				std::string key = std::to_string(coorXYZ[j * 6]) + std::to_string(coorXYZ[j * 6 +1]) + std::to_string(coorXYZ[j * 6+2]);
				// 判断是否在mapping里
				std::unordered_map< std::string, int> ::const_iterator foundKey = mapping.find(key);
				if (foundKey == mapping.end()) { // 如果不在
					mapping.insert({ key, index });
					indices.push_back(index);
					index++;
					vertices.push_back(coorXYZ[j * 6]);
					vertices.push_back(coorXYZ[j * 6 + 1]);
					vertices.push_back(coorXYZ[j * 6 + 2]);
					vertices.push_back(coorXYZ[j * 6 + 3]);
					vertices.push_back(coorXYZ[j * 6 + 4]);
					vertices.push_back(coorXYZ[j * 6 + 5]);
				}
				else { // 如果在
					indices.push_back(foundKey->second);
				}
			}
		}
		in.close();


		// write in vertices
		int verSize = vertices.size();
		std::string outNameVertices(cfilename, cfilename + strlen(cfilename));
		outNameVertices.insert(strlen(cfilename) - 4, "_vertices");
		std::ofstream outVer(outNameVertices.c_str(), std::ofstream::out | std::ofstream::binary);
		outVer.write((char*)&verSize, sizeof(int));
		std::cout << "write in vertices " << vertices.size() << std::endl;
		for (int i = 0; i < vertices.size(); i++) {
			outVer.write((char*)&vertices[i], sizeof(GLfloat));
		}
		outVer.close();
		// write in indices
		int indSize = indices.size();
		std::string outNameIndices(cfilename, cfilename + strlen(cfilename));
		outNameIndices.insert(strlen(cfilename) - 4, "_indices");
		std::ofstream outInd(outNameIndices.c_str(), std::ofstream::out | std::ofstream::binary);
		outInd.write((char*)&indSize, sizeof(int));
		std::cout << "write in indices " << indices.size() << std::endl;
		for (int i = 0; i < indices.size(); i++) {
			outInd.write((char*)&indices[i], sizeof(int));
		}
		outInd.close();*/

		vertices.clear();
		std::string inNameVertices(cfilename, cfilename + strlen(cfilename));
		inNameVertices.insert(strlen(cfilename) - 4, "_vertices");
		std::ifstream inVer(inNameVertices.c_str(), std::ifstream::in | std::ifstream::binary);
		if (!inVer) {
			std::cout << "fail to open file " << inNameVertices << std::endl;
			return;
		}
		int verSize;
		inVer.read((char*)&verSize, sizeof(int));
		if (verSize == 0) {
			std::cout << "verSize == 0 " << std::endl;
			return;
		}
		float tmpFloat;
		for (int i = 0; i < verSize; i+=6)
		{
			for (int j = 0; j < 6; j++) {
				inVer.read((char*)&tmpFloat, sizeof(GLfloat));
				vertices.push_back(tmpFloat);
				if (j == 0) {
					if (tmpFloat < xMin) {
						xMin = tmpFloat;
					}
					else if (tmpFloat > xMax) {
						xMax = tmpFloat;
					}
				}
				else if (j == 1) {
					if (tmpFloat < yMin) {
						yMin = tmpFloat;
					}
					else if (tmpFloat > yMax) {
						yMax = tmpFloat;
					}
				}
				else if (j == 2) {
					if (tmpFloat < zMin) {
						zMin = tmpFloat;
					}
					else if (tmpFloat > zMax) {
						zMax = tmpFloat;
					}
				}
			}
		}
		inVer.close();

		indices.clear();
		std::string outNameIndices(cfilename, cfilename + strlen(cfilename));
		outNameIndices.insert(strlen(cfilename) - 4, "_indices");
		std::ifstream inInd(outNameIndices.c_str(), std::ifstream::in | std::ifstream::binary);
		if (!inInd) {
			std::cout << "fail to open file " << outNameIndices << std::endl;
			return;
		}
		int indSize;
		inInd.read((char*)&indSize, sizeof(int));
		if (indSize == 0) {
			std::cout << "indSize == 0 " << std::endl;
			return;
		}
		int tmpInt;
		for (int i = 0; i < indSize; i++)
		{
			inInd.read((char*)&tmpInt, sizeof(int));
			indices.push_back(tmpInt);
		}
		inInd.close();

	}
}

BaseModel::BaseModel(const std::vector<GLfloat> &vertices, glm::vec3 color) {
	this->vertices.assign(vertices.begin(), vertices.end());
}
BaseModel::~BaseModel()
{
	std::cout << "-----------~BaseModel-------" << std::endl;
	//delete [] markVoxel;
}
void BaseModel::initVertexObject() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &(vertices.front()), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), &indices[0], GL_STATIC_DRAW);
	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0); // Unbind VAO
	
}

void BaseModel::draw(){
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BaseModel::voxelization() {
	// bounding box
	boxMin = glm::vec3(xMin, yMin, zMin);
	glm::vec3 boxMax = glm::vec3(xMax, yMax, zMax);
	glm::vec3 range = boxMax - boxMin;
	resolution = glm::vec3(range.x / step, range.y / step, range.z / step);
	// camera position
	Camera voxelCameraX(glm::vec3(boxMin.x + 0.2f, ((boxMin + boxMax) / 2.0f).y, ((boxMin + boxMax) / 2.0f).z));
	Camera voxelCameraY(glm::vec3(((boxMin + boxMax) / 2.0f).x, boxMin.y + 0.2f, ((boxMin + boxMax) / 2.0f).z));
	Camera voxelCameraZ(glm::vec3(((boxMin + boxMax) / 2.0f).x, ((boxMin + boxMax) / 2.0f).y, boxMax.z + 0.2f));
	// set ortho
	voxelCameraX.setOrthology(-range.z * 0.51, range.z * 0.51,
		-range.y * 0.51, range.y * 0.51, 0.1, range.x * 1.2 + 0.2f);
	voxelCameraY.setOrthology(-range.x * 0.51, range.x * 0.51,
		-range.z * 0.51, range.z * 0.51, 0.1, range.y * 1.2 + 0.2f);
	voxelCameraZ.setOrthology(-range.x * 0.51, range.x * 0.51,
		-range.y * 0.51, range.y * 0.51, 0.1, range.z * 1.2 + 0.2f);
	
	Shader voxelShader("voxelizeCount.vs", "voxelizeCount.frag", "voxelizeCount.gs");
	voxelShader.use();
	voxelShader.setMat4("projection[0]", voxelCameraX.getOrthology());
	voxelShader.setMat4("projection[1]", voxelCameraY.getOrthology());
	voxelShader.setMat4("projection[2]", voxelCameraZ.getOrthology());

	voxelShader.setMat4("projectionInverse[0]", glm::inverse(voxelCameraX.getOrthology()));
	voxelShader.setMat4("projectionInverse[1]", glm::inverse(voxelCameraY.getOrthology()));
	voxelShader.setMat4("projectionInverse[2]", glm::inverse(voxelCameraZ.getOrthology()));
	

	voxelShader.setVec2("halfPixel[0]", glm::vec2(1.0f / resolution.z, 1.0f / resolution.y));
	voxelShader.setVec2("halfPixel[1]", glm::vec2(1.0f / resolution.x, 1.0f / resolution.z));
	voxelShader.setVec2("halfPixel[2]", glm::vec2(1.0f / resolution.x, 1.0f / resolution.y));
	voxelShader.setFloat("coef[0]", 100.0f*step);
	voxelShader.setFloat("coef[1]", 100.0f*step);
	voxelShader.setFloat("coef[2]", 100.0f*step);

	// debugging para
	/*voxelShader.setVec2("halfPixel[0]", glm::vec2(0.0f, 0.0f));
	voxelShader.setVec2("halfPixel[1]", glm::vec2(0.0f, 0.0f));
	voxelShader.setVec2("halfPixel[2]", glm::vec2(0.0f, 0.0f));

	voxelShader.setFloat("coef[0]", 1);
	voxelShader.setFloat("coef[1]", 1);
	voxelShader.setFloat("coef[2]", 1);*/

	// todo 体素化过程有缺陷，有侧面没有被体素化成功
	

	// polygon mode
	
	// 关闭深度测试和背面剔除，保证模型的全面三角形都进入片元着色器
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	

	int length = static_cast<int>(resolution.x * resolution.y * resolution.z);
	std::cout << "BaseModel.cpp line411 length: " << length << std::endl;
	if (markVoxel == NULL) {
		std::cout << "mark voxel new buffer, length = " << length << std::endl;
		markVoxel = new int[length];

		for (int i = 0; i < length; i++) {
			markVoxel[i] = 0;
		}
	}

	glGenBuffers(1, &m_cntBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cntBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, length * sizeof(int), nullptr, GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_cntBuffer);
	voxelShader.use();
	voxelShader.setVec3("boxMin", boxMin);
	voxelShader.setFloat("step", step);
	voxelShader.setVec3("resolution", resolution);

	// initialize
	int* writePtr = reinterpret_cast<int*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));

	for (int x = 0; x < length; x++) {
		writePtr[x] = 0;
	}
	if (!glUnmapBuffer(GL_SHADER_STORAGE_BUFFER))
		std::cout << "unMap error\n" << std::endl;

	// draw and count
	voxelShader.setMat4("model", glm::mat4(1.0f));
	voxelShader.setMat4("view", voxelCameraZ.GetViewMatrix());
	voxelShader.setMat4("modelInverse", glm::inverse(glm::mat4(1.0f)));
	voxelShader.setMat4("viewInverse", glm::inverse(voxelCameraZ.GetViewMatrix()));
	std::vector<int> markIndex; // 记录GPU计算的体素为1的元素的位置
	draw();
	//glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_cntBuffer);
	int* readPtr = reinterpret_cast<int*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));
	if (readPtr != nullptr) {
		// read
		for (int i = 0; i < length; i++) {
			// 不与网格重合
			if (*(readPtr + i) != 0) {
				markVoxel[i] = 1;
				markIndex.push_back(i);
			}
		}
		std::cout << "markIndex.size()" << markIndex.size() <<  std::endl;
	}
	else {
		std::cout << "fail to read from ssbo" << std::endl;
	}
	
	// 清除多余体素
	for (int i = 0, j = markIndex.size() -1; i < markIndex.size()/2 && j > 0; i++, j--) {

		// i
		int iy = markIndex[i] / (resolution.x * resolution.z);
		int iz = (markIndex[i] - iy * resolution.x * resolution.z) / (resolution.x);
		int ix = markIndex[i] - iy * resolution.x * resolution.z - iz * resolution.x;
		glm::vec3 curPosI = glm::vec3(ix, iy, iz);
		int countNeighbor = myUtils::neighbors26(curPosI, resolution, markVoxel);
		// 判断是否26邻域 > 7
		if (myUtils::neighbors26(curPosI, resolution, markVoxel) > 3) {
			voxelIndex.push_back(curPosI);
			voxelPos.push_back(boxMin + glm::vec3(ix * step, iy * step, iz * step));
		}
		else {
			markVoxel[markIndex[i]] = 0;
		}

		// j
		int jy = markIndex[j] / (resolution.x * resolution.z);
		int jz = (markIndex[j] - jy * resolution.x * resolution.z) / (resolution.x);
		int jx = markIndex[j] - jy * resolution.x * resolution.z - jz * resolution.x;
		glm::vec3 curPosJ = glm::vec3(jx, jy, jz);
		countNeighbor = myUtils::neighbors26(curPosJ, resolution, markVoxel);
		// 判断是否26邻域 > 7
		if (myUtils::neighbors26(curPosJ, resolution, markVoxel) > 3) {
			voxelIndex.push_back(curPosJ);
			voxelPos.push_back(boxMin + glm::vec3(jx * step, jy * step, jz * step));
		}
		else {
			markVoxel[markIndex[j]] = 0;
		}

	}
	
	// 内部体素化

	for (int i = 0, j =0; i < length/2 && j > 0; i++, j--) {
		if (markVoxel[i] == 0) {
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			glm::vec3 curPos = glm::vec3(ix, iy, iz);
			int countNeighbor = myUtils::neighbors26(curPos, resolution, markVoxel);
			// 判断是否26邻域 > 7
			if (countNeighbor >= 3) {
				markVoxel[i] = 1;
				voxelIndex.push_back(curPos);
				voxelPos.push_back(boxMin + glm::vec3(ix * step, iy * step, iz * step));
			}
		}
		if (markVoxel[j] == 0) {
			int jy = j / (resolution.x * resolution.z);
			int jz = (j - jy * resolution.x * resolution.z) / (resolution.x);
			int jx = j - jy * resolution.x * resolution.z - jz * resolution.x;
			glm::vec3 curPos = glm::vec3(jx, jy, jz);
			int countNeighbor = myUtils::neighbors26(curPos, resolution, markVoxel);
			// 判断是否26邻域 > 7
			if (countNeighbor >= 3) {
				markVoxel[j] = 1;
				voxelIndex.push_back(curPos);
				voxelPos.push_back(boxMin + glm::vec3(jx * step, jy * step, jz * step));
			}
		}
	}


	std::cout << "voxelPos.size()" << voxelPos.size() << std::endl;
	
	
	glUnmapBuffer(m_cntBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glDeleteBuffers(1, &m_cntBuffer);
	// todo 移除边缘上的点，即cube和网格相交就移除 => voxelPos到网格每个三角形的距离<step/2,则移除
	
}


