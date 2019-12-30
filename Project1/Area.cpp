#include "Area.h"
#include "Plane.h"
// imgui
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
#include "MyCylinder.h"

GLfloat zR = 1, zG = 1, zB = 1;
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
	// mode
	modeSelection = 1;

	// ruler
	ruler.initVertexObject();
	rulerEnd.initVertexObject();
	// cut
	csPlane.initVertexObject();
	cutMode = 1; // general

	// nearest vessel
	nearestPoint.initVertexObject();

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

void Area::tackleRuler(Shader& shader, Shader& shadowShader, Shader& textureShader, Shader& pointShader, std::vector<BaseModel>& models) {
	drawModels(shader, shadowShader, models);
	drawRuler(textureShader, shader);
}


void Area::tackleNearestVessel(Shader& shader, Shader& shadowShader, std::vector<BaseModel>& models, BaseModel & vessel) {
	drawModels(shader, shadowShader, models);
	if (glm::vec3(-10000.0f, -10000.0f, -10000.0f) != nearestPos) {
		// draw point
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
		glm::mat4 model = glm::translate(glm::mat4(1.0f), nearestPos);
		model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		model = glm::scale(model, glm::vec3(3, 3, 3));
		shader.setMat4("model", transformMat * model);
		shader.setInt("withLight", 1);
		shader.setInt("isPlane", 0);
		shader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
		nearestPoint.draw();
	}
	

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
	for (int i = 0; i < modelsID.size(); i++) {
		models[modelsID[i]].draw();
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
	shader.setVec4("plane", planeCoeff);
	shader.setMat4("projection", camera.getOrthology());
	shader.setMat4("view", camera.GetViewMatrix());
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
	shader.setInt("withLight", 1);
	shader.setInt("isPlane", 0);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setVec3("color", models[modelsID[i]].getColor());
		models[modelsID[i]].draw();
	}
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

void Area::drawRuler(Shader & textureShader, Shader& pointShader) {
	// ruler
	textureShader.use();
	textureShader.setVec3("viewPos", camera.Position);
	textureShader.setVec3("lightPos", camera.Position);
	textureShader.setMat4("projection", camera.getOrthology());
	textureShader.setMat4("view", camera.GetViewMatrix());
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, ruler.position);
	model = glm::rotate(model, ruler.rotateAngle, glm::vec3(0,0, 1));
	model = glm::scale(model, glm::vec3(ruler.scaleSize * 2, 3, 1));
	textureShader.setMat4("model", model);
	ruler.generateTexture();
	ruler.draw(textureShader);

	// ends
	pointShader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	pointShader.setVec3("color", glm::vec3(0.5f, 0.8f, 0.3f));
	pointShader.setMat4("projection", camera.getOrthology());
	pointShader.setMat4("view", camera.GetViewMatrix());
	pointShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	pointShader.setVec3("viewPos", camera.Position);
	pointShader.setVec3("lightPos", camera.Position);
	pointShader.setInt("cut", 0);
	pointShader.setInt("withLight", 1);
	pointShader.setInt("isPlane", 0);
	model = glm::scale( glm::translate(glm::mat4(1.0f), ruler.ends[0]), glm::vec3(3,3,3));
	pointShader.setMat4("model", model);
	rulerEnd.draw();
	model = glm::scale(glm::translate(glm::mat4(1.0f), ruler.ends[1]), glm::vec3(3, 3, 3));
	pointShader.setMat4("model", model);
	rulerEnd.draw();
}

void Area::setRulerVertex(const glm::vec3 & vertexPosition) {
	glm::vec3 localPos = glm::vec4(vertexPosition, 1.0f);
	tmpVertices[currentRulerIndex] = localPos;
	if (currentRulerIndex == 0) { // first end
		ruler.ends[currentRulerIndex] = localPos;
		currentRulerIndex = 1;
	}
	else { // second end
		ruler.ends[currentRulerIndex] = localPos;
		currentRulerIndex = 0;

		// calculate ruler attributes
		ruler.distance = glm::distance(ruler.ends[0], ruler.ends[1]);
		ruler.position = (ruler.ends[0] + ruler.ends[1]) / 2.0f;
		ruler.ends[0].z = ruler.CUTFACE;
		ruler.ends[1].z = ruler.CUTFACE;
		GLfloat projLen = glm::distance(ruler.ends[0], ruler.ends[1]);
		ruler.scaleSize = projLen / ruler.distance;
		ruler.position.z = ruler.CUTFACE;
		ruler.rotateAngle = atan((ruler.ends[0].y - ruler.ends[1].y)/ (ruler.ends[0].x - ruler.ends[1].x));		
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
	ImGui::RadioButton("General", &modeSelection, 1);
	ImGui::RadioButton("Ruler", &modeSelection, 2);
	ImGui::RadioButton("Cut", &modeSelection, 3);
	ImGui::RadioButton("Nearest Vessel", &modeSelection, 4);
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

}

void Area::setLocalCoordinate(glm::vec3 worldCoor, BaseModel & vessel) {
	NVLocalPos = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-4.38f, -201.899f, 148.987f))) * glm::inverse(transformMat) * glm::vec4(worldCoor, 1.0f);
	findNearest(vessel);
}

void Area::findNearest(BaseModel& vessel) {
	GLfloat min = 10000.0f;
	int minIndex;
	for (int i = 0; i < vessel.getVertices()->size() / 6; i++) {
		glm::vec3 pos = glm::vec3((*vessel.getVertices())[i*6], (*vessel.getVertices())[i*6+1], (*vessel.getVertices())[i*6+2]);
		GLfloat dis = glm::distance(NVLocalPos, pos);
		if (dis < min) {
			min = dis;
			minIndex = i;
		}
	}
	nearestPos = glm::vec3((*vessel.getVertices())[minIndex * 6], (*vessel.getVertices())[minIndex * 6 + 1], (*vessel.getVertices())[minIndex * 6 + 2]);
}