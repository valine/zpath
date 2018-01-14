#include "zapplication.h"
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtx/string_cast.hpp"

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>


using glm::mat4;
using glm::ortho;
using std::cout;
using glm::to_string;


static const struct
{
    float x, y;
    float r, g, b;
} vertices[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};


static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}


void ZApplication::onWindowChange(int width, int height) {
	view->onWindowChange(width, height);
}


ZApplication::ZApplication(std::string dataPath) {
	mDataPath = dataPath;
  
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    window = glfwCreateWindow(640, 480, "ZPath", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

    glfwSetWindowSizeCallback(window,
    [] (GLFWwindow* window, int width, int height) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
           thiz->onWindowChange(width, height);
        
    });

    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    
    GLenum err = glewInit();

    glEnable(GL_MULTISAMPLE);  
    glfwSwapInterval(1);
    
    GLint vp_location;

    std::string vertexPath = mDataPath + "resources/shaders/uivertexshader.glsl";
    std::string fragmentPath = mDataPath + "resources/shaders/uifragmentshader.glsl";
    ZShader uiShader = ZShader(vertexPath, fragmentPath);
 

    view = new ZView(300, 500, mDataPath, uiShader.mID);
    view->setMargin(10,10,10,10);

    vp_location = glGetUniformLocation(uiShader.mID, "uVPMatrix");

    while (!glfwWindowShouldClose(window)) {
     
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);

        mat4 projection = ortho(0.0f, (float)windowWidth,(float)windowHeight,0.0f, -10.0f, 100.0f);
       // mat4 projection = glm::perspective(glm::radians(45.0f), (float)width/(float)height, 0.1f, 100.0f);

        uiShader.use();

        mat4 matrix;
        matrix = glm::rotate(matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        glUniformMatrix4fv(vp_location, 1, GL_FALSE, glm::value_ptr(projection));

        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        view->draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

}

