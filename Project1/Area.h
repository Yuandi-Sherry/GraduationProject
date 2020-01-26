#ifndef _AREA_H_
#define _AREA_H_

// system headers
#include <vector>
#include <map>

// openGL headers
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// freetype
#include <ft2build.h>
#include FT_FREETYPE_H  

// user-defined headers
#include "Camera.h"
#include "Shader.h"
#include "Light.h"
#include "Plane.h"
#include "CrossSectionPlane.h"
#include "Ruler.h"
#include "MyCylinder.h"
#include "Light.h"
#include "MySphere.h"
#include "MyCube.h"
#include "MyLine.h"
#include "Delaunay.h"
#include "BaseModel.h"
#include "MyTriangles.h"
enum MODE
{
	GENERAL, RULER, CROSS_INTERSECTION, NEAREST_VESSEL, REMOVE_TUMOR
};
class Area
{
public:
	Area();
	~Area();
	void setModelsID(const std::vector<GLint>&, std::vector<BaseModel> & models);
	void setViewport(GLfloat left, GLfloat button, GLfloat right, GLfloat top);
	void drawModels(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models);
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
	//void drawLight(Shader & shader, Light& light);
	void setRulerVertex(const glm::vec3 & vertexPosition);
	
	void displayGUI();
	void setCutFaceVertex(const glm::vec3 & vertexPosition);
	void calculatePlane();
	glm::vec4 planeCoeff = glm::vec4(1, 1, 1, 0); // coefficient of plane equation: ax + by + cz + d = 0;
	glm::vec3 tmpCutFaceVertices[3]; // current operating line vertices
	glm::vec3 transCutFaceVertices[3]; // current operating line vertices
	
	

	// cut 
	// bool confirmedCut = false;//
	void tackleCrossIntersection(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models);
	void drawCutFace(Shader & shader, Shader & shadowShader, std::vector<BaseModel> & models);
	void drawSelectedFace(Shader & shader);
	glm::mat4x4 transformForCut[2];

	int getCutMode() {
		return cutMode;
	}
	int selectedCutAreaIndex;
	void setCutMode(const int mode) {
		cutMode = mode;
		if (mode == 3) {
			transformForCut[0] = transformMat;
			transformForCut[1] = transformMat;
		}
	}
	void renderDepthBuffer(Shader & shadowShaer, std::vector<BaseModel> & models);
	int getMode() {
		return modeSelection;
	}

	// ruler
	void tackleRuler(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models);
	void drawRuler(Shader& textureShader, Shader& pointShader);
	std::string getDistance() {
		return "Distance: " + std::to_string(ruler.distance / 15);
		switch (modeSelection) {
		case RULER:
			return "Distance: " + std::to_string(ruler.distance / 13);
		case NEAREST_VESSEL:
			return "Distance: " + std::to_string(vesselDistance / 13);
		}

	}

	// nearest vessl
	void setLocalCoordinate(glm::vec3 worldCoor, BaseModel& vessel);
	void findNearest(BaseModel& vessel);
	void tackleNearestVessel(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models);

	// remove tumor
	void tackleRemoveTumor(Shader& shader, Shader& shadowShader, std::vector<BaseModel>& models);
	void setRemovePos(glm::vec3 pos);
	void updateCutVertices(BaseModel& tumor);
	void removeTumor(BaseModel& tumor);
	int getRemoveMode() {
		return removeMode;
	}

	void updateLightSpaceMatrix();
	void testCoorTrans(Shader& shader);
private:
	void initDepthBuffer();
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 400.0f);
	std::vector<GLint> modelsID;// the index of model in the model array in main function 
	Camera camera;
	GLfloat viewportPara[4];// 4 parameters of viewport: x, y, w, h
	glm::mat4x4 transformMat; // rotation mat of model
	
	
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
	int modeSelection;

	GLfloat rulerScale = 4;
	// measure distance
	glm::vec3 selectedEnds[2]; // current operating line vertices
	int currentRulerIndex = 0; // -1 -> has 2 vertices, 0 -> no vertex, 1 -> one vertex
	glm::mat4 lightSpaceMatrix;
	glm::mat4 lightProjection;
	glm::mat4 lightView;
	void updateRuler(const glm::vec3& pos1, const glm::vec3& pos2);

	// nearest vessel
	glm::vec3 NVLocalPos;
	glm::vec3 nearestPos = glm::vec3(-10000.0f, -10000.0f, -10000.0f);
	GLfloat vesselDistance = 0.0f;
	// remove Tumor
	int removeMode = 0;
	GLfloat cutRadius = 15.0f;
	glm::vec3 removePos = glm::vec3(0.0f);

	// tool element
	MySphere mySphere;
	MyCube myCube;

	// test remove
	std::vector<glm::vec3> voxel3D;
	std::vector<glm::vec3> vertex3D;

	std::vector<glm::vec2> voxel2D;
	std::vector<glm::vec2> vertex2D;
	// test
	MyLine lines;

	Delaunay mesh;

	MyTriangles * DelaunayTri = NULL;
	std::vector<MyLine> lineVec;

	//std::vector<GLfloat> 
};


#endif