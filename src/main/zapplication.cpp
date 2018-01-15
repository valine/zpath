
#include "zapplication.h"

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void ZApplication::onWindowChange(int width, int height) {
	viewController->onWindowChange(width, height);
}


ZApplication::ZApplication(std::string resourcePath) {
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    window = glfwCreateWindow(640, 480, "ZPath", NULL, NULL);

    if (!window) {
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

    if (err != GLEW_OK) {
    	exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);  
    glfwSwapInterval(1);
    
    viewController = new ZViewController(resourcePath);

    while (!glfwWindowShouldClose(window)) {
     
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT);

        viewController->draw();

        glfwSwapBuffers(window);
        glfwWaitEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

