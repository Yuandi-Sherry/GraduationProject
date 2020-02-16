#include "Area.h"
#include "Plane.h"
// imgui
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
#include <set>
#include "MyCylinder.h"
#include "MyLine.h"

#include "utils.h"
/*----------------------GENERAL----------------------*/
extern const unsigned int SCR_WIDTH, SCR_HEIGHT;
float light_near_plane = 0.0f, light_far_plane = 800.0f, light_left_plane = -150.0f, light_right_plane = 150.0f, light_top_plane = 150.0f, light_bottom_plane = -150.0f;
Area::Area()
{
	modelsID.clear();
	transformMat = glm::rotate( glm::mat4(1.0f), glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
}

Area::~Area()
{
}

void Area::init() {
	camera.setPosition(cameraPos);camera.setOrthology();
	// ruler
	ruler.initVertexObject();
	mySphere.initVertexObject();
	myCube.initVertexObject();
	lines.initVertexObject();
	// init 
	lightProjection = glm::ortho(light_left_plane, light_right_plane, light_bottom_plane, light_top_plane, light_near_plane, light_far_plane);
	lightView = glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;

	initDepthBuffer();	
}

void Area::initDepthBuffer() {
	// create depth texture
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Area::setModelsID(const std::vector<GLint>& modelsIDs, std::vector<BaseModel> & models) {
	this->modelsID.assign(modelsIDs.begin(), modelsIDs.end());
}

void Area::tackleRemoveTumor(Shader& shader, Shader& shadowShader, std::vector<BaseModel>& models) {
	if (removeMode == 0) { // normal
		shader.use();
		drawModels(shader, shadowShader, models);
	}
	else if (removeMode == 1) { // 获取鼠标位置转换
		drawModels(shader, shadowShader, models);
		shader.use();
		glm::mat4 model = glm::translate(glm::mat4(1.0f), removePos);
		model = glm::translate(model, -BaseModel::modelCenter);
		model = glm::scale(model, glm::vec3(cutRadius, cutRadius, cutRadius));
		shader.setMat4("model", transformMat * model);
		shader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
		mySphere.draw();
	}
}

void Area::setRemovePos(glm::vec3 pos) {
	// local coordinate
	removePos = glm::inverse(glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter)) * glm::inverse(transformMat) * glm::vec4(pos, 1.0f);
	removeMode = 1;
}

void Area::updateCutVertices(BaseModel & tumor) {
	voxel2D.clear();
	voxel3D.clear();
	vertex2D.clear();
	vertex3D.clear();
	combinedPoints.clear();
	glm::vec3 resolution = tumor.getResolution();
	// 记录要删除的网格顶点
	std::vector<GLfloat> tmpVertices(*tumor.getVertices());
	std::vector<int> tmpIndices(*tumor.getIndices());
	std::vector<int> tobeDelete;
	std::map<int, int> updateIndices;
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		glm::vec3 curVer = glm::vec3(tmpVertices[i * 6], tmpVertices[i * 6 + 1], tmpVertices[i * 6 + 2]);
		GLfloat dis = glm::distance(curVer, removePos);
		if (dis < cutRadius) {
			// 记录需要删除的点在vertices的index
			tobeDelete.push_back(i);
		}
	}

	// 计算前后indices的映射关系
	int* newIndices = new int[tmpVertices.size() / 6];
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		newIndices[i] = i;
	}
	for (int i = 0, j = 0; i < tmpVertices.size() / 6 && j < tobeDelete.size(); i++) {
		if (i == tobeDelete[j]) {
			j++;
			// 之后的下标更新
			newIndices[i] = -1;
			for (int k = i + 1; k < tmpVertices.size() / 6; k++) {
				newIndices[k] -= 1;
			}
		}
	}

	// 计算边缘顶点构成的平面的平均法向量
	glm::vec3 averNormal = glm::vec3(0, 0, 0); //边缘顶点的平均法向量
	glm::vec3 averCenter = glm::vec3(0, 0, 0); //边缘顶点的平均法向量
	std::set<int> edgeIndices; // 记录边缘顶点在【更新后】vertices中的下标
	for (int i = 0; i < tmpIndices.size() / 3; i++) {
		//记录和移除点相连的三角形
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// 对是-1的检测之前是否加入，其他的加入
			for (int j = 0; j < 3; j++) {
				if (newIndices[tmpIndices[i * 3 + j]] != -1) {
					// 直接放入映射后的index
					edgeIndices.insert(newIndices[tmpIndices[i * 3 + j]]);
					averCenter += glm::vec3(tmpVertices[tmpIndices[i * 3 + j] * 6], tmpVertices[tmpIndices[i * 3 + j] * 6 + 1], tmpVertices[tmpIndices[i * 3 + j] * 6 + 2]);
					averNormal += glm::vec3(tmpVertices[tmpIndices[i * 3 + j] * 6 + 3], tmpVertices[tmpIndices[i * 3 + j] * 6 + 4], tmpVertices[tmpIndices[i * 3 + j] * 6 + 5]);
				}
			}
		}
	}

	// 转为vector

	// 更新indices
	for (int i = 0; i < tmpIndices.size() / 3; ) {
		// 删除和移除点相连的三角形
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// 删除
			tmpIndices.erase(tmpIndices.begin() + i * 3 + 2);
			tmpIndices.erase(tmpIndices.begin() + i * 3 + 1);
			tmpIndices.erase(tmpIndices.begin() + i * 3);
		}
		else {
			tmpIndices[i * 3] = newIndices[tmpIndices[i * 3]];
			tmpIndices[i * 3 + 1] = newIndices[tmpIndices[i * 3 + 1]];
			tmpIndices[i * 3 + 2] = newIndices[tmpIndices[i * 3 + 2]];
			i++;
		}
	}
	delete[] newIndices;
	// 删除对应的vertices
	for (int i = 0; i < tobeDelete.size(); i++) {
		int index = (tobeDelete[i] - i) * 6;
		tmpVertices.erase(tmpVertices.begin() + index + 5);
		tmpVertices.erase(tmpVertices.begin() + index + 4);
		tmpVertices.erase(tmpVertices.begin() + index + 3);
		tmpVertices.erase(tmpVertices.begin() + index + 2);
		tmpVertices.erase(tmpVertices.begin() + index + 1);
		tmpVertices.erase(tmpVertices.begin() + index);
	}
	averCenter /= edgeIndices.size();
	GLfloat averRadius = 0;
	for (std::set<int> ::iterator it = edgeIndices.begin(); it != edgeIndices.end(); it++) {
		averRadius += glm::distance(averCenter, glm::vec3(tmpVertices[(*it) * 6], tmpVertices[(*it) * 6 + 1], tmpVertices[(*it) * 6 + 2]));
	}
	averRadius /= edgeIndices.size();
	// 计算离averNormal最近的球面坐标作为二维空间原点
	int originIndex = -1;
	GLfloat maxDot = -1;

	// 局部坐标系位置
	std::vector<glm::vec3>* voxelsPos = tumor.getVoxelsPos();
	// 体素空间的下标
	std::vector<glm::vec3>* voxelsIndex = tumor.getVoxelsIndex();
	std::set<int> edgeVoxelsIndex;
	// 标记边缘体素
	for (int i = 0; i < voxelsPos->size(); ) {
		// 判断是否在球里面
		GLfloat dis = glm::distance((*voxelsPos)[i], removePos);
		if (dis < cutRadius) {
			// 移除局部坐标
			voxelsPos->erase(voxelsPos->begin() + i);
			// 计算标记数组的的下标
			int tmpIndex = (voxelsIndex->begin() + i)->y * resolution.x * resolution.z + (voxelsIndex->begin() + i)->z * resolution.x + (voxelsIndex->begin() + i)->x;
			// 更新标记数组
			tumor.markVoxel[tmpIndex] = 0;
			// 26邻域
			std::vector<int> resultIndex;
			myUtils::neighbors26(*(voxelsIndex->begin() + i), resolution, resultIndex);
			// 标记数组中为1且在result中的变为2，否则仍然为1

			for (int j = 0; j < resultIndex.size(); j++) {
				if (tumor.markVoxel[resultIndex[j]] == 1) {
					// 将对应位置标记为2
					tumor.markVoxel[resultIndex[j]] = 2;
					// 记录位置坐标
					edgeVoxelsIndex.insert(resultIndex[j]);
				}
			}
			voxelsIndex->erase(voxelsIndex->begin() + i);
		}
		else {
			i++;
		}
	}
	// 在边缘体素中计算原点坐标
	glm::vec3 origin = glm::vec3(-10000.0f, -10000.0f, -10000.0f);
	glm::vec3 originNormal;
	glm::vec3 originRight; // 任意一个法向量的垂直方向作为x+
	glm::vec3 originUp;
	std::cout << "maxDot " << maxDot << std::endl;
	for (int i = 0; i < static_cast<int>(resolution.x * resolution.y * resolution.z); i++) {
		if (tumor.markVoxel[i] == 2) {
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			glm::vec3 coor = tumor.boxMin + glm::vec3(ix * tumor.getStep(), iy * tumor.getStep(), iz * tumor.getStep());
			glm::vec3 tmpNormal = glm::normalize(removePos - coor);
			// 确定原点
			if (glm::dot(tmpNormal, averNormal) > maxDot) {
				maxDot = glm::dot(tmpNormal, averNormal);
				originIndex = i;
			}
		}
	}

	if (originIndex != -1) {
		int originY = originIndex / (resolution.x * resolution.z);
		int originZ = (originIndex - originY * resolution.x * resolution.z) / (resolution.x);
		int originX = originIndex - originY * resolution.x * resolution.z - originZ * resolution.x;
		origin = tumor.boxMin + glm::vec3(originX * tumor.getStep(), originY * tumor.getStep(), originZ * tumor.getStep());
		// 法向量
		originNormal = glm::normalize(removePos - origin);
		originRight = glm::normalize(glm::cross(originNormal, glm::vec3(0, 0, 1)));
		originUp = glm::normalize(glm::cross(originNormal, originRight));
		voxel2D.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		voxel3D.push_back(origin);
		tumor.markVoxel[originIndex] = 0; // 这个位置就不存在了，而改为tumor模型上的位置了
	}
	else {
		std::cout << "FAIL TO FIND ORIGIN" << std::endl;
		return;
	}

	// 计算边缘体素的二维坐标
	for (int i = 0; i < static_cast<int>(resolution.x * resolution.y * resolution.z); i++) {
		if (i != originIndex && tumor.markVoxel[i] == 2) {

			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			//
			glm::vec3 coor = tumor.boxMin + glm::vec3(ix * tumor.getStep(), iy * tumor.getStep(), iz * tumor.getStep());
			GLfloat distance = glm::distance(coor, removePos);
			glm::vec3 newCoor = coor + (removePos - coor) * (distance - cutRadius) / distance;
			// 计算平面上的坐标: 弧长
			double rho = acos(glm::dot(originNormal, glm::normalize(newCoor - origin))) * cutRadius * glm::distance(newCoor, origin) / 10.0f;
			// 和纵轴夹角（y正半轴
			glm::vec3 OP = newCoor - origin;
			glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
			glm::vec3 OQ = OP - OH;
			GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
			if (glm::dot(glm::normalize(OQ), originUp) < 0) {
				theta = -theta;
			}
			// 世界坐标
			voxel3D.push_back(newCoor);
			// 二维坐标：x = rho*sin theta, y = rho * cos theta.
			voxel2D.push_back(glm::vec3(rho * cos(theta), rho * sin(theta), 200.0f));
			// 恢复1
			tumor.markVoxel[i] = 1;
		}
	}

	// 计算边缘顶点的二维空间坐标
	std::vector<int> orderedIndices;
	for (std::set<int>::iterator it = edgeIndices.begin(); it != edgeIndices.end(); it++) {
		glm::vec3 coor = glm::vec3(tmpVertices[(*it) * 6], tmpVertices[(*it) * 6 + 1], tmpVertices[(*it) * 6 + 2]);
		// 调整三维坐标
		GLfloat distance = glm::distance(coor, removePos);
		glm::vec3 newCoor = coor + (removePos - coor) * (distance - cutRadius) / distance;

		// 修改边缘顶点，使其构成一个正圆
		tmpVertices[(*it) * 6] = newCoor.x;
		tmpVertices[(*it) * 6 + 1] = newCoor.y;
		tmpVertices[(*it) * 6 + 2] = newCoor.z;

		// 计算球面距离
		GLfloat rho = 5 * acos(glm::dot(originNormal, glm::normalize(newCoor - origin))) * cutRadius;

		// 计算和纵轴夹角（y正半轴
		glm::vec3 OP = newCoor - origin;
		glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
		glm::vec3 OQ = OP - OH;
		GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
		if (glm::dot(glm::normalize(OQ), originUp) < 0) {
			theta = -theta;
		}
		// 二维坐标：x = rho*sin theta, y = rho * cos theta.
		for (int k = 0; k < vertex2D.size(); k++) {
			if (abs(vertex2D[k].x - rho * cos(theta)) < 0.00001f
				&& abs(vertex2D[k].y - rho * sin(theta) < 0.00001f)) {
			}
		}
		vertex2D.push_back(glm::vec2(rho * cos(theta), rho * sin(theta)));
		vertex3D.push_back(newCoor);
		orderedIndices.push_back(*it);
	}

	int oldVerticesSize = tmpVertices.size() / 6;
	// 将verticesToAppend加入vertices之后
	for (int i = 0; i < voxel3D.size(); i++) {
		tmpVertices.push_back(voxel3D[i].x);
		tmpVertices.push_back(voxel3D[i].y);
		tmpVertices.push_back(voxel3D[i].z);
		// normal
		glm::vec3 normal = removePos - voxel3D[i];
		tmpVertices.push_back(normal.x);
		tmpVertices.push_back(normal.y);
		tmpVertices.push_back(normal.z);
	}

	// -------------在二维空间构造三角剖分-----------------
	// 要求voxel在前,vertex在后
	combinedPoints.assign(voxel2D.begin(), voxel2D.end());
	combinedPoints.insert(combinedPoints.end(), vertex2D.begin(), vertex2D.end());
	myUtils::generateMesh(combinedPoints, mesh);
	for (int i = 0; i < combinedPoints.size(); i++) {
		mesh.addPoint(combinedPoints[i]);
	}
	mesh.deleteSuperTriangle();

	// 根据mesh1对应的下标，加入indices，vertex采用edgeIndices，voxel直接往后加
	for (std::list<Triangle>::iterator it = mesh.triangleVector.begin(); it != mesh.triangleVector.end(); it++) {
		// 三个点都为顶点则去除
		for (int j = 0; j < 3; j++) {
			// 如果是体素坐标
			if (it->vertices[j] - 4 < (int)voxel2D.size()) {
				// vertices4D在vertices的下标为：原始长度（移除后，插入前） + 在合并中的位置
				tmpIndices.push_back(oldVerticesSize + it->vertices[j] - 4);
			}
			// 如果是vertex
			else {
				// vertex的下标为edgeIndices
				tmpIndices.push_back(orderedIndices[it->vertices[j] - 4 - (int)voxel2D.size()]);
			}
		}
	}
	/// 更新模型
	tumor.setVertices(tmpVertices);
	tumor.setIndices(tmpIndices);
	
}

