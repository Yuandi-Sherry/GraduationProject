#include "Area.h"
#include "Plane.h"
// imgui
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
#include <set>
#include "MyCylinder.h"

#include "utils.h"

GLfloat zR = 1, zG = 1, zB = 1;
extern const unsigned int SCR_WIDTH, SCR_HEIGHT;
float light_near_plane = 0.0f, light_far_plane = 800.0f, light_left_plane = -150.0f, light_right_plane = 150.0f, light_top_plane = 150.0f, light_bottom_plane = -150.0f;
Area::Area()
{
	modelsID.clear();
	transformMat = glm::rotate( glm::mat4(1.0f), glm::radians(-90.0f),glm::vec3(1.0f, 0.0f, 0.0f));
	testPlane = NULL;
}

Area::~Area()
{
	if (testPlane != NULL) {
		delete testPlane;
	}
}

void Area::init() {
	camera.setPosition(cameraPos);
//	ortho = glm::ortho(-orthoWidth / 2, orthoWidth / 2, -orthoWidth / 2 * height / width, orthoWidth / 2 * height / width, Near, 1000.0f);
	camera.setOrthology();
	// mode
	modeSelection = 4;

	// ruler
	ruler.initVertexObject();
	// cut
	csPlane.initVertexObject();
	cutMode = 1; // general

	// nearest vessel
	mySphere.initVertexObject();
	myCube.initVertexObject();
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
	this->models = &models;
}

void Area::tackleCrossIntersection(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models) {
	shader.use();
	if (cutMode == 1) { // selecting
		camera.setSize(viewportPara[2], viewportPara[3]);
		drawModels(shader, shadowShader, models);
	}
	else if (cutMode == 2) { // confirming
		drawModels(shader, shadowShader, models);
		drawSelectedFace(shader);
	}
	else {// confirmed {
		camera.setSize(viewportPara[2] / 2, viewportPara[3]);
		drawCutFace(shader, shadowShader, models);
	}
}

void Area::tackleRuler(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models) {
	shader.use();
	drawModels(shader, shadowShader, models);
	drawRuler(textureShader, shader);
}


void Area::tackleNearestVessel(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models) {
	shader.use();
	drawModels(shader, shadowShader, models);

	if (glm::vec3(-10000.0f, -10000.0f, -10000.0f) != nearestPos) {
		// draw point
		/*shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shader.setInt("withShadow", 1);
		shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("lightPos", camera.Position);
		shader.setInt("cut", 0);
		shader.setVec4("plane", planeCoeff);
		shader.setMat4("projection", camera.getOrthology());
		shader.setMat4("view", camera.GetViewMatrix());
		glm::mat4 model = glm::translate(glm::mat4(1.0f), selectedEnds[1]);
		model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		model = glm::scale(model, glm::vec3(3, 3, 3));
		shader.setMat4("model", transformMat * model);
		shader.setInt("withLight", 1);
		shader.setInt("isPlane", 0);
		shader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
		mySphere.draw();
		model = glm::translate(glm::mat4(1.0f), selectedEnds[0]);
		model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		model = glm::scale(model, glm::vec3(3, 3, 3));
		shader.setMat4("model", transformMat * model);
		mySphere.draw();
		// display ruler*/

		drawRuler(textureShader, shader);
	}
}

void Area::tackleRemoveTumor(Shader& shader, Shader& shadowShader, std::vector<BaseModel>& models) {
	if (removeMode == 0) { // normal
		shader.use();
		drawModels(shader, shadowShader, models);
	}
	else if (removeMode == 1) { // 获取鼠标位置转换
		drawModels(shader, shadowShader, models);
		// draw remove point
		shader.use();
		
		glm::mat4 model = glm::translate(glm::mat4(1.0f), removePos);
		model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		model = glm::scale(model, glm::vec3(cutRadius, cutRadius, cutRadius));
		shader.setMat4("model", transformMat * model);
		shader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
		mySphere.draw();
	}
}

