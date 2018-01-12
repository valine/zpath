#include "zapplication.h"
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "zshader.h"

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

ZApplication::ZApplication() {

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;
    glfwSetErrorCallback(error_callback);

   
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    window = glfwCreateWindow(640, 480, "Oh SHIZZLE it's dat boi", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    
    GLenum err = glewInit();

    
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  

    // vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    // glCompileShader(vertex_shader);
    // fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    // glCompileShader(fragment_shader);
   
    // program = glCreateProgram();
   
    // glAttachShader(program, vertex_shader);
    // glAttachShader(program, fragment_shader);
    // glLinkProgram(program);

    ZShader shader = ZShader("./bin/resources/shaders/uivertexshader.glsl", 
    	"./bin/resources/shaders/uifragmentshader.glsl");
   
    shader.use();

    mvp_location = glGetUniformLocation(shader.mID, "MVP");
    vpos_location = glGetAttribLocation(shader.mID, "vPos");
    vcol_location = glGetAttribLocation(shader.mID, "vCol");
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) 0);
    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) (sizeof(float) * 2));
    while (!glfwWindowShouldClose(window)) {
     
        glm::mat4 matrix;
        matrix = glm::rotate(matrix, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

       
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(matrix));

        float ratio;
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(program);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);

}

