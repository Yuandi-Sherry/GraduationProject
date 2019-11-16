#include "Area.h"
#include "Plane.h"
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <string>
Area::Area()
{
	// init 
	modelsID.clear();
	GLfloat Near = 0.1f;
	camera.setPosition(cameraPos);
	transformMat = glm::mat4(1.0f);
	testPlane = NULL;
}

Area::~Area()
{
	if (testPlane != NULL) {
		delete testPlane;
	}
}

void Area::setModelsID(const std::vector<GLint>& models) {
	this->modelsID.assign(models.begin(), models.end());
}

void Area::initModels() {

}

void Area::setViewport(GLfloat left, GLfloat bottom, GLfloat width, GLfloat height) {
	viewportPara[0] = left;
	viewportPara[1] = bottom;
	viewportPara[2] = width;
	viewportPara[3] = height;
	camera.setSize(width, height);
}

glm::vec4 Area::getViewport() {
	return glm::vec4(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
}

void Area::draw(Shader & shader, std::vector<BaseModel> & models) {
	shader.use();
	glViewport(viewportPara[0], viewportPara[1], viewportPara[2], viewportPara[3]);
	shader.setVec4("plane", planeCoeff);
	shader.setMat4("projection", camera.getProjection());
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
	for (int i = 0; i < modelsID.size(); i++) {
		shader.setInt("type", models[modelsID[i]].getcolorID());
		models[modelsID[i]].draw();
	}
	if (testPlane != NULL) {
		glUniform1i(glGetUniformLocation(shader.ID, "withLight"), 0);
		glUniform1i(glGetUniformLocation(shader.ID, "isPlane"), 1);
		glm::mat4 model = glm::mat4(1.0f);
		//model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		shader.setMat4("model", /*transformMat */ model);
		shader.setInt("type", 4);
		testPlane->draw();
		glUniform1i(glGetUniformLocation(shader.ID, "isPlane"), 0);
	}
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
	shader.setMat4("model", transformMat * glm::translate(model, light.Position));
	//shader.setMat4("model", glm::mat4(1.0f));
	light.draw();
}

void Area::drawLine(Shader & shader) {
	shader.use();
	// in phongShader do not use lighting
	glUniform1i(glGetUniformLocation(shader.ID, "withLight"), 0);
	glm::mat4 model = glm::mat4(1.0f);
	// model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
	shader.setInt("type", 4);
	for (int i = 0; i < rulerLines.size(); i++) {

		rulerLines[i].draw();
	}
}

void Area::setRulerVertex(const glm::vec3 & vertexPosition) {
	tmpVertices[currentRulerIndex] = vertexPosition;
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
	tmpCutFaceVertices[currentCutFaceIndex] = vertexPosition;
	if (currentCutFaceIndex == 0) {
		currentCutFaceIndex = 1;
	}
	else if (currentCutFaceIndex == 1) {
		currentCutFaceIndex = 2;
	}
	else if (currentCutFaceIndex == 2) {
		currentCutFaceIndex = 0;
		calculatePlane();
	}
}

void Area::calculatePlane() {
	if (testPlane != NULL) {
		delete testPlane;
	}
	glm::vec3 vector1 = glm::vec3(transCutFaceVertices[0].x - transCutFaceVertices[1].x, transCutFaceVertices[0].y - transCutFaceVertices[1].y, transCutFaceVertices[0].z - transCutFaceVertices[1].z);
	glm::vec3 vector2 = glm::vec3(transCutFaceVertices[0].x - transCutFaceVertices[2].x, transCutFaceVertices[0].y - transCutFaceVertices[2].y, transCutFaceVertices[0].z - transCutFaceVertices[2].z);
	GLfloat a = (transCutFaceVertices[1].y - transCutFaceVertices[0].y) * (transCutFaceVertices[2].z - transCutFaceVertices[0].z) - (transCutFaceVertices[2].y - transCutFaceVertices[0].y) * (transCutFaceVertices[1].z - transCutFaceVertices[0].z);
	GLfloat b = (transCutFaceVertices[1].z - transCutFaceVertices[0].z) * (transCutFaceVertices[2].x - transCutFaceVertices[0].x) - (transCutFaceVertices[2].z - transCutFaceVertices[0].z) * (transCutFaceVertices[1].x - transCutFaceVertices[0].x);
	GLfloat c = (transCutFaceVertices[1].x - transCutFaceVertices[0].x) * (transCutFaceVertices[2].x - transCutFaceVertices[0].y) - (transCutFaceVertices[2].x - transCutFaceVertices[0].x) * (transCutFaceVertices[1].y - transCutFaceVertices[0].y);
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
	
	testPlane = new Plane(tmpVec, 4, TRIANGLE);
	testPlane->initVertexObject();
}

void Area::displayGUI() {
	ImGui::Text("Lines information for ruler");
	for (int i = 0; i < rulerLines.size(); i++) {
		std::string str = "line" + std::to_string(i+1) + " : " + std::to_string(rulerLines[i].getDistance());
		ImGui::TextDisabled(str.c_str());
	}
	
	
}