void Area::setViewport(GLfloat left, GLfloat bottom, GLfloat width, GLfloat height) {
	viewportPara[0] = left;
	viewportPara[1] = bottom;
	viewportPara[2] = width;
	viewportPara[3] = height;
	camera.setSize(width, height);
}

void Area::updateLightSpaceMatrix() {
	lightProjection = glm::ortho(light_left_plane,light_right_plane, light_bottom_plane, light_top_plane, light_near_plane, light_far_plane);
	lightView = glm::lookAt(camera.Position, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
}

void Area::renderDepthBuffer(Shader & shadowShader, std::vector<BaseModel> & models) {
	shadowShader.use();
	shadowShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model,-BaseModel::modelCenter);
	shadowShader.setMat4("model", transformMat * model);
	for (int i = 0; i < modelsID.size(); i++) {
		models[modelsID[i]].draw();
	}
	if (modeSelection == RULER || modeSelection == NEAREST_VESSEL) {
		model = glm::translate(glm::mat4(1.0f), ruler.position);
		model = glm::rotate(model, ruler.rotateAngle, glm::vec3(0, 0, 1));
		model = glm::scale(model, glm::vec3(ruler.scaleSize, ruler.scaleSize, 1));
		shadowShader.setMat4("model", model);
		ruler.draw();
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);	
}

