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
	if (frontFace < zMax) {
		frontFace = zMax;
	}
	inVer.close();
	modelCenter += glm::vec3((xMax+xMin)/6.0f, (yMax + yMin) / 6.0f, (zMax + zMin) / 6.0f);
	
	frontFace -= modelCenter.z;
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

BaseModel::BaseModel(const std::vector<GLfloat> &vertices, glm::vec3 color) {
	this->vertices.assign(vertices.begin(), vertices.end());
	this->color = color;
}

BaseModel::BaseModel(const std::vector<GLfloat>& vertices, const std::vector<int>& indices, glm::vec3 color) {
	this->vertices.assign(vertices.begin(), vertices.end());
	this->indices.assign(indices.begin(), indices.end());
	this->color = color;
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
	cout << "debug  1  " << vertices.size();
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
	resolution = glm::vec3(static_cast<int>(range.x / step), static_cast<int>(range.y / step), static_cast<int>(range.z / step));

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

	
	// 关闭深度测试和背面剔除，保证模型的全面三角形都进入片元着色器
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	

	int length = static_cast<int>(resolution.x * resolution.y * resolution.z);
	if (markVoxel == NULL) {
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
	}
	else {
		std::cout << "fail to read from ssbo" << std::endl;
	}
	
	// 清除多余体素
	for (int i = 0, j = markIndex.size() -1; i < markIndex.size()/2 && j > 0; i++, j--) {
		// i - 正向遍历
		int iy = markIndex[i] / (resolution.x * resolution.z);
		int iz = (markIndex[i] - iy * resolution.x * resolution.z) / (resolution.x);
		int ix = markIndex[i] - iy * resolution.x * resolution.z - iz * resolution.x;
		glm::vec3 curPosI = glm::vec3(ix, iy, iz);
		int countNeighbor = myUtils::neighbors26(curPosI, resolution, markVoxel);
		// 判断是否26邻域 > 7，腐蚀
		if (myUtils::neighbors26(curPosI, resolution, markVoxel) > 3) {
			// voxelIndex.push_back(curPosI);
			// voxelPos.push_back(boxMin + glm::vec3(ix * step, iy * step, iz * step));
		}
		else {
			markVoxel[markIndex[i]] = 0;
		}

		// j - 反向遍历
		int jy = markIndex[j] / (resolution.x * resolution.z);
		int jz = (markIndex[j] - jy * resolution.x * resolution.z) / (resolution.x);
		int jx = markIndex[j] - jy * resolution.x * resolution.z - jz * resolution.x;
		glm::vec3 curPosJ = glm::vec3(jx, jy, jz);
		countNeighbor = myUtils::neighbors26(curPosJ, resolution, markVoxel);
		// 判断是否26邻域 > 7
		if (myUtils::neighbors26(curPosJ, resolution, markVoxel) > 3) {
		}
		else {
			markVoxel[markIndex[j]] = 0;
		}
	}

	// 膨胀 t 次
	for (int t = 0; t < 2; t++) {
		for (int i = 0, j = 0; i < length / 2 && j > 0; i++, j--) {
			if (markVoxel[i] == 0) {
				int iy = i / (resolution.x * resolution.z);
				int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
				int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
				glm::vec3 curPos = glm::vec3(ix, iy, iz);
				int countNeighbor = myUtils::neighbors26(curPos, resolution, markVoxel);
				// 判断是否26邻域 > 7
				if (countNeighbor >= 3) {
					markVoxel[i] = 1;
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
				}
			}
		}
	}
	
	// 深度优先搜索
	std::queue<int> inputQueue; 
	// 将体素中心push进去，todo
	
	inputQueue.push(length/2);// 已经验证length/2对应的体素在模型内部
	myUtils::BFS(resolution, markVoxel, inputQueue, length, 1);
	
	for (int i = 0; i < length; i++) {
		if (markVoxel[i] == 2 || markVoxel[i] == 1) {
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			voxelPos.push_back(boxMin + glm::vec3(ix * step, iy * step, iz * step));
			voxelIndex.push_back(glm::vec3(ix, iy, iz));
		}
	}
	glUnmapBuffer(m_cntBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	glDeleteBuffers(1, &m_cntBuffer);
	// todo 移除边缘上的点，即cube和网格相交就移除 => voxelPos到网格每个三角形的距离<step/2,则移除
	
}


