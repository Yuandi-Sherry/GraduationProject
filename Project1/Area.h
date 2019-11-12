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
#include "Line.h"
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
	void setVertex(const glm::vec3 & vertexPosition);
	void drawLine(Shader & shader);
	void displayGUI();
private:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 200.0f);
	std::vector<GLint> modelsID;
	Camera camera;
	GLfloat bound[4];
	glm::mat4x4 transformMat;
	glm::vec3 tmpVertices[2];
	int currentRulerIndex = 0; // -1 -> has 2 vertices, 0 -> no vertex, 1 -> one vertex
	std::vector<Line> rulerLines;
};


#endif