glm::vec4 Area::getViewport() {
	return glm::vec4(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
}


void Area::drawModels(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models) {
	shadowShader.use();
	shadowShader.setInt("cut", 0);
	renderDepthBuffer(shadowShader, models);
	glViewport(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
	
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader.setInt("withShadow", 1);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightPos", camera.Position);
	shader.setInt("cut", 0);
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	std::vector<glm::vec3>* tmpVoxelPos = models[1].getVoxelsPos();
	glm::mat4 model(1.0f);
	// debug 显示体素
	/*if (modeSelection == REMOVE_TUMOR) {
		if (tmpVoxelPos->size() != 0) {// 有体素坐标
			for (int j = 0; j < tmpVoxelPos->size(); j++) {
				shader.setVec3("color", models[1].getColor());
				model = glm::scale(transformMat * glm::translate(glm::translate(glm::mat4(1.0f), (*tmpVoxelPos)[j]),-BaseModel::modelCenter), glm::vec3(models[1].getStep() / 1.2, models[1].getStep() / 1.2, models[1].getStep() / 1.2));
				shader.setMat4("model", model);
				// mySphere.draw();
			}
		}
	}*/
	model = glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter);
	shader.setMat4("model", transformMat * model);
	shader.setInt("withLight", 1);
	shader.setInt("isPlane", 0);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setVec3("color", models[modelsID[i]].getColor());
		models[modelsID[i]].draw();
	}
	// debug 单独显示肿瘤
	/*shader.setVec3("color", models[modelsID[1]].getColor());
	models[modelsID[1]].draw();*/
}


