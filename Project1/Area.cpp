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
	debugIndex = -1;
	// ruler
	ruler.initVertexObject();
	// cut
	csPlane.initVertexObject();
	cutMode = 1; // general

	// nearest vessel
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
	else if (removeMode == 1) { // ��ȡ���λ��ת��
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
	
	// local coordinate
	removePos = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f))) * glm::inverse(transformMat) * glm::vec4(pos, 1.0f);
	removeMode = 1;
}

void Area::addPoint() {
	lineVec.clear();
	std::cout << "add point " << debugIndex << std::endl;
	// debug ����
	/*for (int i = 0; i < combinedPoints.size()/3; i++) {
		mesh1.addPoint(combinedPoints[i]);

	}*/
	mesh1.addPoint(combinedPoints[++debugIndex]);
	std::cout << "coor: " << combinedPoints[debugIndex].x << " " << combinedPoints[debugIndex].y << std::endl;
	std::vector<GLfloat> lineVertices;
	lineVertices.clear();
	//std::cout << " mesh1.triangleVector.size() " << mesh1.triangleVector.size() << std::endl;
	// test������
	for (int i = 0; i < mesh1.triangleVector.size(); i++) {
		lineVertices.clear();
		// std::cout << "in debug ";
		for (int j = 0; j < 3; j++) {
			// std::cout << mesh1.triangleVector[i].vertices[j] << " ";
			lineVertices.push_back(mesh1.pointVector[mesh1.triangleVector[i].vertices[j]].x);
			lineVertices.push_back(mesh1.pointVector[mesh1.triangleVector[i].vertices[j]].y);
			lineVertices.push_back(200.0f);
		}
		// ʹ���߶αպ�
		// lineVertices.push_back(mesh1.pointVector[mesh1.triangleVector[i].vertices[0]].x);
		// lineVertices.push_back(mesh1.pointVector[mesh1.triangleVector[i].vertices[0]].y);
		// lineVertices.push_back(100.0f);
		MyLine line(lineVertices);
		lineVec.push_back(line);
	}
}
/* 
 * ����������涥���и�
 */
