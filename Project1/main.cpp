#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// imgui
#include "imgui/imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// others
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "BaseModel.h"
#include "Area.h"
#include "Ruler.h"
#include "MyCylinder.h"
#include "Character.h"
// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
// gui
void initGUI(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
// camera
GLfloat Near = 0.1f;
// mouse parameters
	// current mouse position
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
glm::vec3 getObjCoor(GLfloat x, GLfloat y);
	// dragging mode detection
bool rbutton_down = false;
bool lbutton_down = false;
float xoffset = 0, yoffset = 0;
// light
// Light light;
GLfloat ambientPara = 0.4f , diffusePara = 1.0f, specularPara = 0.5f;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

GLfloat color1[3] = { 0.6f, 0.0f, 0.0f }, color2[3] = {0.5f, 0.5f, 0.6f}, color3[3] = {0.7f, 0.7f, 0.5f};
// select area
Area mainArea, vesselArea, tumorArea, bonesArea;
Area* currentArea = &mainArea;
Character characterController;
std::vector<BaseModel> models;
int main()
{
	// initialize GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	// create window
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
	initGUI(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	
	// set viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	
	// build and compile the shader
	Shader ourShader("phongShader.vs", "phongShader.frag");
	Shader pointSader("camera.vs", "camera.frag");
	Shader shadowShader("shadowMappingDepth.vs", "shadowMappingDepth.frag");
	Shader textureShader("textureShader.vs", "textureShader.frag");
	Shader cylinderShader("positionShader.vs", "positionShader.frag");
	Shader characterShader("characterShader.vs", "characterShader.frag");

	characterController.init();
	characterShader.setMat4("projection", characterController.getProjection());
	// load model
	BaseModel vessel("vessel_normal.stl", glm::vec3(0.6f, 0.0f, 0.0f));
	BaseModel tumor("tumor_normal.stl", glm::vec3(0.5f, 0.5f, 0.6f));
	BaseModel bones("bones_normal.stl", glm::vec3(0.7f, 0.7f, 0.5f));
	vessel.initVertexObject();
	tumor.initVertexObject();
	bones.initVertexObject();
	tumor.voxelization();
	
	// configure global opengl state
	// -----------------------------
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	models.push_back(vessel);
	models.push_back(tumor);
	models.push_back(bones);
	
	std::vector<GLint> tmp = {0, 1, 2};
	mainArea.setModelsID(tmp, models);

	tmp.clear();
	tmp.push_back(0);
	vesselArea.setModelsID(tmp, models);
	
	tmp.clear();
	tmp.push_back(1);
	tumorArea.setModelsID(tmp, models);

	tmp.clear();
	tmp.push_back(2);
	bonesArea.setModelsID(tmp, models);

	mainArea.init();
	vesselArea.init();
	tumorArea.init();
	bonesArea.init();

	mainArea.setViewport(0, 0, SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT);
	vesselArea.setViewport(SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT * 2 / 3.0f, SCR_WIDTH / 4.0f, SCR_HEIGHT / 3.0f);
	tumorArea.setViewport(SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT / 3.0f, SCR_WIDTH / 4.0f, SCR_HEIGHT / 3.0f);
	bonesArea.setViewport(SCR_WIDTH * 3 / 4.0f, 0, SCR_WIDTH / 4.0f, SCR_HEIGHT / 3.0f);

	// shader configuration
	// --------------------
	shadowShader.use();
	shadowShader.setInt("shadowMap", 0);

	ourShader.use();
	ourShader.setInt("depthMap", 0);
	ourShader.setFloat("ambientStrength", ambientPara);
	ourShader.setFloat("diffuseStrength", diffusePara);
	ourShader.setFloat("specularStrength", specularPara);
	textureShader.use();
	textureShader.setFloat("ambientStrength", ambientPara);
	textureShader.setFloat("diffuseStrength", diffusePara);
	textureShader.setFloat("specularStrength", specularPara);
	// game loop
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// input
		// -----
		processInput(window);


		// Render
		// Clear the colorbuffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		ourShader.use();
		ourShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
		ourShader.setInt("withLight", 1);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		/*if (currentArea->getMode() == RULER) {
			mainArea.tackleRuler(ourShader, shadowShader, textureShader, models);
			//characterController.RenderText(characterShader, currentArea->getDistance(), 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
			//vesselArea.tackleRuler(ourShader, shadowShader, textureShader, models);
			//tumorArea.tackleRuler(ourShader, shadowShader, textureShader,  models);
			//bonesArea.tackleRuler(ourShader, shadowShader, textureShader, models);
		}
		else if (currentArea->getMode() == CROSS_INTERSECTION) {
			mainArea.tackleCrossIntersection(ourShader, shadowShader,  models);
			//vesselArea.tackleCrossIntersection(ourShader, shadowShader, models);
			//tumorArea.tackleCrossIntersection(ourShader, shadowShader,models );
			//bonesArea.tackleCrossIntersection(ourShader, shadowShader, models);
		}
		else if (currentArea->getMode() == NEAREST_VESSEL) {
			mainArea.tackleNearestVessel(ourShader, shadowShader, textureShader, models);
		}
		else if (currentArea->getMode() == REMOVE_TUMOR) {
			mainArea.tackleRemoveTumor(ourShader, shadowShader, models);
		}
		else {
			//
			mainArea.drawModels(ourShader, shadowShader, models);
			//vesselArea.drawModels(ourShader, shadowShader, models);
			//tumorArea.drawModels(ourShader, shadowShader, models);
			//bonesArea.drawModels(ourShader, shadowShader, models);

		}*/
		//if (currentArea->getRemoveMode() == 1) {
			mainArea.tackleRemoveTumor(ourShader, shadowShader, models);
		//}
		
		mainArea.testCoorTrans(ourShader);
		//tumor.voxelization();
		// display GUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Options", NULL, ImGuiWindowFlags_MenuBar);

		currentArea->displayGUI();
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(window);
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(-1, 1, display_w, display_h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// swap buffer
		glfwSwapBuffers(window);
		// check events
		glfwPollEvents();
	}

	// release resources
	delete[] tumor.markVoxel;
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	// move camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(RIGHT, deltaTime);
	
	currentArea->updateLightSpaceMatrix();
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		(*currentArea->getCamera()).alterMouse();
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && currentArea->getCutMode() == 2) {
		currentArea->setCutMode(3);
	}
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && currentArea->getCutMode() == 3) {
		currentArea->setCutMode(1);
	}

	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && currentArea->getMode() == REMOVE_TUMOR && currentArea -> getRemoveMode() == 1) {
		currentArea->removeTumor(models[1]);
		std::cout << "currentArea->getRemoveMode() " << currentArea->getRemoveMode() << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && currentArea->getMode() == REMOVE_TUMOR && currentArea->getRemoveMode() == 0) {
		currentArea->addPoint();
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

#define ROT_COEF 100
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	xoffset = xpos - lastX;
	yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (rbutton_down) {
		glm::mat4x4 rotationMatY = glm::mat4x4(1.0f);
		glm::mat4x4 rotationMatX = glm::mat4x4(1.0f);
		const float R_SPEED = 0.1f;
		// Rotate the camera around with the mouse according to how much it's moved 
		// since the last frame 
		rotationMatY = glm::rotate(rotationMatY, xoffset * deltaTime * R_SPEED, glm::vec3(0, 1, 0));
		rotationMatX = glm::rotate(rotationMatX, -yoffset * deltaTime * R_SPEED, glm::vec3(1, 0, 0));
		if (currentArea->getMode() == CROSS_INTERSECTION && currentArea->getCutMode() == 3) {
			//currentArea->updateTransformForCut();
			currentArea->setCutTransformMat(rotationMatY * rotationMatX);
		}
		else {
			currentArea->setTransformMat(rotationMatY * rotationMatX *(currentArea->getTransformMat()));
		}
		
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// 鼠标左键
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action) {
			lbutton_down = true;
			// select area
			if (lastX < SCR_WIDTH*3.0f / 4.0f) {
				currentArea = &mainArea;
			}
			else if (lastY < SCR_HEIGHT / 3.0f){
				currentArea =  &vesselArea;
			}
			else if (lastY < SCR_HEIGHT * 2.0f / 3.0f) {
				currentArea = &tumorArea;
			}
			else {
				currentArea = &bonesArea;
			}
			// ruler
			if (currentArea->getMode() == RULER) {
				currentArea->setRulerVertex(getObjCoor(lastX, lastY));
			}
			// cut
			if (currentArea-> getMode() == CROSS_INTERSECTION){
				if (currentArea->getCutMode() == 3) {
					if (lastX < currentArea->getViewport()[0] + currentArea->getViewport()[2] / 2) {
						currentArea->selectedCutAreaIndex = 0;
					}
					else {
						currentArea->selectedCutAreaIndex = 1;
					}
				}
				else if (currentArea->getCutMode() == 1) {
					currentArea->setCutFaceVertex(getObjCoor(lastX, lastY));
				}
			}
			// NV
			if (currentArea->getMode() == NEAREST_VESSEL) {
				currentArea->setLocalCoordinate(getObjCoor(lastX, lastY), models[0]);
			}
			// remove
			if (currentArea->getMode() == REMOVE_TUMOR) {
				currentArea->setRemovePos(getObjCoor(lastX, lastY));
			}

			
		}
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}
	// 鼠标右键：用于旋转
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}
	
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// camera.ProcessMouseScroll(yoffset);
}

void initGUI(GLFWwindow* window) {
	// set up GUI context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");
}
/*
 get vertex position from screen coordinate to world coordinate
 */
glm::vec3 getObjCoor(GLfloat x, GLfloat y) {
	GLfloat z;
	glm::mat4 modelview = currentArea->getCamera()->GetViewMatrix();
	glm::mat4 proj = currentArea->getCamera()->getOrthology();
	glm::vec4 viewport = currentArea->getViewport();
	x = x;
	y = SCR_HEIGHT - y;
	
	glReadBuffer(GL_BACK);
	glReadPixels(x,y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	glm::vec3 win = glm::vec3(x, y, z);
	glm::vec3 ans = glm::unProject(win, modelview, proj, viewport);

	return ans;
}