void Area::setRemovePos(glm::vec3 pos) {
	
	removePos = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f))) * glm::inverse(transformMat) * glm::vec4(pos, 1.0f);
	removeMode = 1;
}
void Area::updateCutVertices(BaseModel & tumor) {
	// 局部坐标系位置
	std::vector<glm::vec3>* voxelsPos = tumor.getVoxelsPos();
	// 体素空间的下标
	std::vector<glm::vec3>* voxelsIndex = tumor.getVoxelsIndex();
	// 体素空间分辨率
	glm::vec3 resolution = tumor.getResolution();
	std::set<int> edgeVoxelsIndex; 
	// 挖去体素
	int count1 = 0; // 记录边缘体素数量
	// 处理体素
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
				// todo 这里有bug
				
				if (tumor.markVoxel[resultIndex[j]] == 1) {
					count1++;
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
	std::cout << "count1 = " << count1 << std::endl;
	std::cout << "edgeVoxelsIndex.size() " << edgeVoxelsIndex.size() << std::endl;

	// 二维平面参数计算
	glm::vec3 origin = glm::vec3(-10000.0f, -10000.0f, -10000.0f);
	glm::vec3 originNormal;
	glm::vec3 originRight; // 任意一个法向量的垂直方向作为y+
	// 随便选择一个点做原点
	// 记录体素位置
	voxelTest.clear();
	vertexTest.clear();
	int count2 = 0;
	for (int i = 0; i < static_cast<int>(resolution.x * resolution.y * resolution.z); i++) {
		//std::cout << "[ERROR] i " << i << std::endl;
	
		if (tumor.markVoxel[i] == 2) {
			count2++;
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			//std::cout << "output: " << ix << " " << iy << " " << iz << " index: " << i << std::endl;
			glm::vec3 coor = tumor.boxMin + glm::vec3(ix * tumor.getStep(), iy * tumor.getStep(), iz * tumor.getStep());
			// 确定原点
			if (origin == glm::vec3(-10000.0f, -10000.0f, -10000.0f)) {
				origin = coor;
				originNormal = glm::normalize(removePos - origin);
				originRight = glm::normalize(glm::cross(originNormal, glm::vec3(0,0,1)));
				voxelTest.push_back(origin);
			}
			else {
				// 计算平面上的坐标: 弧长
				GLfloat rho = acos (glm::dot(originNormal, glm::normalize(coor - origin))) * cutRadius;
				// 和纵轴夹角（y正半轴
				glm::vec3 OP = coor - origin;
				glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
				glm::vec3 OQ = OP - OH;
				GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
				// 世界坐标
				//if (glm::distance(coor, removePos) < 1.2f* cutRadius) {
					voxelTest.push_back(coor);
				//}
				

				// 二维坐标：x = rho*sin theta, y = rho * cos theta.
				//voxelTest.push_back(glm::vec3(rho * sin(theta), rho * cos(theta), 0.0f));
				// std::cout << "voxelTest - rho: " << acos(glm::dot(originNormal, glm::normalize(coor - origin))) / 3.14 * 180 << " theta: " << theta << std::endl;
				// std::cout << rho * sin(theta) << " " << rho * cos(theta) << std::endl;

			}
			// 恢复1
			tumor.markVoxel[i] = 1;
		}
	}
	std::cout << "voxelTest.size() " << voxelTest.size() << " count2 " << count2 << std::endl;
	// 删去网格顶点
	std::vector<GLfloat> tmpVertices(*tumor.getVertices());
	std::vector<int> tmpIndices(*tumor.getIndices());
	std::vector<int> tobeDelete;
	std::map<int, int> updateIndices;
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		glm::vec3 curVer = glm::vec3(tmpVertices[i * 6], tmpVertices[i * 6 + 1], tmpVertices[i * 6 + 2]);
		GLfloat dis = glm::distance(curVer, removePos);
		if (dis < cutRadius) {
			// 记录需要删除的点的index
			tobeDelete.push_back(i);
		}
	}
		
	int* newIndices = new int[tmpVertices.size()/6]; // 前后indices的映射关系
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

	// 找到所有边缘indices

	std::set<int> edgeIndices;
	for (int i = 0; i < tmpIndices.size() / 3; i++) {
		//记录和移除点相连的三角形
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// 对是-1的检测之前是否加入，其他的加入
			for (int j = 0; j < 3; j++) {
				if (newIndices[tmpIndices[i * 3 + j]] != -1) {
					// 直接放入映射后的index
					// debug
					edgeIndices.insert(newIndices[tmpIndices[i * 3 + j]]);
					//edgeIndices.insert(tmpIndices[i * 3 + j]);
				}
			}
		}
	}

	// 记录边缘点的index和本身的position
	// 更新indices
	for (int i = 0; i < tmpIndices.size()/3; ) {
		// 删除和移除点相连的三角形
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3+1]] == -1 || newIndices[tmpIndices[i * 3+2]] == -1) {
			// 
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

	// 计算edgeIndices对应的位置在二维空间的坐标
	for (std::set<int>::iterator it = edgeIndices.begin(); it != edgeIndices.end(); it++) {
		glm::vec3 coor = glm::vec3(tmpVertices[(*it) * 6], tmpVertices[(*it) * 6 + 1], tmpVertices[(*it) * 6 + 2]);
		// 球面距离
		GLfloat rho = 1.1 * acos(glm::dot(originNormal, glm::normalize(coor - origin))) * cutRadius;
		// 和纵轴夹角（y正半轴
		glm::vec3 OP = coor - origin;
		glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
		glm::vec3 OQ = OP - OH;
		GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));

		// 二维坐标：x = rho*sin theta, y = rho * cos theta.
		// std::cout << "vertexTest - rho: " << acos(glm::dot(originNormal, glm::normalize(coor - origin))) / 3.14 * 180<< " theta: " << theta << std::endl;
		//std::cout << rho * sin(theta) << " " << rho * cos(theta) << std::endl;
		// vertexTest.push_back(glm::vec3(rho* sin(theta), rho* cos(theta), 0.0f));
		vertexTest.push_back(coor);
	}
	
	tumor.setVertices(tmpVertices);
	tumor.setIndices(tmpIndices);
	std::cout << "vertexTest.size() " << vertexTest.size() << std::endl;
	
}

