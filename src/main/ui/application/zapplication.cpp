#include <utils/zfontstore.h>
#include "ui/zapplication.h"
// #include <unistd.h>
#include <thread>
#include <utils/zsettingsstore.h>
#include <zconf.h>
#include "ui/stb_image.h"

static void error_callback(int error, const char* description) {
    cout<< "GLFW error " << description << endl;
    cout<< "GLFW error " << error << endl;
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

ZApplication::ZApplication(ZViewController *controller, string name, bool shouldPoll, int windowWidth,
                           int windowHeight, string icon, function<void()> onStart) {
    mOnStartListener = std::move(onStart);
    vector<ZViewController*> controllers = {controller};
    init(controllers, std::move(name), shouldPoll, windowWidth, windowHeight, nullptr);
    mIconPath = std::move(icon);
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
        startUiThread(controller, mShouldPoll, this, windowName, width, height);
        index++;
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
}


void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    std::cout << "[OpenGL Error](" << type << ") " << message << std::endl;
}

void ZApplication::startUiThread(ZViewController *viewController, bool shouldPoll, ZApplication *app, string windowName,
                            int width, int height) {
    glfwSetErrorCallback(error_callback);

    GLFWwindow *window;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    const char *c = windowName.c_str();
    window = glfwCreateWindow(width, height, c, NULL, NULL);
    app->mWindows.insert(pair<GLFWwindow*, ZViewController*>(window, viewController));

    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


    glfwSetWindowUserPointer(window, reinterpret_cast<void *>(app));

    if (!app->mIconPath.empty()) {
        int iconWidth, iconHeight, nrComponents;
        string path = viewController->getResourcePath();
        unsigned char *data = stbi_load(app->mIconPath.c_str(), &iconWidth, &iconHeight, &nrComponents, 0);

        GLFWimage icons[1];
        icons[0].pixels = data;
        icons[0].width = iconWidth;
        icons[0].height = iconHeight;
        glfwSetWindowIcon(window, 1, icons);
        stbi_image_free(data);
    }

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
                           if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                             //  glfwSetWindowShouldClose(window, true);
                           }
                           thiz->onKeyPress(key, scancode, action, mods, thiz->mWindows.at(window));
                       });


    glfwSetCharCallback(window,
                       [](GLFWwindow *window, unsigned int code) {
                           auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                           thiz->onCharacterInput(code, thiz->mWindows.at(window));
                       });


    glfwSetMouseButtonCallback(window,[](GLFWwindow *window, int button, int action, int mods) {
                                   auto thiz = reinterpret_cast<ZApplication *>(glfwGetWindowUserPointer(window));
                                    double xpos, ypos;
                                    glfwGetCursorPos(window, &xpos, &ypos);
        thiz->onMouseEvent(window, button, action, mods, xpos, ypos, thiz->mWindows.at(window));
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


    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    int frameSizeW, frameSizeH;
    glfwGetFramebufferSize(window, &frameSizeW, &frameSizeH);
    float dpScale = (float) frameSizeW / (float) windowWidth;
    viewController->onDpChange(dpScale);

    string resourceString = viewController->getResourcePath() + "resources/fonts/roboto/Roboto-Bold.ttf";
    ZFontStore::getInstance().loadFont(resourceString, dpScale);
    ZFontStore::getInstance().setDefaultResource(resourceString);
    ZSettingsStore::getInstance().setResourcePath(viewController->getResourcePath());

    glEnable(GL_MULTISAMPLE);

    if (dpScale == 1) {
        glEnable(GL_LINE_SMOOTH);
    }
    glfwSwapInterval(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    vec4 background = viewController->getBackgroundColor();

    glClearColor(background.r, background.g, background.b, background.a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Draw first frame during application startup
    glfwSwapBuffers(window);

    viewController->onCreate();

    viewController->onWindowChange(windowWidth, windowHeight);
    viewController->onLayoutFinished();


    if (mOnStartListener != nullptr) {
        mOnStartListener();
    }

    viewController->onWindowChange(windowWidth, windowHeight);
    viewController->draw();
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window)) {
        if (shouldPoll) {
            glfwPollEvents();
        } else {
            glfwWaitEvents();
        }

//#ifdef __APPLE__
        // MacOS corrupts the back buffer so the partial
        // view update optimization won't work until
        // the ui renders to an off screen framebuffer
        viewController->invalidate();
//#endif
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

void ZApplication::onCharacterInput(unsigned int character, ZViewController *viewController) {
    viewController->onCharacterInput(character);
}

void ZApplication::onKeyPress(int key, int scancode, int action, int mods, ZViewController* viewController) {
    viewController->onKeyPress(key, scancode, action, mods);
}

void ZApplication::onMouseEvent(GLFWwindow *window, int button, int action, int mods, double xpos, double ypos,
                                ZViewController *viewController) {
    viewController->onMouseEvent(button, action, mods, xpos, ypos);
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
