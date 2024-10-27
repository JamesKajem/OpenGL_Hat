#include "ViewManager.h"
#include <cmath>
#include <glm/gtx/string_cast.hpp> // for debugging :)

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>    

// declarations for global variables and defines
namespace
{
	// Variables for window width and height
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 800;
	const char* g_ViewName = "view";
	const char* g_ProjectionName = "projection";

	// camera object used for viewing and interacting with
	// the 3D scene
	Camera* g_pCamera = nullptr;

	// these variables are used for mouse movement processing
	float gLastX = WINDOW_WIDTH / 2.0f;
	float gLastY = WINDOW_HEIGHT / 2.0f;
	bool gFirstMouse = true;

	// time between current frame and last frame
	float gDeltaTime = 0.0f;
	float gLastFrame = 0.0f;

	// the following variable is false when orthographic projection
	// is off and true when it is on
	bool bOrthographicProjection = false;

	float cameraSpeed; // Speed of the camera movement
	float yaw; // Horizontal angle
	float pitch; // Vertical angle
	float lastX; // Last mouse X position
	float lastY; // Last mouse Y position
	bool firstMouse; // To check if the mouse is being moved for the first time
}

/***********************************************************
 *  ViewManager()
 *
 *  The constructor for the class
 ***********************************************************/
ViewManager::ViewManager(
	ShaderManager* pShaderManager)
{
	// initialize the member variables
	m_pShaderManager = pShaderManager;
	m_pWindow = NULL;
	g_pCamera = new Camera();
	// default camera view parameters
	g_pCamera->Position = glm::vec3(0.0f, 9.0f, 18.0f);
	g_pCamera->Front = glm::vec3(0.0f, -0.8f, -3.0f);
	g_pCamera->Up = glm::vec3(0.0f, 1.0f, 0.0f);
	g_pCamera->Zoom = 80;

	// Initialize camera speed
	cameraSpeed = 2.5f;

	firstMouse = true; // Initialize in the constructor
}

/***********************************************************
 *  ~ViewManager()
 *
 *  The destructor for the class
 ***********************************************************/
ViewManager::~ViewManager()
{
	// free up allocated memory
	m_pShaderManager = NULL;
	m_pWindow = NULL;
	if (NULL != g_pCamera)
	{
		delete g_pCamera;
		g_pCamera = NULL;
	}
}

/***********************************************************
 *  CreateDisplayWindow()
 *
 *  This method is used to create the main display window.
 ***********************************************************/
GLFWwindow* ViewManager::CreateDisplayWindow(const char* windowTitle)
{
	GLFWwindow* window = nullptr;

	// try to create the displayed OpenGL window
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		windowTitle,
		NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	// Set the input mode for the mouse cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set the scroll callback
	glfwSetScrollCallback(window, Mouse_Scroll_Callback);

	// enable blending for supporting tranparent rendering
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_pWindow = window;

	return(window);
}

/***********************************************************
 *  Mouse_Position_Callback()
 *
 *  This method is automatically called from GLFW whenever
 *  the mouse is moved within the active GLFW display window.
 ***********************************************************/
void ViewManager::Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos)
{
	if (firstMouse) // Check if it's the first time moving the mouse
	{
		lastX = static_cast<float>(xMousePos);
		lastY = static_cast<float>(yMousePos);
		firstMouse = false; // Set to false after the first check
	}

	float xoffset = static_cast<float>(xMousePos) - lastX; // Calculate offset
	float yoffset = lastY - static_cast<float>(yMousePos); // Inverted Y axis
	lastX = static_cast<float>(xMousePos); // Update last X
	lastY = static_cast<float>(yMousePos); // Update last Y

	float sensitivity = 0.1f; // Sensitivity for mouse movement
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset; // Update yaw
	pitch += yoffset; // Update pitch

	// Constrain the pitch angle
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	// Update the camera's front vector (yaw and pitch)
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	g_pCamera->Front = glm::normalize(front); // Update camera front
}

void ViewManager::Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset)
{
	cameraSpeed += static_cast<float>(yOffset); // Adjust camera speed with scroll
	if (cameraSpeed < 1.0f) // Limit minimum speed
		cameraSpeed = 1.0f;
	if (cameraSpeed > 10.0f) // Limit maximum speed
		cameraSpeed = 10.0f;
}

/***********************************************************
 *  ProcessKeyboardEvents()
 *
 *  This method is called to process any keyboard events
 *  that may be waiting in the event queue.
 ***********************************************************/
void ViewManager::ProcessKeyboardEvents()
{
	// close the window if the escape key has been pressed
	if (glfwGetKey(m_pWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(m_pWindow, true);
	}

	// Switch between perspective and orthographic projections
	if (glfwGetKey(m_pWindow, GLFW_KEY_P) == GLFW_PRESS)
	{
		bOrthographicProjection = false; // Set to perspective
	}
	if (glfwGetKey(m_pWindow, GLFW_KEY_O) == GLFW_PRESS)
	{
		bOrthographicProjection = true; // Set to orthographic
	}

	// Camera movement controls
	if (glfwGetKey(m_pWindow, GLFW_KEY_W) == GLFW_PRESS)
		g_pCamera->Position += cameraSpeed * g_pCamera->Front * gDeltaTime; // Move forward
	if (glfwGetKey(m_pWindow, GLFW_KEY_S) == GLFW_PRESS)
		g_pCamera->Position -= cameraSpeed * g_pCamera->Front * gDeltaTime; // Move backward
	if (glfwGetKey(m_pWindow, GLFW_KEY_A) == GLFW_PRESS)
		g_pCamera->Position -= glm::normalize(glm::cross(g_pCamera->Front, g_pCamera->Up)) * cameraSpeed * gDeltaTime; // Move left
	if (glfwGetKey(m_pWindow, GLFW_KEY_D) == GLFW_PRESS)
		g_pCamera->Position += glm::normalize(glm::cross(g_pCamera->Front, g_pCamera->Up)) * cameraSpeed * gDeltaTime; // Move right
	if (glfwGetKey(m_pWindow, GLFW_KEY_Q) == GLFW_PRESS)
		g_pCamera->Position += cameraSpeed * g_pCamera->Up * gDeltaTime; // Move up
	if (glfwGetKey(m_pWindow, GLFW_KEY_E) == GLFW_PRESS)
		g_pCamera->Position -= cameraSpeed * g_pCamera->Up * gDeltaTime; // Move down
}

/***********************************************************
 *  PrepareSceneView()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void ViewManager::PrepareSceneView()
{
	glm::mat4 view;
	glm::mat4 projection;

	// per-frame timing
	float currentFrame = glfwGetTime();
	gDeltaTime = currentFrame - gLastFrame;
	gLastFrame = currentFrame;

	// process any keyboard events that may be waiting in the event queue
	ProcessKeyboardEvents();

	// get the current view matrix from the camera
	view = g_pCamera->GetViewMatrix();

	// define the current projection matrix (now based on the projection type)
	if (bOrthographicProjection) {
		// Orthographic projection
		float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
		projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, 0.1f, 100.0f);
	}
	else {
		// Perspective projection
		projection = glm::perspective(glm::radians(g_pCamera->Zoom), (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT, 0.1f, 100.0f);
	}

	// if the shader manager object is valid
	if (NULL != m_pShaderManager)
	{
		// set the view matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ViewName, view);
		// set the projection matrix into the shader for proper rendering
		m_pShaderManager->setMat4Value(g_ProjectionName, projection);
		// set the view position of the camera into the shader for proper rendering
		m_pShaderManager->setVec3Value("viewPosition", g_pCamera->Position);
	}
}