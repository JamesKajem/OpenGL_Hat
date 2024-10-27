#pragma once

#include "ShaderManager.h"
#include "camera.h"

// GLFW library
#include "GLFW/glfw3.h" 

class ViewManager
{
public:
    // Constructor
    ViewManager(ShaderManager* pShaderManager);
    // Destructor
    ~ViewManager();

    // Mouse position callback for mouse interaction with the 3D scene
    static void Mouse_Position_Callback(GLFWwindow* window, double xMousePos, double yMousePos);

    //Mouse scroll callback
    static void Mouse_Scroll_Callback(GLFWwindow* window, double xOffset, double yOffset);

private:
    // Pointer to shader manager object
    ShaderManager* m_pShaderManager;
    // Active OpenGL display window
    GLFWwindow* m_pWindow;

    // Speed of camera movement
    static float cameraSpeed;

    // Projection type
    bool bOrthographicProjection = false;

    // Process keyboard events for interaction with the 3D scene
    void ProcessKeyboardEvents();

public:
    // Create the initial OpenGL display window
    GLFWwindow* CreateDisplayWindow(const char* windowTitle);

    // Prepare the conversion from 3D object display to 2D scene display
    void PrepareSceneView();
};