void Area::planA(BaseModel& tumor) {
	// �洢����֮���vertices��indices
	std::vector<GLfloat> tmpVertices(*tumor.getVertices());
	std::vector<int> tmpIndices(*tumor.getIndices());

	// ��¼Ҫɾ�������񶥵�
	std::vector<int> tobeDelete;
	// �ڶ����Ƴ�ǰ�󣬶�Ӧ�±ߵı仯
	std::map<int, int> updateIndices;

	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		glm::vec3 curVer = glm::vec3(tmpVertices[i * 6], tmpVertices[i * 6 + 1], tmpVertices[i * 6 + 2]);
		// ��������и�����
		GLfloat dis = glm::distance(curVer, removePos);
		if (dis < cutRadius) {
			// ��¼��Ҫɾ���ĵ���vertices��index
			tobeDelete.push_back(i);
		}
	}

	// ����ǰ��indices��ӳ���ϵ
	int* newIndices = new int[tmpVertices.size() / 6];
	// ��ʼ��
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		newIndices[i] = i;
	}
	// �����±�
	for (int i = 0, j = 0; i < tmpVertices.size() / 6 && j < tobeDelete.size(); i++) {
		if (i == tobeDelete[j]) {
			j++;
			// ֮����±����
			newIndices[i] = -1;
			for (int k = i + 1; k < tmpVertices.size() / 6; k++) {
				newIndices[k] -= 1;
			}
		}
	}

	// �����Ե���㹹�ɵ�ƽ���ƽ��������
	glm::vec3 averNormal = glm::vec3(0, 0, 0); //��Ե�����ƽ��������
	std::set<int> edgeIndices; // ��¼��Ե�����ڡ����º�vertices�е��±�
	for (int i = 0; i < tmpIndices.size() / 3; i++) {
		//��¼���Ƴ���������������
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// ����-1�ļ��֮ǰ�Ƿ���룬�����ļ���
			for (int j = 0; j < 3; j++) {
				if (newIndices[tmpIndices[i * 3 + j]] != -1) {
					// ֱ�ӷ���ӳ����index
					edgeIndices.insert(newIndices[tmpIndices[i * 3 + j]]);
					averNormal += glm::vec3(tmpVertices[tmpIndices[i * 3 + j] * 6 + 3], tmpVertices[tmpIndices[i * 3 + j] * 6 + 4], tmpVertices[tmpIndices[i * 3 + j] * 6 + 5]);
				}
			}
		}
	}

	// ����indices
	for (int i = 0; i < tmpIndices.size() / 3; ) {
		// ɾ�����Ƴ���������������
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// ɾ��
			tmpIndices.erase(tmpIndices.begin() + i * 3 + 2);
			tmpIndices.erase(tmpIndices.begin() + i * 3 + 1);
			tmpIndices.erase(tmpIndices.begin() + i * 3);
		}
		// ����δɾ�������ε�����
		else {
			tmpIndices[i * 3] = newIndices[tmpIndices[i * 3]];
			tmpIndices[i * 3 + 1] = newIndices[tmpIndices[i * 3 + 1]];
			tmpIndices[i * 3 + 2] = newIndices[tmpIndices[i * 3 + 2]];
			i++;
		}
	}
	delete[] newIndices;
	// ɾ����Ӧ��vertices
	for (int i = 0; i < tobeDelete.size(); i++) {
		int index = (tobeDelete[i] - i) * 6;
		tmpVertices.erase(tmpVertices.begin() + index + 5);
		tmpVertices.erase(tmpVertices.begin() + index + 4);
		tmpVertices.erase(tmpVertices.begin() + index + 3);
		tmpVertices.erase(tmpVertices.begin() + index + 2);
		tmpVertices.erase(tmpVertices.begin() + index + 1);
		tmpVertices.erase(tmpVertices.begin() + index);
	}


	// �����и���ı��涥������
	// �������� = vertices + ԭ��λ��
	// ԭ��λ�� = cutPoint
	// vertices: ��MySphere����
	// ����������и�λ������Ķ���
	std::vector<GLfloat> ballVertices = *mySphere.getVertices();
	glm::mat4 simuModel = glm::scale(glm::translate(glm::mat4(1.0f), removePos), glm::vec3(cutRadius, cutRadius, cutRadius)); // scale -> translate transpose(inverse(mat3(model)))
	for (int i = 0; i < ballVertices.size(); i += 6) {
		glm::vec3 curVertices = glm::vec3(ballVertices[i + 0], ballVertices[i + 1], ballVertices[i + 2]);
		glm::vec3 transVertices = simuModel * glm::vec4(curVertices, 1.0f);
		ballVertices[i + 0] = transVertices.x;
		ballVertices[i + 1] = transVertices.y;
		ballVertices[i + 2] = transVertices.z;
	}

	// �������������е���Ϊ��ά�ռ�ԭ��
	int originIndex = -1;
	GLfloat maxDot = -1;

	// ���ԭ���±�
	for (int i = 0; i < ballVertices.size(); i += 6) {
		glm::vec3 curVer = glm::vec3(ballVertices[i], ballVertices[i + 1], ballVertices[i + 2]);
		glm::vec3 tmpNormal = removePos - curVer;
		if (glm::dot(tmpNormal, averNormal) > maxDot) {
			maxDot = glm::dot(tmpNormal, averNormal);
			originIndex = i / 6;
		}
	}

	// ���涥���й����и�ƽ��Ĳ���

	glm::vec3 origin = glm::vec3(-10000.0f, -10000.0f, -10000.0f);
	glm::vec3 originNormal;
	glm::vec3 originRight; // ����һ���������Ĵ�ֱ������Ϊx+
	glm::vec3 originUp;
	// ����ԭ������
	if (originIndex != -1) {
		origin = glm::vec3(ballVertices[originIndex * 6], ballVertices[originIndex * 6 + 1], ballVertices[originIndex * 6 + 2]);
		// ������
		std::cout << "origin: " << origin.x << " " << origin.y << " " << origin.z << std::endl;
		originNormal = glm::normalize(removePos - origin);
		std::cout << "originNormal: " << originNormal.x << " " << originNormal.y << " " << originNormal.z << std::endl;
		originRight = glm::normalize(glm::cross(originNormal, glm::vec3(0, 0, 1)));
		std::cout << "originRight: " << originRight.x << " " << originRight.y << " " << originRight.z << std::endl;
		originUp = glm::normalize(glm::cross(originNormal, originRight));
		verticesToAppend2D.push_back(glm::vec2(0.0f, 0.0f));
		verticesToAppend3D.push_back(origin);
	}
	else {
		std::cout << "FAIL TO FIND ORIGIN" << std::endl;
		return;
	}

	// ɸѡ��������桿������
	for (int i = 0; i < ballVertices.size(); i += 6) {
		// ��Ϊ�ֲ�����
		glm::vec3 curVer = glm::vec3(ballVertices[i], ballVertices[i + 1], ballVertices[i + 2]);
		// ��curVer��originRight�ĵ��
		GLfloat dot = glm::dot(glm::normalize(curVer - removePos), originNormal);
		// ������°���
		if (dot < 0) {
			// ����ֲ���������
			verticesToAppend3D.push_back(curVer);

			// �����ά�ռ�����
			// ����
			GLfloat rho = 3 * acos(glm::dot(originNormal, glm::normalize(curVer - origin))) * cutRadius;
			// ������нǣ�y������
			glm::vec3 OP = curVer - origin;
			glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
			glm::vec3 OQ = OP - OH;
			GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
			if (glm::dot(glm::normalize(OQ), originUp) < 0) {
				theta = -theta;
			}
			std::cout << "3D: " << curVer.x << " " << curVer.y << " " << curVer.z<< std::endl;
			for (int k = 0; k < verticesToAppend2D.size(); k++) {
				if (abs(verticesToAppend2D[k].x - rho * sin(theta)) < 0.000001f && abs(verticesToAppend2D[k].y - rho * cos(theta)) < 0.000001f) {
					std::cout << "-------------------------������������ڶ�ά�ռ���ͬ:" << std::endl;
				}
			}
			verticesToAppend2D.push_back(glm::vec2(rho * sin(theta), rho * cos(theta)));
			std::cout << "2D: " << rho * sin(theta) << " " << rho * cos(theta) << std::endl;
		}
	}

	std::cout << "verticesToAppend3D.size() " << verticesToAppend3D.size() << std::endl;

	// ɸѡ����Ե������
	std::vector<int> orderedIndices;
	for (std::set<int>::iterator it = edgeIndices.begin(); it != edgeIndices.end(); it++) {
		glm::vec3 coor = glm::vec3(tmpVertices[(*it) * 6], tmpVertices[(*it) * 6 + 1], tmpVertices[(*it) * 6 + 2]);
		// �������
		GLfloat rho =  //2 * cutRadius;
		  3*2*acos(glm::dot(originNormal, glm::normalize(coor - origin))) * cutRadius;
		// ������нǣ�y������
		glm::vec3 OP = coor - origin;
		glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
		glm::vec3 OQ = OP - OH;
		GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
		if (glm::dot(glm::normalize(OQ), originUp) < 0) {
			theta = -theta;
		}
		// ��ά���꣺x = rho*sin theta, y = rho * cos theta.
		// todo �����Ϊ���ȷֲ���͹��...
		vertex2D.push_back(glm::vec2(rho * sin(theta), rho * cos(theta)));
		vertex3D.push_back(coor);
		orderedIndices.push_back(*it);
	}

	int oldVerticesSize = tmpVertices.size();

	// -------------ԭ���Ķ�����������µĶ���-----------------
	// ��verticesToAppend����vertices֮��
	for (int i = 0; i < verticesToAppend3D.size(); i++) {
		tmpVertices.push_back(verticesToAppend3D[i].x);
		tmpVertices.push_back(verticesToAppend3D[i].y);
		tmpVertices.push_back(verticesToAppend3D[i].z);
		// normal
		glm::vec3 normal = origin - verticesToAppend3D[i];
		tmpVertices.push_back(normal.x);
		tmpVertices.push_back(normal.y);
		tmpVertices.push_back(normal.z);
	}

	// -------------�ڶ�ά�ռ乹�������ʷ�-----------------
	// Ҫ��voxel��ǰ,vertex�ں�
	combinedPoints.assign(verticesToAppend2D.begin(), verticesToAppend2D.end());
	combinedPoints.insert(combinedPoints.end(), vertex2D.begin(), vertex2D.end());
	std::cout << "combinedPoints.size() " << combinedPoints.size() << std::endl;
	myUtils::generateMesh1(combinedPoints, mesh1, verticesToAppend2D.size());
	// -------------���������ʷֵĽ��-----------------
	// ����indices��mesh�õ����±�ȫ��-3
	// ����mesh��Ӧ���±꣬����indices��vertex����edgeIndices��vertices3Dֱ�������
	/*for (int i = 0; i < mesh.triangleVector.size(); i++) {
		// ������Ǳ�Ե��ɾ��
		if (mesh.triangleVector[i].vertices[0] - 3 >= verticesToAppend2D.size()
			&& mesh.triangleVector[i].vertices[1] - 3 >= verticesToAppend2D.size()
			&& mesh.triangleVector[i].vertices[2] - 3 >= verticesToAppend2D.size()) {
			i++;
			continue;
		}
		for (int j = 0; j < 3; j++) {
			
			// �������������
			if (mesh.triangleVector[i].vertices[j] - 3 < verticesToAppend2D.size()) {
				// vertices3D��vertices���±�Ϊ��ԭʼ���ȣ��Ƴ��󣬲���ǰ�� + �ںϲ��е�λ��
				tmpIndices.push_back(oldVerticesSize / 6 + mesh.triangleVector[i].vertices[j] - 3);
			}
			// �����vertex
			else {
				// vertex���±�ΪedgeIndices
				tmpIndices.push_back(orderedIndices[mesh.triangleVector[i].vertices[j] - 3 - verticesToAppend2D.size()]);
			}
		}
	}*/

	/*tumor.setVertices(tmpVertices);
	tumor.setIndices(tmpIndices);*/
	// ���������ʷ�
	
}
void Area::planB(BaseModel& tumor) {
	std::cout << "This is plan B" << std::endl;
	glm::vec3 resolution = tumor.getResolution();
	// ��¼Ҫɾ�������񶥵�
	std::vector<GLfloat> tmpVertices(*tumor.getVertices());
	std::vector<int> tmpIndices(*tumor.getIndices());
	std::vector<int> tobeDelete;
	std::map<int, int> updateIndices;
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		glm::vec3 curVer = glm::vec3(tmpVertices[i * 6], tmpVertices[i * 6 + 1], tmpVertices[i * 6 + 2]);
		GLfloat dis = glm::distance(curVer, removePos);
		if (dis < cutRadius) {
			// ��¼��Ҫɾ���ĵ���vertices��index
			tobeDelete.push_back(i);
		}
	}

	// ����ǰ��indices��ӳ���ϵ
	int* newIndices = new int[tmpVertices.size()/6];
	for (int i = 0; i < tmpVertices.size() / 6; i++) {
		newIndices[i] = i;
	}
	for (int i = 0, j = 0; i < tmpVertices.size() / 6 && j < tobeDelete.size(); i++) {
		if (i == tobeDelete[j]) {
			j++;
			// ֮����±����
			newIndices[i] = -1;
			for (int k = i + 1; k < tmpVertices.size() / 6; k++) {
				newIndices[k] -= 1;
			}
		}
	}

	// �����Ե���㹹�ɵ�ƽ���ƽ��������
	glm::vec3 averNormal = glm::vec3(0, 0, 0);//��Ե�����ƽ��������
	std::set<int> edgeIndices;
	for (int i = 0; i < tmpIndices.size() / 3; i++) {
		//��¼���Ƴ���������������
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// ����-1�ļ��֮ǰ�Ƿ���룬�����ļ���
			for (int j = 0; j < 3; j++) {
				if (newIndices[tmpIndices[i * 3 + j]] != -1) {
					// ֱ�ӷ���ӳ����index
					edgeIndices.insert(newIndices[tmpIndices[i * 3 + j]]);
					averNormal += glm::vec3(tmpVertices[tmpIndices[i * 3 + j] * 6 + 3], tmpVertices[tmpIndices[i * 3 + j] * 6 + 4], tmpVertices[tmpIndices[i * 3 + j] * 6 + 5]);
				}
			}
		}
	}

	// תΪvector

	// ����indices
	for (int i = 0; i < tmpIndices.size() / 3; ) {
		// ɾ�����Ƴ���������������
		if (newIndices[tmpIndices[i * 3]] == -1 || newIndices[tmpIndices[i * 3 + 1]] == -1 || newIndices[tmpIndices[i * 3 + 2]] == -1) {
			// ɾ��
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
	// ɾ����Ӧ��vertices
	for (int i = 0; i < tobeDelete.size(); i++) {
		int index = (tobeDelete[i] - i) * 6;
		tmpVertices.erase(tmpVertices.begin() + index + 5);
		tmpVertices.erase(tmpVertices.begin() + index + 4);
		tmpVertices.erase(tmpVertices.begin() + index + 3);
		tmpVertices.erase(tmpVertices.begin() + index + 2);
		tmpVertices.erase(tmpVertices.begin() + index + 1);
		tmpVertices.erase(tmpVertices.begin() + index);
	}

	// ������averNormal���������������Ϊ��ά�ռ�ԭ��
	int originIndex = -1;
	GLfloat maxDot = -1;

	// �ֲ�����ϵλ��
	std::vector<glm::vec3>* voxelsPos = tumor.getVoxelsPos();
	// ���ؿռ���±�
	std::vector<glm::vec3>* voxelsIndex = tumor.getVoxelsIndex();
	std::set<int> edgeVoxelsIndex;
	// ��Ǳ�Ե����
	for (int i = 0; i < voxelsPos->size(); ) {
		// �ж��Ƿ���������
		GLfloat dis = glm::distance((*voxelsPos)[i], removePos);
		if (dis < cutRadius) {
			// �Ƴ��ֲ�����
			voxelsPos->erase(voxelsPos->begin() + i);
			// ����������ĵ��±�
			int tmpIndex = (voxelsIndex->begin() + i)->y * resolution.x * resolution.z + (voxelsIndex->begin() + i)->z * resolution.x + (voxelsIndex->begin() + i)->x;
			// ���±������
			tumor.markVoxel[tmpIndex] = 0;
			// 26����
			std::vector<int> resultIndex;
			myUtils::neighbors26(*(voxelsIndex->begin() + i), resolution, resultIndex);
			// ���������Ϊ1����result�еı�Ϊ2��������ȻΪ1

			for (int j = 0; j < resultIndex.size(); j++) {
				// todo ������bug

				if (tumor.markVoxel[resultIndex[j]] == 1) {
					// ����Ӧλ�ñ��Ϊ2
					tumor.markVoxel[resultIndex[j]] = 2;
					// ��¼λ������
					edgeVoxelsIndex.insert(resultIndex[j]);
				}
			}
			voxelsIndex->erase(voxelsIndex->begin() + i);
		}
		else {
			i++;
		}
	}
	// �ڱ�Ե�����м���ԭ������
	glm::vec3 origin = glm::vec3(-10000.0f, -10000.0f, -10000.0f);
	glm::vec3 originNormal;
	glm::vec3 originRight; // ����һ���������Ĵ�ֱ������Ϊx+
	glm::vec3 originUp;
	for (int i = 0; i < static_cast<int>(resolution.x * resolution.y * resolution.z); i++) {
		if (tumor.markVoxel[i] == 2) {
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			//std::cout << "output: " << ix << " " << iy << " " << iz << " index: " << i << std::endl;
			glm::vec3 coor = tumor.boxMin + glm::vec3(ix * tumor.getStep(), iy * tumor.getStep(), iz * tumor.getStep());
			glm::vec3 tmpNormal = glm::normalize(removePos - coor);
			// ȷ��ԭ��
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
		std::cout << "origin: " << origin.x << " " << origin.y << " " << origin.z << std::endl;
		// ������
		originNormal = glm::normalize(removePos - origin);
		originRight = glm::normalize(glm::cross(originNormal, glm::vec3(0, 0, 1)));
		originUp = glm::normalize(glm::cross(originNormal, originRight));
		voxel2D.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
		voxel3D.push_back(origin);
		tumor.markVoxel[originIndex] = 1;
	}
	else {
		std::cout << "FAIL TO FIND ORIGIN" << std::endl;
		return;
	}

	// �����Ե���صĶ�ά����
	for (int i = 0; i < static_cast<int>(resolution.x * resolution.y * resolution.z); i++) {
		if (tumor.markVoxel[i] == 2) {
			int iy = i / (resolution.x * resolution.z);
			int iz = (i - iy * resolution.x * resolution.z) / (resolution.x);
			int ix = i - iy * resolution.x * resolution.z - iz * resolution.x;
			//
			glm::vec3 coor = tumor.boxMin + glm::vec3(ix * tumor.getStep(), iy * tumor.getStep(), iz * tumor.getStep());

			// ����ƽ���ϵ�����: ����
			GLfloat rho = 2*acos(glm::dot(originNormal, glm::normalize(coor - origin))) * cutRadius;
			// ������нǣ�y������
			glm::vec3 OP = coor - origin;
			glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
			glm::vec3 OQ = OP - OH;
			GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
			if (glm::dot(glm::normalize(OQ), originUp) < 0) {
				theta = -theta;
			}
			// ��������
			voxel3D.push_back(coor);
			// ��ά���꣺x = rho*sin theta, y = rho * cos theta.
			voxel2D.push_back(glm::vec3(rho * sin(theta), rho * cos(theta),200.0f));
			// �ָ�1
			tumor.markVoxel[i] = 1;
		}
	}
	std::cout << "voxel3D.size()" << voxel3D.size() << std::endl;
	// �����Ե����Ķ�ά�ռ�����
	std::vector<int> orderedIndices;
	for (std::set<int>::iterator it = edgeIndices.begin(); it != edgeIndices.end(); it++) {
		glm::vec3 coor = glm::vec3(tmpVertices[(*it) * 6], tmpVertices[(*it) * 6 + 1], tmpVertices[(*it) * 6 + 2]);
		// �������
		GLfloat rho = 5 * cutRadius;
		// 5*acos(glm::dot(originNormal, glm::normalize(coor - origin))) * cutRadius;
	// todo͹����ͬʱ�ؽ�˳��
	// ������нǣ�y������
		glm::vec3 OP = coor - origin;
		glm::vec3 OH = glm::dot(OP, originNormal) * originNormal;
		glm::vec3 OQ = OP - OH;
		GLfloat theta = acos(glm::dot(glm::normalize(OQ), originRight));
		if (glm::dot(glm::normalize(OQ), originUp) < 0) {
			theta = -theta;
		}
		// ��ά���꣺x = rho*sin theta, y = rho * cos theta.
		// todo�����Ϊ���ȷֲ���͹��...
		vertex2D.push_back(glm::vec2(rho * sin(theta), rho * cos(theta)));
		vertex3D.push_back(coor);
		orderedIndices.push_back(*it);
	}
	
	int oldVerticesSize = tmpVertices.size()/6;
	// ��verticesToAppend����vertices֮��
	for (int i = 0; i <  voxel3D.size(); i++) {
		tmpVertices.push_back(voxel3D[i].x);
		tmpVertices.push_back(voxel3D[i].y);
		tmpVertices.push_back(voxel3D[i].z);
		// normal
		glm::vec3 normal = origin - voxel3D[i];
		tmpVertices.push_back(normal.x);
		tmpVertices.push_back(normal.y);
		tmpVertices.push_back(normal.z);
	}


	// -------------�ڶ�ά�ռ乹�������ʷ�-----------------
	std::vector<glm::vec2> combinedPoints;// Ҫ��voxel��ǰ,vertex�ں�
	combinedPoints.assign(voxel2D.begin(), voxel2D.end());
	combinedPoints.insert(combinedPoints.end(), vertex2D.begin(), vertex2D.end());
	myUtils::generateMesh(combinedPoints, mesh, voxel2D.size());

	std::cout << "voxel3D.size()" << voxel3D.size() << std::endl;
	// mesh�õ����±�ȫ��-3
	// ����mesh��Ӧ���±꣬����indices��vertex����edgeIndices��voxelֱ�������
	for (int i = 0; i < mesh.triangleVector.size(); i++) {
		// �����㶼Ϊ������ȥ��
		if (mesh.triangleVector[i].vertices[0] - 3 >= voxel2D.size()
			&& mesh.triangleVector[i].vertices[1] - 3 >= voxel2D.size()
			&& mesh.triangleVector[i].vertices[2] - 3 >= voxel2D.size()) {

		}
		else {
			for (int j = 0; j < 3; j++) {
				// �������������
				if (mesh.triangleVector[i].vertices[j] - 3 < voxel2D.size()) {
					// vertices3D��vertices���±�Ϊ��ԭʼ���ȣ��Ƴ��󣬲���ǰ�� + �ںϲ��е�λ��
					tmpIndices.push_back(oldVerticesSize + mesh.triangleVector[i].vertices[j] - 3);
				}
				// �����vertex
				else {
					// vertex���±�ΪedgeIndices
					tmpIndices.push_back(orderedIndices[mesh.triangleVector[i].vertices[j] - 3 - voxel2D.size()]);
				}
			}
		}

	
	}
	/// ����ģ��
	tumor.setVertices(tmpVertices);
	tumor.setIndices(tmpIndices);
	// test������
	
	// -------------����ά�ռ乹�������ʷ�-----------------
	lineVec.clear();
	std::vector<GLfloat> lineVertices;
	lineVertices.clear();
	// test������
	for (int i = 0; i < mesh.triangleVector.size(); i++) {
		lineVertices.clear();
		for (int j = 0; j < 3; j++) {
			lineVertices.push_back(mesh.pointVector[mesh.triangleVector[i].vertices[j]].x);
			lineVertices.push_back(mesh.pointVector[mesh.triangleVector[i].vertices[j]].y);
			lineVertices.push_back(200.0f);
		}
		//lineVertices.push_back(mesh.pointVector[mesh.triangleVector[i].vertices[0]].x);
		//lineVertices.push_back(mesh.pointVector[mesh.triangleVector[i].vertices[0]].y);
		//lineVertices.push_back(-10.0f);
		MyLine line(lineVertices);
		lineVec.push_back(line);
	}
}
void Area::updateCutVertices(BaseModel & tumor) {
	voxel2D.clear();
	voxel3D.clear();
	vertex2D.clear();
	vertex3D.clear();
	verticesToAppend2D.clear();
	verticesToAppend3D.clear();
	lineVec.clear();
	combinedPoints.clear();
	
	planA(tumor);
	//planB(tumor);
	
}

void Area::testCoorTrans(Shader & shader) {
	// todo Ѱ�ұ�Ե���ص�ӳ���ϵ���ǲ���
	shader.use();
	shader.setInt("withShadow", 0);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightPos", camera.Position);
	shader.setInt("cut", 0);
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	glm::mat4 model(1.0f);
	shader.setMat4("model", model);
	// draw edge vertices �����Ե - ��ɫ 
	// std::cout << "debugIndex " << debugIndex << " verticesToAppend2D - 4 " <<  - 4 - verticesToAppend2D.size() << std::endl;
	for (int i = 0; i < debugIndex - (int)verticesToAppend2D.size(); i++) {
		shader.setVec3("color", glm::vec3(1.0f,0.0f,0.0f));
		model = glm::translate(glm::mat4(1.0f), glm::vec3(vertex2D[i], 200.0f));
		//model = transformMat * glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", model);
		mySphere.draw();
	}
	// std::cout << "debugIndex " << debugIndex << " debugIndex - 4 " << debugIndex - 4 << std::endl;
	/*for (int i = 0; i < debugIndex - 4; i++) {
		std::cout << "for";
		// std::cout << "for i " << i << " " << debugIndex - 4 - voxel2D.size() << std::endl;
		shader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(glm::mat4(1.0f), glm::vec3(voxel2D[i], 200.0f));
		// model = transformMat * glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", model);
		mySphere.draw();
	}*/
	// draw edge voxel ���ر�Ե - ��ɫ
	for (int i = 0; i <= debugIndex; i++) {
		shader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(glm::mat4(1.0f), glm::vec3(verticesToAppend2D[i], 200.0f));
		//model = transformMat * glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", model);
	    mySphere.draw();
	}

	//3D draw edge voxel ���ر�Ե - ��ɫ
	/*std::cout << "voxel3D " << voxel3D.size() << std::endl;
	for (int i = 0; i < voxel3D.size(); i++) {
		shader.setVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(glm::mat4(1.0f), voxel3D[i]);
		model = transformMat * glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", model);
		mySphere.draw();
	}*/
	
	//3D draw edge voxel �����Ե - ��ɫ
	/*for (int i = 0; i < vertex3D.size(); i++) {
		shader.setVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::translate(glm::mat4(1.0f), vertex3D[i]);
		model = transformMat * glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", model);
		mySphere.draw();
	}*/
	//model = transformMat * glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", glm::mat4(1.0f));
	//lines.draw();
	/*for (int i = 0; i < mesh.edgeVector.size(); i++) {
		std::vector<GLfloat> tmp = {
			mesh.pointVector[mesh.edgeVector[i].point1ID].x, mesh.pointVector[mesh.edgeVector[i].point1ID].y, -10.0f,
			mesh.pointVector[mesh.edgeVector[i].point2ID].x, mesh.pointVector[mesh.edgeVector[i].point2ID].y, -10.0f
		};
		lines.displayLine(tmp);
	}*/
	if (DelaunayTri != NULL) {
		
		//DelaunayTri->draw();
	}
	//std::cout << "lineVec.size() " << lineVec.size() << std::endl;

	shader.setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));
	for (int i = 0; i < lineVec.size(); i++) {
		lineVec[i].draw();
	}
		
}
// ͨ�����̼���
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
		
		if (tmpVoxelPos->size() != 0) {// ����������
			for (int j = 0; j < tmpVoxelPos->size(); j++) {
				shader.setVec3("color", models[1].getColor());
				model = glm::scale(transformMat * glm::translate(glm::translate(glm::mat4(1.0f), (*tmpVoxelPos)[j]), glm::vec3(-4.38f, -201.899f, 148.987f)), glm::vec3(models[1].getStep() / 1.2, models[1].getStep() / 1.2, models[1].getStep() / 1.2));
				shader.setMat4("model", model);
				//mySphere.draw();
			}
		}
	}
	model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
	shader.setInt("withLight", 1);
	shader.setInt("isPlane", 0);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setVec3("color", models[modelsID[i]].getColor());
		//models[modelsID[i]].draw();
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

void Area::updateRuler(const glm::vec3& pos1, const glm::vec3& pos2) { // �������Ϊ��������
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
		if (removeMode == 1) { // ���ڰ뾶
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