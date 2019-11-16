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
#include "Plane.h"

class Area
{
public:
	Area();
	~Area();
	void setModelsID(const std::vector<GLint>&);
	void initModels();
	void setViewport(GLfloat left, GLfloat button, GLfloat right, GLfloat top);
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
	glm::vec4 getViewport();
	void drawLight(Shader & shader, Light& light);
	void setRulerVertex(const glm::vec3 & vertexPosition);
	void drawLine(Shader & shader);
	void displayGUI();
	void setCutFaceVertex(const glm::vec3 & vertexPosition);
	void calculatePlane();
	glm::vec4 planeCoeff = glm::vec4(1, 1, 1, 0); // coefficient of plane equation: ax + by + cz + d = 0;
	glm::vec3 tmpCutFaceVertices[3]; // current operating line vertices
	glm::vec3 transCutFaceVertices[3]; // current operating line vertices
private:
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 200.0f);
	std::vector<GLint> modelsID;// the index of model in the model array in main function 
	Camera camera;
	GLfloat viewportPara[4];// 4 parameters of viewport: x, y, w, h
	glm::mat4x4 transformMat; // rotation mat of model
	glm::vec3 tmpVertices[2]; // current operating line vertices
	int currentRulerIndex = 0; // -1 -> has 2 vertices, 0 -> no vertex, 1 -> one vertex
	std::vector<Line> rulerLines;
	
	
	int currentCutFaceIndex = 0; // -1 -> has 2 vertices, 0 -> no vertex, 1 -> one vertex
	Plane * testPlane;
};


#endif