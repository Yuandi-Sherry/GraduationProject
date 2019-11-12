#include "Area.h"
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
}

Area::~Area()
{
}

void Area::setModelsID(const std::vector<GLint>& models) {
	this->modelsID.assign(models.begin(), models.end());
}

void Area::initModels() {

}

void Area::setBound(GLfloat left, GLfloat bottom, GLfloat width, GLfloat height) {
	bound[0] = left;
	bound[1] = bottom;
	bound[2] = width;
	bound[3] = height;
	camera.setSize(width, height);
}

void Area::draw(Shader & shader, std::vector<BaseModel> & models) {
	shader.use();
	glViewport(bound[0], bound[1], bound[2], bound[3]);
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

void Area::setVertex(const glm::vec3 & vertexPosition) {
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

void Area::displayGUI() {
	ImGui::Text("Lines information for ruler");
	for (int i = 0; i < rulerLines.size(); i++) {
		std::string str = "line" + std::to_string(i+1) + " : " + std::to_string(rulerLines[i].getDistance());
		ImGui::TextDisabled(str.c_str());
	}
	
	
}