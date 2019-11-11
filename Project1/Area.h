#ifndef _AREA_H_
#define _AREA_H_

// system headers
#include <vector>

// openGL headers
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// user-defined headers
#include "BaseModel.h"
#include "Camera.h"
#include "Shader.h"
#include "Light.h"

class Area
{
public:
	Area();
	~Area();
	void setModelsID(const std::vector<GLint>&);
	void initModels();
	void setBound(GLfloat left, GLfloat button, GLfloat right, GLfloat top);
	void draw(Shader & shader, std::vector<BaseModel> & models);
	Camera* getCamera() {
		return &camera;
	}
	glm::mat4x4 getTransformMat() {
		return transformMat;
	}
	void setTransformMat(const glm::mat4x4& curMat) {
		transformMat = curMat;
	}
	glm::vec4 getBound() {
		return glm::vec4(bound[0], bound[1], bound[2], bound[3]);
	}
	void drawLight(Shader & shader, Light& light);
private:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 200.0f);
	std::vector<GLint> modelsID;
	Camera camera;
	GLfloat bound[4];
	glm::mat4x4 transformMat;
	glm::vec3 ruler[2];//start coordinate & end coordinate
};

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
		shader.setInt("type", models[modelsID[i]].getColor());
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
#endif