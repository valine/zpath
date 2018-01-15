
#include "zapplication.h"
#include <unistd.h>

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void ZApplication::onWindowResize(int width, int height) {
	viewController->onWindowChange(width, height);
}

void ZApplication::onWindowMove(GLFWwindow *window) {
	mShouldSwapBuffer = false;
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
           thiz->onWindowResize(width, height);
           glfwSwapBuffers(window);
        
    });

    glfwSetWindowPosCallback(window,
    [] (GLFWwindow* window, int x, int y) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
           thiz->onWindowMove(window);
    });


    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    
    GLenum err = glewInit();

    if (err != GLEW_OK) {
    	exit(EXIT_FAILURE);
    }

    glEnable(GL_MULTISAMPLE);  
    glfwSwapInterval(2);
    
    viewController = new ZViewController(resourcePath);

    while (!glfwWindowShouldClose(window)) {
    
        glfwWaitEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        viewController->draw();

        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);

        if (mShouldSwapBuffer) {
        	glfwSwapBuffers(window);
	    }

	    mShouldSwapBuffer = true;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