void Area::drawRuler(Shader & textureShader, Shader& shader) {
	// ruler
	textureShader.use();
	textureShader.setVec3("viewPos", camera.Position);
	textureShader.setVec3("lightPos", camera.Position);
	textureShader.setMat4("projection", camera.getOrthology());
	textureShader.setMat4("view", camera.GetViewMatrix());
	//改为从0刻度开始
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, ruler.position);
	model = glm::rotate(model, ruler.rotateAngle, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(ruler.scaleSize, ruler.scaleSize, 1));
	textureShader.setMat4("model", model);
	ruler.generateTexture();
	ruler.draw();

	// ends
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightPos", camera.Position);
	shader.setInt("withLight", 1);
	shader.setInt("withShadow", 0);
	if (modeSelection == NEAREST_VESSEL)
		shader.setVec3("color", glm::vec3(1.0f, 0.4f, 0.4f));
	model = glm::scale(glm::translate(glm::mat4(1.0f), ruler.ends[0]), glm::vec3(3, 3, 3));
	shader.setMat4("model", model);
	mySphere.draw();
	shader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
	model = glm::scale(glm::translate(glm::mat4(1.0f), ruler.ends[1]), glm::vec3(3, 3, 3));
	shader.setMat4("model", model);
	mySphere.draw();
}



