#include <utils/zfontstore.h>
#include "ui/zapplication.h"
// #include <unistd.h>
#include <thread>
static void error_callback(int error, const char* description) {
    cout<<description;
    fprintf(stderr, "Error: %s\n", description);
}

ZApplication::ZApplication(ZViewController* controller) {
    vector<ZViewController*> controllers = {controller};
    init(controllers, "ZPath", true, 800, 500, nullptr);
}

ZApplication::ZApplication(ZViewController* controller, string name) {
    vector<ZViewController*> controllers = {controller};
    init(controllers, name, false, 800, 500, nullptr);
}

ZApplication::ZApplication(ZViewController* controller, string name, bool shouldPoll) {
    vector<ZViewController*> controllers = {controller};
    init(controllers, name, shouldPoll, 800, 500, nullptr);
}

ZApplication::ZApplication(ZViewController* controller, string name, bool shouldPoll, int windowWidth, int windowHeight) {
    vector<ZViewController*> controllers = {controller};
    init(controllers, name, shouldPoll, windowWidth, windowHeight, nullptr);
}

ZApplication::ZApplication(vector<ZViewController*> controllers) {
    init(controllers, "ZPath", true, 800, 500, nullptr);
}

ZApplication::ZApplication(vector<ZViewController*> controllers, string name) {
    init(controllers, name, false, 800, 500, nullptr);
}

ZApplication::ZApplication(vector<ZViewController*> controllers, string name, bool shouldPoll) {
    init(controllers, name, shouldPoll, 800, 500, nullptr);
}

ZApplication::ZApplication(vector<ZViewController*> controllers, string name, bool shouldPoll, int windowWidth, int windowHeight) {
    init(controllers, name, shouldPoll, windowWidth, windowHeight, nullptr);
}

void
ZApplication::init(vector<ZViewController *> controllers, string windowName, bool shouldPoll, int width, int height,
                   ZApplication *application) {

    mShouldPoll = shouldPoll;

    mViewControllers = std::move(controllers);

    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    int index = 0;
    for (ZViewController* controller : mViewControllers) {
        thread t2(startUiThread, controller, mShouldPoll, this, windowName, width, height);

        if (index < mViewControllers.size() - 1) {
            t2.detach();
        } else {
            t2.join();
        }

        index++;
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

void ZApplication::startUiThread(ZViewController *viewController, bool shouldPoll, ZApplication *app, string windowName,
                            int width, int height) {
    glfwSetErrorCallback(error_callback);

    GLFWwindow *window;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    const char *c = windowName.c_str();
    window = glfwCreateWindow(width, height, c, NULL, NULL);
    app->mWindows.insert(pair<GLFWwindow*, ZViewController*>(window, viewController));

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetWindowUserPointer(window, reinterpret_cast<void *>(app));

    glfwSetWindowSizeCallback(window,
                              [](GLFWwindow *window, int width, int height) {
                                  auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                                  thiz->onWindowResize(width, height, thiz->mWindows.at(window));
                              });

    glfwSetWindowPosCallback(window,
                             [](GLFWwindow *window, int x, int y) {
                                 auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                                 thiz->onWindowMove(window);
                             });

    glfwSetKeyCallback(window,
                       [](GLFWwindow *window, int key, int scancode, int action, int mods) {
                           auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                           if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
                               glfwSetWindowShouldClose(window, true);
                           thiz->onKeyPress(key, scancode, action, mods, thiz->mWindows.at(window));
                       });

    glfwSetMouseButtonCallback(window,[](GLFWwindow *window, int button, int action, int mods) {
                                   auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                                    double xpos, ypos;
                                    glfwGetCursorPos(window, &xpos, &ypos);
                                    thiz->onMouseEvent(window, button, action, mods, xpos, ypos);
                               });

    glfwSetCursorPosCallback(window,
                             [](GLFWwindow *window, double xpos, double ypos) {
                                 auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                                 thiz->onCursorPosChange(xpos, ypos, thiz->mWindows.at(window));
                             });

    glfwSetDropCallback(window,
                        [](GLFWwindow *window, int count, const char **paths) {
                            auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                            thiz->onFileDrop(window, count, paths, thiz->mWindows.at(window));
                        });

    glfwSetScrollCallback(window,
                          [](GLFWwindow *window, double xoffset, double yoffset) {
                              auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                              thiz->onScrollEvent(window, xoffset, yoffset, thiz->mWindows.at(window));
                          });

    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

    string resourceString = viewController->getResourcePath() + "resources/fonts/roboto/Roboto-Bold.ttf";
    ZFontStore::getInstance().loadFont(resourceString);
    ZFontStore::getInstance().setDefaultResource(resourceString);

    glEnable(GL_MULTISAMPLE);
    glfwSwapInterval(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    vec4 background = viewController->getBackgroundColor();

    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);

    viewController->onCreate();

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    //  glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    viewController->onWindowChange(windowWidth, windowHeight);
    viewController->draw();
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window)) {
        if (shouldPoll) {
            glfwPollEvents();
        } else {
            glfwWaitEvents();
        }

        viewController->draw();
        glfwSwapBuffers(window);
    }

}

void ZApplication::setShouldPollEvents(bool shouldPoll) {
    mShouldPoll = shouldPoll;
}

void ZApplication::onWindowResize(int width, int height, ZViewController *viewController) {

    vec4 background = viewController->getBackgroundColor();
    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    viewController->onWindowChange(width, height);
}

void ZApplication::onWindowMove(GLFWwindow *window) {
    mShouldSwapBuffer = false;
}

void ZApplication::onKeyPress(int key, int scancode, int action, int mods, ZViewController* viewController) {
    viewController->onKeyPress(key, scancode, action, mods);
}

void ZApplication::onMouseEvent(GLFWwindow* window, int button, int action, int mods, double xpos, double ypos) {
    mWindows.at(window)->onMouseEvent(button, action, mods, xpos, ypos);
}

void ZApplication::onCursorPosChange(double x, double y, ZViewController *viewController) {
    viewController->onCursorPosChange(x, y);
}

void ZApplication::onScrollEvent(GLFWwindow *window, double xoffset, double yoffset, ZViewController *viewController) {
    viewController->onScrollChange(xoffset, yoffset);
}

void ZApplication::onFileDrop(GLFWwindow *window, int count, const char **paths, ZViewController *viewController) {
    viewController->onFileDrop(count, paths);
}