void Area::testCoorTrans(Shader & shader) {
	// todo 寻找边缘体素的映射关系还是不对
	shader.use();
	shader.setInt("withShadow", 0);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightPos", camera.Position);
	shader.setInt("cut", 0);
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	glm::mat4 model(1.0f);

	// draw edge vertices 网格边缘 - 红色 
	for (int i = 0; i < vertexTest.size(); i++) {
		shader.setVec3("color", glm::vec3(1.0f,0.0f,0.0f));
		model = glm::translate(glm::mat4(1.0f), vertexTest[i]);
		model = transformMat * glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", model);
		mySphere.draw();
	}

	// draw edge voxel 体素边缘 - 绿色
	for (int i = 0; i < voxelTest.size(); i++) {
		shader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
		model = transformMat * glm::translate(glm::translate(glm::mat4(1.0f), voxelTest[i]), glm::vec3(-4.38f, -201.899f, 148.987f));

		//model = glm::translate(glm::mat4(1.0f), voxelTest[i]);
		//model = transformMat * glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", model);
	    mySphere.draw();
	}
}
// 通过键盘激活
void Area::removeTumor(BaseModel& tumor) {
	updateCutVertices(tumor);
	tumor.initVertexObject();
	removeMode = 0;
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
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	shadowShader.setMat4("model", transformMat * model);
	/*for (int i = 0; i < modelsID.size(); i++) {
		models[modelsID[i]].draw();
	}*/
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
	shader.setVec4("plane", planeCoeff);
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	std::vector<glm::vec3>* tmpVoxelPos = models[1].getVoxelsPos();
	glm::mat4 model(1.0f);
	if (modeSelection == REMOVE_TUMOR) {
		
		if (tmpVoxelPos->size() != 0) {// 有体素坐标
			for (int j = 0; j < tmpVoxelPos->size(); j++) {
				shader.setVec3("color", models[1].getColor());
				model = glm::scale(transformMat * glm::translate(glm::translate(glm::mat4(1.0f), (*tmpVoxelPos)[j]), glm::vec3(-4.38f, -201.899f, 148.987f)), glm::vec3(models[1].getStep() / 1.2, models[1].getStep() / 1.2, models[1].getStep() / 1.2));
				shader.setMat4("model", model);
				mySphere.draw();
			}
		}
	}
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
	shader.setInt("withLight", 1);
	shader.setInt("isPlane", 0);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setVec3("color", models[modelsID[i]].getColor());
	//	models[modelsID[i]].draw();
	}
	shader.setVec3("color", models[modelsID[1]].getColor());
	models[modelsID[1]].draw();
}