void Area::displayGUI() {
	ImGui::RadioButton("General", &modeSelection, GENERAL);
	ImGui::RadioButton("Ruler", &modeSelection, RULER);
	ImGui::RadioButton("Nearest Vessel", &modeSelection, NEAREST_VESSEL);
	ImGui::RadioButton("Rmove Tumor", &modeSelection, REMOVE_TUMOR);
	// adjust the radius of removing tumor
	if (modeSelection == REMOVE_TUMOR) {
		ImGui::SliderFloat("remove rumor radius: ", &cutRadius, 0, 20);
	}
}
/*----------------------RULER----------------------*/
void Area::setRulerVertex(const glm::vec3& vertexPosition) {
	glm::vec3 localPos = glm::vec4(vertexPosition, 1.0f);
	if (currentRulerIndex == 0) { // first end
		ruler.ends[currentRulerIndex] = localPos;
		currentRulerIndex = 1;
	}
	else { // second end
		ruler.ends[currentRulerIndex] = localPos;
		currentRulerIndex = 0;
		// calculate ruler attributes
		updateRuler(ruler.ends[0], ruler.ends[1]);
	}
}

void Area::updateRuler(const glm::vec3& pos1, const glm::vec3& pos2) { // 输入参数为世界坐标
	// calculate ruler attributes
	ruler.distance = glm::distance(pos1, pos2);	
	ruler.ends[0] = pos1;
	ruler.ends[1] = pos2;
	ruler.ends[0].z = ruler.CUTFACE;
	ruler.ends[1].z = ruler.CUTFACE;
	GLfloat projLen = glm::distance(ruler.ends[0], ruler.ends[1]);
	ruler.scaleSize = projLen / ruler.distance * 20 ;
	cout << "scale " << ruler.scaleSize << endl;
	ruler.position = pos1 - (pos1 - pos2) / ruler.scaleSize / 2.0f;
	ruler.position.z = ruler.CUTFACE;
	// 使pos1为刻度0
	ruler.rotateAngle = atan((pos1.y - pos2.y) / (pos1.x - pos2.x));
}

