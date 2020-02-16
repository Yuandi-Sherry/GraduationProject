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
#include "Delaunay.h"
#include "BaseModel.h"
#include "MyTriangles.h"

#define SCALE_RULER 12.8

enum MODE
{
	GENERAL, RULER, CROSS_INTERSECTION, NEAREST_VESSEL, REMOVE_TUMOR
};
class Area
{
public:
	Area();
	~Area();
	/*---------------------- getters & setters ----------------------*/
	void setTransformMat(const glm::mat4x4& curMat) {	transformMat = curMat;	}
	int getMode() {		return modeSelection;	}
	Camera* getCamera() {	return &camera;		}
	glm::mat4x4 getTransformMat() {		return transformMat;	}
	std::string getDistance() {
		switch (modeSelection) {
		case RULER:
			return "Distance: " + std::to_string(ruler.distance / SCALE_RULER);
		case NEAREST_VESSEL:
			return "Distance: " + std::to_string(vesselDistance / SCALE_RULER);
		default:
			return "Distance: " + std::to_string(ruler.distance / SCALE_RULER);
		}
	}
	
	void setModelsID(const std::vector<GLint>&, std::vector<BaseModel> & models);
	void setViewport(GLfloat left, GLfloat button, GLfloat right, GLfloat top);
	glm::vec4 getViewport();
	
	
	void init();
	void renderDepthBuffer(Shader& shadowShaer, std::vector<BaseModel>& models);
	void displayGUI();

	/*----------------------GENERAL----------------------*/
	void drawModels(Shader& shader, Shader& shadowShader, std::vector<BaseModel>& models);
	/*----------------------RULER----------------------*/
	/**
	 * get ends of distance to measure by ruler
	 */
	void setRulerVertex(const glm::vec3 & vertexPosition);
	void tackleRuler(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models);
	void drawRuler(Shader& textureShader, Shader& pointShader);
	
	/*----------------------NEAREST_VESSEL----------------------*/
	void setLocalCoordinate(glm::vec3 worldCoor, BaseModel& vessel);

	void tackleNearestVessel(Shader& shader, Shader& shadowShader, Shader& textureShader, std::vector<BaseModel>& models);

	/*----------------------REMOVE_TUMOR----------------------*/
	void setRemovePos(glm::vec3 pos);
	void tackleRemoveTumor(Shader& shader, Shader& shadowShader, std::vector<BaseModel>& models);
	void updateCutVertices(BaseModel& tumor);
	void removeTumor(BaseModel& tumor);
	int getRemoveMode() {
		return removeMode;
	}

	void updateLightSpaceMatrix();
private:
	void initDepthBuffer();
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 400.0f);
	std::vector<GLint> modelsID;// the index of model in the model array in main function 
	Camera camera;
	
	glm::mat4x4 transformMat; // rotation mat of model
	/*----------------------parameters----------------------*/
	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	unsigned int depthMap;
	GLfloat viewportPara[4];// 4 parameters of viewport: x, y, w, h
	glm::vec3 modelCenter;
	/*----------------------REUSABLE ELEMENTS----------------------*/
	MySphere mySphere;
	MyCube myCube;
	MyLine lines;

	/*----------------------states----------------------*/
	int modeSelection;
	/*----------------------RULER----------------------*/
	Ruler ruler;
	// measure distance
	int currentRulerIndex = 0; // -1 -> has 2 vertices, 0 -> no vertex, 1 -> one vertex
	glm::mat4 lightSpaceMatrix;
	glm::mat4 lightProjection;
	glm::mat4 lightView;
	void updateRuler(const glm::vec3& pos1, const glm::vec3& pos2);

	/*----------------------NEAREST_VESSEL----------------------*/
	glm::vec3 NVLocalPos;
	glm::vec3 nearestPos = glm::vec3(-10000.0f, -10000.0f, -10000.0f);
	GLfloat vesselDistance = 0.0f;
	void findNearest(BaseModel& vessel);

	/*----------------------REMOVE_TUMOR----------------------*/
	int removeMode = 0;
	GLfloat cutRadius = 15.0f;
	glm::vec3 removePos = glm::vec3(0.0f);
	std::vector<glm::vec3> voxel3D;
	std::vector<glm::vec3> vertex3D;
	std::vector<glm::vec2> voxel2D;
	std::vector<glm::vec2> vertex2D;
	std::vector<glm::vec2> combinedPoints;
	Delaunay mesh;
};


#endif