void Area::drawCutFace(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models) {
	shadowShader.use();
	shadowShader.setInt("cut", 1);
	renderDepthBuffer(shadowShader, models);
	// left part
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	shader.setInt("withLight", 1);
	shader.setInt("isPlane", 0);
	shader.setVec4("plane", planeCoeff);
	shader.setMat4("projection", camera.getOrthology());
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	glViewport(viewportPara[0], viewportPara[1], viewportPara[2] / 2, viewportPara[3]);
	shader.setInt("cut", 1);
	shader.setMat4("model", transformForCut[0] * model);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setVec3("color", models[modelsID[i]].getColor());
		models[modelsID[i]].draw();
	}


	shadowShader.use();

	shadowShader.setInt("cut", 2);
	renderDepthBuffer(shadowShader, models);
	// right part
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glViewport(viewportPara[0] + viewportPara[2] / 2, viewportPara[1], viewportPara[2] / 2, viewportPara[3]);
	shader.setInt("cut", 2);
	shader.setMat4("model", transformForCut[1] * model);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setVec3("color", models[modelsID[i]].getColor());
		models[modelsID[i]].draw();
	}
}

void Area::drawSelectedFace(Shader & shader) {
	shader.use();
	// upper part 
	shader.setInt("cut", 2);
	glViewport(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
	shader.setVec4("plane", planeCoeff);
	shader.setMat4("projection", camera.getOrthology());
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	// models
	shader.setInt("withLight", 0);
	shader.setInt("isPlane", 1);
	shader.setMat4("model", transformMat * model);
	csPlane.draw();
}

void Area::drawRuler(Shader & textureShader, Shader& shader) {
	// ruler
	textureShader.use();
	textureShader.setVec3("viewPos", camera.Position);
	textureShader.setVec3("lightPos", camera.Position);
	textureShader.setMat4("projection", camera.getOrthology());
	textureShader.setMat4("view", camera.GetViewMatrix());
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, ruler.position);
	model = glm::rotate(model, ruler.rotateAngle, glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(ruler.scaleSize * 20, ruler.scaleSize * 20, 1));
	textureShader.setMat4("model", model);
	ruler.generateTexture();
	ruler.draw(textureShader);

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
	shader.setInt("cut", 0);
	shader.setInt("withLight", 1);
	shader.setInt("isPlane", 0);
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

void Area::updateRuler(const glm::vec3& pos1, const glm::vec3& pos2) { // 输入参数为世界坐标
	// calculate ruler attributes
	ruler.distance = glm::distance(pos1, pos2);
	ruler.position = (pos1 + pos2) / 2.0f;
	ruler.ends[0] = pos1;
	ruler.ends[1] = pos2;
	/*if (modeSelection == NEAREST_VESSEL) {
		ruler.ends[0].z = pos1.z - 10.0f;
		ruler.ends[1].z = pos1.z - 10.0f;
		ruler.position.z = pos1.z - 10.0f;
	}
	else {*/
	ruler.ends[0].z = ruler.CUTFACE;
	ruler.ends[1].z = ruler.CUTFACE;
	ruler.position.z = ruler.CUTFACE;
	//}
	GLfloat projLen = glm::distance(pos1, pos2);
	ruler.scaleSize = projLen / ruler.distance;

	ruler.rotateAngle = atan((pos1.y - pos2.y) / (pos1.x - pos2.x));
}

void Area::setRulerVertex(const glm::vec3 & vertexPosition) {
	glm::vec3 localPos = glm::vec4(vertexPosition, 1.0f);
	selectedEnds[currentRulerIndex] = localPos;
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

void Area::setCutFaceVertex(const glm::vec3 & vertexPosition) {
	// get local pos
	glm::vec3 localPos = glm::vec3(1.0f);
	localPos = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f))) * glm::inverse(transformMat) * glm::vec4(vertexPosition, 1.0f);
	tmpCutFaceVertices[currentCutFaceIndex] = localPos;
	transCutFaceVertices[currentCutFaceIndex] = localPos;
	if (currentCutFaceIndex == 0) {
		currentCutFaceIndex = 1;
	}
	else if (currentCutFaceIndex == 1) {
		currentCutFaceIndex = 2;
	}
	else if (currentCutFaceIndex == 2) {
		currentCutFaceIndex = 0;
		calculatePlane();
		cutMode = 2;
	}
}