void Area::tackleRuler(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models) {
	shader.use();
	drawModels(shader, shadowShader, models);
	drawRuler(textureShader, shader);
}


/*----------------------NEAREST_VESSEL----------------------*/
void Area::tackleNearestVessel(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models) {
	shader.use();
	drawModels(shader, shadowShader, models);

	if (glm::vec3(-10000.0f, -10000.0f, -10000.0f) != nearestPos) {
		drawRuler(textureShader, shader);
	}
}
void Area::setLocalCoordinate(glm::vec3 worldCoor, BaseModel& vessel) {
	NVLocalPos = glm::inverse(glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter)) * glm::inverse(transformMat) * glm::vec4(worldCoor, 1.0f);
	findNearest(vessel);
}

void Area::findNearest(BaseModel& vessel) {
	GLfloat min = 10000.0f;
	int minIndex = 0;
	for (int i = 0; i < vessel.getVertices()->size() / 6; i++) {
		glm::vec3 pos = glm::vec3((*vessel.getVertices())[i*6], (*vessel.getVertices())[i*6+1], (*vessel.getVertices())[i*6+2]);
		GLfloat dis = glm::distance(NVLocalPos, pos);
		if (dis < min) {
			min = dis;
			minIndex = i;
		}
	}
	vesselDistance = min;
	nearestPos = glm::vec3((*vessel.getVertices())[minIndex * 6], (*vessel.getVertices())[minIndex * 6 + 1], (*vessel.getVertices())[minIndex * 6 + 2]);
	glm::mat4 model = glm::translate(glm::mat4(1.0f),-BaseModel::modelCenter);
	model = transformMat * model;
	updateRuler(model * glm::vec4(nearestPos, 1.0f), model * glm::vec4(NVLocalPos, 1.0f));
}



void Area::removeTumor(BaseModel& tumor) {
	// if it will cut vessels
	// 
	/*if (checkCutVessels(models[0])) {
		// 
	}
	else {*/
		updateCutVertices(tumor);
		tumor.initVertexObject();
		removeMode = 0;
	//}
	
}