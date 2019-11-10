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
const unsigned int SCR_WIDTH = 1800;
const unsigned int SCR_HEIGHT = 1200;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 50.0f);
GLfloat Near = 0.1f;
Camera camera(cameraPos);
Camera* curCamera = NULL;
glm::mat4x4* curTransformMat = NULL;
// mouse parameters
	// current mouse position
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

	// dragging mode detection
bool rbutton_down = false;
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
	BaseModel vessel("vessel.stl", 1);
	BaseModel tumor("tumor.stl", 2);
	BaseModel bones("bones.stl", 3);
	vessel.initVertexObject();
	tumor.initVertexObject();
	bones.initVertexObject();
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
	tmp.push_back(1);
	vesselArea.setModelsID(tmp);
	
	tmp.clear();
	tmp.push_back(2);
	tumorArea.setModelsID(tmp);

	tmp.clear();
	tmp.push_back(3);
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
		GLint viewPosLoc = glGetUniformLocation(ourShader.ID, "viewPos");
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, light.Position.x, light.Position.y, light.Position.z);
		glUniform3f(viewPosLoc, camera.Position.x, camera.Position.y, camera.Position.z);

		// send parameters of camera
		// set camera related matrix
		mainArea.setBound(0, 0, SCR_WIDTH * 3 /4.0f, SCR_HEIGHT);
		mainArea.draw(ourShader, models);

		/*ourShader.setInt("type", models[0].getColor());
		models[0].draw();
		ourShader.setInt("type", models[1].getColor());
		models[1].draw();
		ourShader.setInt("type", models[2].getColor());
		models[2].draw();*/
		/*
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		
		//for (int i = 1; i < 4; i++) {
			ourShader.setInt("type", vessel.getColor());
			vessel.draw();
		//}

		// draw light
		/*lightShader.use();
		lightShader.setMat4("projection", camera.projection);
		// camera/view transformation
		lightShader.setMat4("view", glm::translate(camra,light.Position));
		model = glm::mat4(1.0f);
		lightShader.setMat4("model", model);
		light.draw();*/


		// ----------------------------
		/*glViewport(SCR_WIDTH * 3 / 4.0f, 0, SCR_WIDTH * 3 / 4.0f, height/3.0f);
		ourShader.use();
		model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		ourShader.setInt("type", 1);
		vessel.draw();

		glViewport(SCR_WIDTH * 3 / 4.0f, height / 3.0f, SCR_WIDTH * 3 / 4.0f, height / 3.0f);
		ourShader.use();
		model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		ourShader.setInt("type", 2);
		tumor.draw();

		glViewport(SCR_WIDTH * 3 / 4.0f, height*2 / 3.0f, SCR_WIDTH * 3 / 4.0f, height / 3.0f);
		ourShader.use();
		model = glm::translate(model, glm::vec3(-4.38f, -201.899f, 148.987f));
		ourShader.setInt("type", 3);
		bones.draw();*/


		// display GUI
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::Begin("Options", NULL, ImGuiWindowFlags_MenuBar);
		ImGui::InputFloat("lightPos x", &light.Position.x);
		ImGui::InputFloat("lightPos y", &light.Position.y);
		ImGui::InputFloat("lightPos z", &light.Position.z);
		ImGui::Checkbox("标尺", &toolbar.ruler);
		ImGui::Checkbox("截面", &toolbar.cutface);
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
		(*(*currentArea).getCamera()).ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		(*(*currentArea).getCamera()).ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		(*(*currentArea).getCamera()).ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		(*(*currentArea).getCamera()).ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		(*(*currentArea).getCamera()).alterMouse();

	
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
		 (*currentArea).setTransformMat(rotationMatY * rotationMatX *((*currentArea).getTransformMat()));
		// camera.ProcessMouseDragging(translationMat);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	// select area
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action) {
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
	}
	// mouse dragging
	if (!toolbar.ruler && !toolbar.cutface) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (GLFW_PRESS == action)
				rbutton_down = true;
			else if (GLFW_RELEASE == action)
				rbutton_down = false;
		}
	}
	// mouse click, ruler mode
	if (toolbar.ruler) {
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (GLFW_PRESS == action) {
				// deal with ray detection
				std::cout << "lastX: " << lastX << " lastY: " << lastY << std::endl;
				float scale_h = tanf(camera.Zoom)*Near;   //投影窗口高度的一半
				float scale_w = ((GLfloat)SCR_WIDTH/SCR_HEIGHT) * scale_h;
				// TODO: 改成viewport的尺寸
				// aim is the mouse position in range (-1.0, 1.0), clip coordinate
				//glm::vec3 aim = glm::vec3(scale_w*(static_cast<float>(lastX) / static_cast<float>(SCR_WIDTH)*2.f - 1.f),
					//scale_h*(1.f - static_cast<float>(lastY) / static_cast<float>(SCR_HEIGHT)*2.f), 0.0f);
				glm::vec3 aim = glm::vec3(lastX, SCR_HEIGHT - lastY, 1.0f);
				// std::cout << "mouse position " << aim.x << " " << aim.y << " " << aim.z << std::endl;
				// from view coordinate to world coordinate
				glm::vec3 viewCor = glm::unProject(aim, camera.GetViewMatrix(), glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, Near, 1000.0f), glm::vec4(0, 0, SCR_WIDTH, SCR_HEIGHT));
				std::cout << "mouse position " << viewCor.x << " " << viewCor.y << " " << viewCor.z << std::endl;
			}
				
		}
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
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
