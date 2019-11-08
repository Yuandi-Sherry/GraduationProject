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

class Area
{
public:
	Area();
	~Area();
	void setModels(const std::vector<BaseModel>&);
	void initModels();
	void setShader(Shader & shader);
	void setBound(GLfloat left, GLfloat button, GLfloat right, GLfloat top);
	void editShader();
	void draw(Shader & shader);
	void draw();
private:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 50.0f);
	std::vector<BaseModel> models;
	Camera camera;
	glm::mat4x4 transformMat;
	Shader * myShader;
	GLfloat bound[4];
	// Shader myShader("phongShader.vs", "phongShader.frag");
};

Area::Area()
{
	// init 
	models.clear();	
	GLfloat Near = 0.1f;
	camera.setPosition(cameraPos);
	glm::mat4x4 transformMat = glm::mat4x4(1.0f);
}

Area::~Area()
{
}

void Area::setModels(const std::vector<BaseModel>& models) {
	this->models.assign(models.begin(), models.end());
}

void Area::initModels() {
	for (int i = 0; i < models.size(); i++) {
		models[i].initVertexObject();
	}
}

void Area::setBound(GLfloat left, GLfloat button, GLfloat right, GLfloat top) {
	bound[0] = left;
	bound[1] = button;
	bound[2] = right;
	bound[3] = top;
}
void Area::setShader(Shader & shader) {
	shader.setMat4("projection", camera.projection);
	// camera/view transformation
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	shader.setMat4("model", transformMat * model);
}
void Area::editShader() {
	(*myShader).setMat4("projection", camera.projection);
	// camera/view transformation
	glm::mat4 view = camera.GetViewMatrix();
	(*myShader).setMat4("view", view);
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
	(*myShader).setMat4("model", transformMat * model);
}

void Area::draw(Shader & shader) {
	glViewport(bound[0], bound[1], bound[2], bound[3]); 
	// std::cout << "shader ID in function" <<  shader.ID << std::endl;
	for (int i = 1; i < 4; i++) {
		shader.setInt("type", models[i - 1].getColor());
		models[i - 1].draw();
	}
}

void Area::draw() {
	glViewport(bound[0], bound[1], bound[2], bound[3]); 
		// std::cout << "shader ID in function" <<  shader.ID << std::endl;
		for (int i = 1; i < 4; i++) {
			models[i - 1].draw();
		}
}
#endif