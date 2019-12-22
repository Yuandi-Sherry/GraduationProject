#include "Area.h"
#include "Plane.h"
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
Area::Area()
{
	modelsID.clear();
	transformMat = glm::mat4(1.0f);
	testPlane = NULL;
}

Area::~Area()
{
	if (testPlane != NULL) {
		delete testPlane;
	}
}

void Area::init() {
	// init 
	GLfloat Near = 0.1f;
	camera.setPosition(cameraPos);
	cutMode = 1;
	csPlane.initVertexObject();
	// ruler=
	ruler.initVertexObject();

	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture

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

void Area::initModels() {

}
void Area::tackleCrossIntersection(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models) {

	if (cutMode == 1) { // selecting
		camera.setSize(viewportPara[2], viewportPara[3]);
		draw(shader, shadowShader, models);
	}
	else if (cutMode == 2) { // confirming
		draw(shader, shadowShader, models);
		drawSelectedFace(shader);
	}
	else {// confirmed {
		camera.setSize(viewportPara[2] / 2, viewportPara[3]);
		drawCutFace(shader, shadowShader, models);
	}

		
}
void Area::setViewport(GLfloat left, GLfloat bottom, GLfloat width, GLfloat height) {
	viewportPara[0] = left;
	viewportPara[1] = bottom;
	viewportPara[2] = width;
	viewportPara[3] = height;
	camera.setSize(width, height);
}

void Area::renderDepthBuffer(Shader & shadowShader, std::vector<BaseModel> & models) {
	shadowShader.use();
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
void Area::drawShadow(Shader & shadowShader, std::vector<BaseModel> & models) {
	shadowShader.use();
	shadowShader.setInt("cut", 1);
	shadowShader.setVec4("plane", planeCoeff);
	renderDepthBuffer(shadowShader, models);
}

void Area::draw(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models) {
	shadowShader.use();
	shadowShader.setInt("cut", 0);
	renderDepthBuffer(shadowShader, models);

	glViewport(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	
	shader.use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("lightPos", camera.Position);
	shader.setInt("cut", 0);
	shader.setVec4("plane", planeCoeff);
	shader.setMat4("projection", camera.getProjection());
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
	shader.setInt("withLight", 1);
	shader.setInt("isPlane", 0);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setInt("type", models[modelsID[i]].getcolorID());
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
	shader.setMat4("projection", camera.getProjection());
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	glViewport(viewportPara[0], viewportPara[1], viewportPara[2] / 2, viewportPara[3]);
	shader.setInt("cut", 1);
	shader.setMat4("model", transformForCut[0] * model);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setInt("type", models[modelsID[i]].getcolorID());
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
		shader.setInt("type", models[modelsID[i]].getcolorID());
		models[modelsID[i]].draw();
	}
}

void Area::drawSelectedFace(Shader & shader) {
	shader.use();
	// upper part 
	shader.setInt("cut", 2);
	glViewport(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
	shader.setVec4("plane", planeCoeff);
	shader.setMat4("projection", camera.getProjection());
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	// models
	// plane bug: plane is not transparent
	shader.setInt("withLight", 0);
	shader.setInt("isPlane", 1);
	shader.setMat4("model", transformMat * model);
	csPlane.draw();
}

void Area::drawLight(Shader & shader, Light& light) {
	shader.use();
	GLint viewPosLoc = glGetUniformLocation(shader.ID, "viewPos");
	glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);
	shader.setMat4("projection", camera.getProjection());
	// camera/view transformation
	shader.setMat4("view", camera.GetViewMatrix());
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	model = glm::translate(model, light.Position);
	//model = glm::translate(model, glm::vec3(4.38f, 201.899f, -148.987f));
	shader.setMat4("model", model);
	//shader.setMat4("model", model);
	if (cutMode == 3){
		glViewport(viewportPara[0], viewportPara[1], viewportPara[2] / 2, viewportPara[3]);
		light.draw();
		glViewport(viewportPara[0] + viewportPara[2] / 2, viewportPara[1], viewportPara[2] / 2, viewportPara[3]);
	}
	else {
		glViewport(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
	}
	light.draw();
	
	
}

void Area::drawLine(Shader & shader) {
	shader.use();
	// in phongShader do not use lighting
	glUniform1i(glGetUniformLocation(shader.ID, "withLight"), 0);
	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
	shader.setInt("type", 4);
	for (int i = 0; i < rulerLines.size(); i++) {

		rulerLines[i].draw();
	}
}

void Area::drawRuler(Shader & shader) {
	shader.use();
	ruler.generateTexture();
	ruler.draw(shader);
}

void Area::setRulerVertex(const glm::vec3 & vertexPosition) {
	glm::vec3 localPos = glm::inverse(transformMat) * glm::vec4(vertexPosition, 1.0f);
	tmpVertices[currentRulerIndex] = localPos;
	if (currentRulerIndex == 0) {
		currentRulerIndex = 1;
	}
	else if (currentRulerIndex == 1) {
		// add to rulerLines vector
		std::vector<GLfloat> tmp = {
			tmpVertices[0].x,
			tmpVertices[0].y,
			tmpVertices[0].z,
			tmpVertices[1].x,
			tmpVertices[1].y,
			tmpVertices[1].z
		};
		Line newLine(tmp, 4, LINE);
		newLine.initVertexObject();
		rulerLines.push_back(newLine);
		currentRulerIndex = 0;
	}

	// std::cout << "current size of lines"
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

	// editModel
	/*editedModel.clear();
	std::vector<GLfloat> upperPart;
	std::vector<GLfloat> lowerPart;
	// for each model set initially
	for (int i = 0; i < modelsID.size(); i++) {
		std::vector<GLfloat> * tmp = (*models)[i].getVertices();
		std::cout << "tmp->size()" << tmp->size() << std::endl;
		for (int j = 0; j < tmp->size(); j += 18) {
			if ((*tmp)[j] * planeCoeff.x + (*tmp)[j + 1] * planeCoeff.y + (*tmp)[j + 2] * planeCoeff.z + planeCoeff.w > 0) {
				for (int k = 0; k < 18; k++) {
					upperPart.push_back((*tmp)[j+k]);
				}
			}
			else {
				for (int k = 0; k < 18; k++) {
					lowerPart.push_back((*tmp)[j + k]);
				}
			}
		}
		BaseModel upperModel(upperPart, (*models)[i].getcolorID(), TRIANGLE);
		BaseModel lowerModel(lowerPart, (*models)[i].getcolorID(), TRIANGLE);
		editedModel.push_back(upperModel);
		editedModel.push_back(lowerModel);
		upperPart.clear();
		lowerPart.clear();
	}
	for (int i = 0; i < editedModel.size(); i++) {
		editedModel[i].initVertexObject();
	}*/
	
}

void Area::displayGUI() {
	ImGui::Text("Lines information for ruler");
	for (int i = 0; i < rulerLines.size(); i++) {
		std::string str = "line" + std::to_string(i+1) + " : " + std::to_string(rulerLines[i].getDistance());
		ImGui::TextDisabled(str.c_str());
	}
	if (cutMode == 1) {
		ImGui::Text("selecting");
	}
	else if(cutMode == 2) {
		ImGui::Text("press C to confirm");
		/*bool tmp2 = !ImGui::Button("Cancel");
		if (tmp2) {
			cutMode = 1;
		}*/
		//confirmedCut = !ImGui::Button("Cancel");
	}
	else {
		ImGui::Text("confirmed");
		/*}*/
	}
}