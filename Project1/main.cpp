#include <glad/glad.h>
// GLFW
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
// others
#include <iostream>
#include "Shader.h"
#include "Camera.h"
#include "BaseModel.h"
#include "Light.h"
#include "Toolbar.h"
#include "Area.h"

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

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 50.0f);
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
Light light;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// select area
Area mainArea, vesselArea, tumorArea, bonesArea;
Area* currentArea = &mainArea;
// toolbar
Toolbar toolbar;
int main()
{
	// initialize GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	// set viewport
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	
	// build and compile the shader
	Shader ourShader("phongShader.vs", "phongShader.frag");
	Shader lightShader("camera.vs", "camera.frag");
	// load model
	BaseModel vessel("vessel.stl", 1, TRIANGLE);
	BaseModel tumor("tumor.stl", 2, TRIANGLE);
	BaseModel bones("bones.stl", 3, TRIANGLE);
	vessel.initVertexObject();
	tumor.initVertexObject();
	bones.initVertexObject();
	light.initVertexObject();
	std::vector<BaseModel> models;
	models.push_back(vessel);
	models.push_back(tumor);
	models.push_back(bones);
	// vessel.initVertexObject();

	
	std::vector<GLint> tmp;
	tmp.clear();
	tmp.push_back(0);
	tmp.push_back(1);
	tmp.push_back(2);
	mainArea.setModelsID(tmp);

	tmp.clear();
	tmp.push_back(0);
	vesselArea.setModelsID(tmp);
	
	tmp.clear();
	tmp.push_back(1);
	tumorArea.setModelsID(tmp);

	tmp.clear();
	tmp.push_back(2);
	bonesArea.setModelsID(tmp);

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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// send parameters to the shader
		ourShader.use();
		GLint lightColorLoc = glGetUniformLocation(ourShader.ID, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(ourShader.ID, "lightPos");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, light.Position.x, light.Position.y, light.Position.z);
		glUniform1i(glGetUniformLocation(ourShader.ID, "withLight"), 1);
		
		// send parameters of camera
		// set camera related matrix
		mainArea.setBound(0, 0, SCR_WIDTH*3/4.0f, SCR_HEIGHT);
		mainArea.draw(ourShader, models);
		// draw light
		mainArea.drawLight(lightShader, light);
		mainArea.drawLine(ourShader);

		vesselArea.setBound(SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT * 2 / 3.0f, SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT/3.0f);
		vesselArea.draw(ourShader, models);
		vesselArea.drawLight(lightShader, light);

		tumorArea.setBound(SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT / 3.0f, SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT / 3.0f);
		tumorArea.draw(ourShader, models);
		tumorArea.drawLight(lightShader, light);

		bonesArea.setBound(SCR_WIDTH * 3 / 4.0f, 0, SCR_WIDTH * 3 / 4.0f, SCR_HEIGHT / 3.0f);
		bonesArea.draw(ourShader, models);
		bonesArea.drawLight(lightShader, light);
		
		// display GUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Options", NULL, ImGuiWindowFlags_MenuBar);
		ImGui::Checkbox("RULER", &toolbar.ruler);
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
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		(*currentArea->getCamera()).ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		(*currentArea->getCamera()).alterMouse();

	
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
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

	// todo 根据鼠标的坐标判断选定的区域
	// std::cout << "xpos " << xpos << " ypos " << ypos << std::endl;
	//camera.ProcessMouseMovement(xoffset, yoffset);
	if (lbutton_down) {
		//std::cout << "mouse is clicked" << std::endl;
		// modify camera position
		// todo
		const float M_SPEED = 0.1f;
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(xoffset * deltaTime * M_SPEED, yoffset * deltaTime * M_SPEED, 0));

		// currentArea->getCamera()->setPosition(model * glm::vec4(currentArea->getCamera()->Position, 0));
	}
	if (rbutton_down) {
		// std::cout << "mouse is clicked" << std::endl;
		glm::mat4x4 rotationMatY = glm::mat4x4(1.0f);
		glm::mat4x4 rotationMatX = glm::mat4x4(1.0f);
		
		const float R_SPEED = 0.1f;

		// Rotate the camera around with the mouse according to how much it's moved 
		// since the last frame 

		rotationMatY = glm::rotate(rotationMatY, xoffset * deltaTime * R_SPEED, glm::vec3(0, 1, 0));
		rotationMatX = glm::rotate(rotationMatX, -yoffset * deltaTime * R_SPEED, glm::vec3(1, 0, 0));
		// todo
		 currentArea->setTransformMat(rotationMatY * rotationMatX *(currentArea->getTransformMat()));
		// camera.ProcessMouseDragging(translationMat);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// select area
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action) {
			
			lbutton_down = true;
			// main
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
			
		}
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}
	// right button dragging
	if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		if (GLFW_PRESS == action)
			rbutton_down = true;
		else if (GLFW_RELEASE == action)
			rbutton_down = false;
	}
	// mouse click, ruler mode
	if (toolbar.ruler) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			if (GLFW_PRESS == action) {
				currentArea->setVertex(getObjCoor(lastX, lastY));
			}
				
		}
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

glm::vec3 getObjCoor(GLfloat x, GLfloat y) {
	// todo from screen to viewport
	GLfloat z;
	glm::mat4 modelview = currentArea->getCamera()->GetViewMatrix() * currentArea->getTransformMat();
	glm::mat4 proj = currentArea->getCamera()->getProjection();
	glm::vec4 viewport = currentArea->getBound();
	x = x;
	y = SCR_HEIGHT - y;
	
	glReadBuffer(GL_BACK);
	glReadPixels(x,y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
	glm::vec3 win = glm::vec3(x, y, z);
	glm::vec3 ans = glm::unProject(win, modelview, proj, viewport);
	light.Position.x = ans.x;
	light.Position.y = ans.y;
	light.Position.z = ans.z;
	return ans;
}