#include "ui/zapplication.h"
#include <unistd.h>

static void error_callback(int error, const char* description) {
    cout<<description;
    fprintf(stderr, "Error: %s\n", description);
}

ZApplication::ZApplication(ZViewController* controller) {
    init(controller, "ZPath", true, 500, 500);
}

ZApplication::ZApplication(ZViewController* controller, string name) {
    init(controller, name, false, 500, 500);
}

ZApplication::ZApplication(ZViewController* controller, string name, bool shouldPoll) {
    init(controller, name, shouldPoll, 500, 500);
}

void ZApplication::init(ZViewController* controller, string windowName, bool shouldPoll, int width, int height) {

    mShouldPoll = shouldPoll;
    
    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    const char * c = windowName.c_str();
    window = glfwCreateWindow(500, 500, c, NULL, NULL);

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
            glfwPostEmptyEvent();
        });

    glfwSetCursorPosCallback(window,
        [] (GLFWwindow* window, double xpos, double ypos) {
            auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
            thiz->onCursorPosChange(xpos, ypos);
        });

    glfwSetDropCallback(window,
        [] (GLFWwindow* window, int count, const char** paths) {
            auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
            thiz->onFileDrop(window, count, paths);
        });

    glfwSetScrollCallback(window,
        [] (GLFWwindow* window, double xoffset, double yoffset) {
            auto thiz = reinterpret_cast<ZApplication*>(glfwGetWindowUserPointer(window));
            thiz->onScrollEvent(window, xoffset, yoffset);
        });

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    glEnable(GL_MULTISAMPLE);  
    glfwSwapInterval(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    viewController = controller;
    viewController->onCreate();

    int windowWidth, windowHeight;
    glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    viewController->onWindowChange(windowWidth, windowHeight);
    viewController->draw();
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window)) {
        if (mShouldPoll) {
            glfwPollEvents();
        } else {
            glfwWaitEvents();
        }

        viewController->draw();
        glfwSwapBuffers(window);
    }

    viewController->onTerminate();
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void ZApplication::setShouldPollEvents(bool shouldPoll) {
    mShouldPoll = shouldPoll;
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