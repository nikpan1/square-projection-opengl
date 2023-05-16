#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include <math.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

float WINDOW_WIDTH = 800.f;
float WINDOW_HEIGHT = 600.f;

GLuint VBO;
GLuint IBO;
GLuint gWorldLocation;


static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.01f;
                            // angle | axis
    glm::mat4 rotation = glm::rotate(glm::radians(45.f), glm::vec3(0.f, 1.f, 0.f));
    glm::mat4 translation = glm::translate(glm::vec3(1.f, 0.f, 0.f));
    float FOV = glm::radians(45.f);
    float ratio = WINDOW_WIDTH / WINDOW_HEIGHT;
    float zNear = 0.1f, zFar = 100.f;
    glm::mat4 projection = glm::perspective(FOV, ratio, zNear, zFar);
    
    auto mvp = projection * rotation * translation;

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, glm::value_ptr(mvp));

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);

    // color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    glfwSwapBuffers(win);
    glfwPollEvents();
}

static void CreateVertexBuffer()
{
    float Vertices[] = {
        0.5f, 0.5f, 0.5f,       //0
       -0.5f, 0.5f, -0.5f,
       -0.5f, 0.5f, 0.5f,
        0.5f, -0.5f, -0.5f,
       -0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
       -0.5f, -0.5f, 0.5f,      //7
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}

static void CreateIndexBuffer()
{
    unsigned int Indices[] = {
                              0, 1, 2, //0
                              1, 3, 4, //1
                              5, 6, 3, //2
                              7, 3, 6, //3
                              2, 4, 7, //4
                              0, 7, 6, //5
                              0, 5, 1, //6
                              1, 5, 3, //7
                              5, 0, 6, //8
                              7, 4, 3, //9
                              2, 1, 4, //10
                              0, 2, 7  //11
    };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    GLuint ShaderObj = glCreateShader(ShaderType);

    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(1);
    }

    const GLchar* p[1];
    p[0] = pShaderText;

    GLint Lengths[1];
    Lengths[0] = (GLint)strlen(pShaderText);

    glShaderSource(ShaderObj, 1, p, Lengths);

    glCompileShader(ShaderObj);

    GLint success;
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLchar InfoLog[1024];
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);
}

const char* pVSFileName = "shaders/shader.vs";
const char* pFSFileName = "shaders/shader.fs";

static void CompileShaders()
{
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    std::ifstream file("shaders/shader.vs");
    if (file.is_open()) {
        std::string vs((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    
    std::ifstream file("shaders/shader.fs");
    if (file.is_open()) {
        std::string fs((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    
    std::cout << vs << '\n' << fs << '\n';

    AddShader(ShaderProgram, vs.c_str(), GL_VERTEX_SHADER);
    AddShader(ShaderProgram, fs.c_str(), GL_FRAGMENT_SHADER);

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };

    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    if (gWorldLocation == -1) {
        printf("Error getting uniform location of 'gWorld'\n");
        exit(1);
    }

    glValidateProgram(ShaderProgram);
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);
}

    int main() {
    if (!glfwInit()) {
        std::cout << "GLFW was not initialized\n";
    };

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Window", NULL, NULL);
    if (!window) {
        std::cout << "Window was not created\n";
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);        // sync
    
    if (glewInit()) {
        std::cout << "GLEW was not initialized\n";
        glfwTerminate();
    }
    glClearColor(0.3f, 0.3f, 1.f, 1.f);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    CreateVertexBuffer();
    CreateIndexBuffer();

    CompileShaders();
}
