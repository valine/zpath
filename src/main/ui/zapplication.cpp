#include "ui/zapplication.h"
#include <unistd.h>

static void error_callback(int error, const char* description) {
    cout<<description;
    cout<<"hello";
    fprintf(stderr, "Error: %s\n", description);
}

void ZApplication::onWindowResize(int width, int height) {
	viewController->onWindowChange(width, height);
}

void ZApplication::onWindowMove(GLFWwindow *window) {
	mShouldSwapBuffer = false;
}

void ZApplication::onKeyPress(int key, int scancode, int action, int mods) {
	viewController->onKeyPress(key, scancode, action, mods);
}

void ZApplication::onMouseEvent(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
	viewController->onMouseEvent(button, action, mods, xpos, ypos);
}

void ZApplication::onCursorPosChange(double x, double y) {
	viewController->onCursorPosChange(x, y);
}

void ZApplication::onScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
    viewController->onScrollChange(xoffset, yoffset);
}

void ZApplication::onFileDrop(GLFWwindow* window, int count, const char** paths) {
    viewController->onFileDrop(count, paths);
}

ZApplication::ZApplication(ZViewController* controller) {
    init(controller, "ZPath");
}

ZApplication::ZApplication(ZViewController* controller, string name) {
    init(controller, name);
}

void ZApplication::init(ZViewController* controller, string windowName) {

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    const char * c = windowName.c_str();
    window = glfwCreateWindow(1000, 700, c, NULL, NULL);

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

    glfwSetWindowSizeCallback(window,
    [] (GLFWwindow* window, int width, int height) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
           thiz->onWindowResize(width, height);
    });

    glfwSetWindowPosCallback(window,
    [] (GLFWwindow* window, int x, int y) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
           thiz->onWindowMove(window);
    });

    glfwSetKeyCallback(window,
    [] (GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
      	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
       		glfwSetWindowShouldClose(window, true);
            thiz->onKeyPress(key, scancode, action, mods);
    });

    glfwSetMouseButtonCallback(window,
    [] (GLFWwindow* window, int button, int action, int mods) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
      	
           thiz->onMouseEvent(window, button, action, mods);
    });

    glfwSetCursorPosCallback(window,
    [] (GLFWwindow* window, double xpos, double ypos) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
           thiz->onCursorPosChange(xpos, ypos);
           glfwSwapBuffers(window);
    });

    glfwSetDropCallback(window,
    [] (GLFWwindow* window, int count, const char** paths) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
           thiz->onFileDrop(window, count, paths);
           glfwSwapBuffers(window);
    });

    glfwSetScrollCallback(window,
    [] (GLFWwindow* window, double xoffset, double yoffset) {
        auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
           thiz->onScrollEvent(window, xoffset, yoffset);
           glfwSwapBuffers(window);
    });


    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    
   // GLenum err = glewInit();

    // if (err != GLEW_OK) {
    // 	exit(EXIT_FAILURE);
    // }

    glEnable(GL_MULTISAMPLE);  
    glfwSwapInterval(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    viewController = controller;

    viewController->onCreate();
    while (!glfwWindowShouldClose(window)) {
    
        glfwPollEvents();

         int windowWidth, windowHeight;
         glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
   

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        viewController->draw();

        if (mShouldSwapBuffer) {
            viewController->onWindowChange(windowWidth, windowHeight);
        	glfwSwapBuffers(window);
	    }

	    mShouldSwapBuffer = true;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

