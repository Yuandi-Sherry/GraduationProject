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
#include "CrossSectionPlane.h"
#include "Ruler.h"
#include "MyCylinder.h"
class Area
{
public:
	Area();
	~Area();
	void setModelsID(const std::vector<GLint>&, std::vector<BaseModel> & models);
	void initModels();
	void setViewport(GLfloat left, GLfloat button, GLfloat right, GLfloat top);
	void draw(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models);
	Camera* getCamera() {
		return &camera;
	}
	glm::mat4x4 getTransformMat() {
		return transformMat;
	}
	void setTransformMat(const glm::mat4x4& curMat) {
		transformMat = curMat;
	}
	void setCutTransformMat(const glm::mat4x4& curMat) {
		transformForCut[selectedCutAreaIndex] = curMat * transformForCut[selectedCutAreaIndex];
	}
	glm::vec4 getViewport();
	void init();
	void drawLight(Shader & shader, Light& light);
	void setRulerVertex(const glm::vec3 & vertexPosition);
	void drawLine(Shader & shader);
	void displayGUI();
	void setCutFaceVertex(const glm::vec3 & vertexPosition);
	void calculatePlane();
	glm::vec4 planeCoeff = glm::vec4(1, 1, 1, 0); // coefficient of plane equation: ax + by + cz + d = 0;
	glm::vec3 tmpCutFaceVertices[3]; // current operating line vertices
	glm::vec3 transCutFaceVertices[3]; // current operating line vertices
	
	// ruler
	void drawRuler(Shader & shader);
	void tackleRuler(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models);

	// cut 
	// bool confirmedCut = false;//
	void tackleCrossIntersection(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models);
	void drawCutFace(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models);
	void drawSelectedFace(Shader & shader);
	glm::mat4x4 transformForCut[2];
	void calcalateTransMatForCut();

	int getCutMode() {
		return cutMode;
	}
	int selectedCutAreaIndex;
	void setCutMode(const int mode) {
		std::cout << " setCutMode" << mode << std::endl;
		cutMode = mode;
		if (mode == 3) {
			transformForCut[0] = transformMat;
			transformForCut[1] = transformMat;
		}
	}
	void drawShadow(Shader & shader, std::vector<BaseModel> & models);
	void renderDepthBuffer(Shader & shadowShaer, std::vector<BaseModel> & models);
	int getRulerMode() {
		return rulerMode;
	}

	void setRulerMovement(Camera_Movement direction, float deltaTime);
	void drawZAxis(Shader& cylinderShader, Shader& shadowShader, std::vector<BaseModel>& models);
private:

	glm::vec3 cameraPos = glm::vec3(0.0f, 20.0f, 200.0f);
	std::vector<GLint> modelsID;// the index of model in the model array in main function 
	Camera camera;
	GLfloat viewportPara[4];// 4 parameters of viewport: x, y, w, h
	glm::mat4x4 transformMat; // rotation mat of model
	glm::vec3 tmpVertices[2]; // current operating line vertices
	int currentRulerIndex = 0; // -1 -> has 2 vertices, 0 -> no vertex, 1 -> one vertex
	std::vector<Line> rulerLines;
	
	
	int currentCutFaceIndex = 0; // -1 -> has 2 vertices, 0 -> no vertex, 1 -> one vertex
	Plane * testPlane;
	CrossSectionPlane csPlane;

	int cutMode; // 1->selecting, 2->confirming, 3-> confirmed;
	std::vector<BaseModel> editedModel;
	std::vector<BaseModel> * models;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	unsigned int depthMap;

	Ruler ruler;
	int rulerMode = 1;

	GLfloat rulerScale = 4;
	MyCylinder zAxis;
};


#endif