void Area::calculatePlane() {
	if (testPlane != NULL) {
		delete testPlane;
	}
	GLfloat a = (transCutFaceVertices[1].y - transCutFaceVertices[0].y) * (transCutFaceVertices[2].z - transCutFaceVertices[0].z) - (transCutFaceVertices[2].y - transCutFaceVertices[0].y) * (transCutFaceVertices[1].z - transCutFaceVertices[0].z);
	GLfloat b = (transCutFaceVertices[1].z - transCutFaceVertices[0].z) * (transCutFaceVertices[2].x - transCutFaceVertices[0].x) - (transCutFaceVertices[2].z - transCutFaceVertices[0].z) * (transCutFaceVertices[1].x - transCutFaceVertices[0].x);
	GLfloat c = (transCutFaceVertices[1].x - transCutFaceVertices[0].x) * (transCutFaceVertices[2].y - transCutFaceVertices[0].y) - (transCutFaceVertices[2].x - transCutFaceVertices[0].x) * (transCutFaceVertices[1].y - transCutFaceVertices[0].y);
	GLfloat d = -a * transCutFaceVertices[0].x - b * transCutFaceVertices[0].y - c * transCutFaceVertices[0].z;
	planeCoeff[0] = a;
	planeCoeff[1] = b;
	planeCoeff[2] = c;
	planeCoeff[3] = d;
	// test part
	std::vector<GLfloat> tmpVec = {
		transCutFaceVertices[0].x, transCutFaceVertices[0].y, transCutFaceVertices[0].z,
		transCutFaceVertices[1].x, transCutFaceVertices[1].y, transCutFaceVertices[1].z,
		transCutFaceVertices[2].x, transCutFaceVertices[2].y, transCutFaceVertices[2].z
	};
	planeCoeff = glm::normalize(planeCoeff);
	csPlane.setCoeff(planeCoeff);	
}

void Area::displayGUI() {
	ImGui::RadioButton("General", &modeSelection, GENERAL);
	ImGui::RadioButton("Ruler", &modeSelection, RULER);
	ImGui::RadioButton("Cut", &modeSelection, CROSS_INTERSECTION);
	ImGui::RadioButton("Nearest Vessel", &modeSelection, NEAREST_VESSEL);
	ImGui::RadioButton("Rmove Tumor", &modeSelection, REMOVE_TUMOR);
	//std::cout << modeSelection << std::endl;
	if (modeSelection == 3) {
		if (cutMode == 1) {
			ImGui::Text("selecting");
		}
		else if (cutMode == 2) {
			ImGui::Text("press C to confirm");
		}
		else {
			ImGui::Text("confirmed");
		}
	}
	else if (modeSelection == REMOVE_TUMOR) {
		if (removeMode == 1) { // 调节半径
			std::string str = "Radius: " + std::to_string(cutRadius);
			ImGui::Text(str.c_str());
		}
	}

}

void Area::setLocalCoordinate(glm::vec3 worldCoor, BaseModel & vessel) {
	NVLocalPos = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f))) * glm::inverse(transformMat) * glm::vec4(worldCoor, 1.0f);
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


	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f));
	model = transformMat * model;
	updateRuler(model * glm::vec4(nearestPos, 1.0f), model * glm::vec4(NVLocalPos, 1.0f));
	

}