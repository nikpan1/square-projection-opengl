#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource {
    std::string VertexSource;
    std::string FragmentSource;
};

static struct ShaderProgramSource ParseShader(const std::string& filepath) {
    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::ifstream stream(filepath);
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else ss[(int)type] << line << '\n';
    }
    return { ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    // Error handling
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    std::cout << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader compile status: " << result << std::endl;
    if ( result == GL_FALSE )
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout 
            << "Failed to compile "
            << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << "shader"
            << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    int program_linked;

    glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
    std::cout << "Program link status: " << program_linked << std::endl;
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        glGetProgramInfoLog(program, 1024, &log_length, message);
        std::cout << "Failed to link program" << std::endl;
        std::cout << message << std::endl;
    }

    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main() {
	if(!glfwInit()) {
        std::cout << "GLFW was not initialized\n";
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow( 800, 600, "Window", NULL, NULL);
	if(!window) {
        std::cout << "Window was not created\n";
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);        // sync

	if (glewInit()) {
        std::cout << "GLEW was not initialized\n";
        glfwTerminate();
		return -1;
	}
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfwGL3_Init(window, true);
    ImGui::StyleColorsDark();

	unsigned int VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

    float positions[] = {
        -0.5f, -0.5f, 0.0f, // 0
         0.5f, -0.5f, 0.0f, // 1
         0.5f,  0.5f, 0.0f, // 2
        -0.5f,  0.5f, 0.0f  // 3 
        -0.5f, -0.5f, 1.0f, // 0
         0.5f, -0.5f, 1.0f, // 1
         0.5f,  0.5f, 1.0f, // 2
        -0.5f,  0.5f, 1.0f  // 3
    };

        unsigned int indices[] = {0, 1, 2,  2, 3, 0,
                                  3, 4, 5,  5, 6, 0  };  // 4 triangles
    
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
        
        unsigned int vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

        // index    |   size from 1 to 4    | type  | normalized?   | stride    |   offset
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        unsigned int ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
        unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
        glUseProgram(shader);

        int uniformColor = glGetUniformLocation(shader, "u_Color");
        auto uniformMtx = glGetUniformLocation(shader, "mtx");
        GLfloat color[] = {1.0f, 1.0f, 1.0f, 1.0f};
        
        float rotator = 0.f;
        float near = 0.1f;
        float far = 100.0f;
    
        glm::mat4 model = glm::mat4(1.0f); // Initialize as identity matrix for this example
        glm::mat4 projection = glm::perspective(glm::radians(rotator), 800.0f / 600.0f, near, far); // Example perspective projection
        glm::vec4 viewport = glm::vec4(0.0f, 0.0f, 800.0f, 600.0f); // Example viewport of size 800x600

        //left right bottom top z-Near  z-Fear
        //float matx[6] = { -1.0f, 1.0f, -0.5f, 0.5f, 0.1f, 100.0f };
        //glm::mat4 projectionMatrix = glm::ortho(matx[0], matx[1], matx[2], matx[3], matx[4], matx[5]);
        //glUniformMatrix4fv(uniformMtx, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        
        while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
            glClearColor(0.3f, .1f, 0.4f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplGlfwGL3_NewFrame();
            
            glUniform4f(uniformColor, color[0], color[1], color[2], color[3]);
            
            model = glm::mat4(1.0f); // Initialize as identity matrix for this example
            //projection = glm::perspective(glm::radians(rotator), 800.0f / 600.0f, near, far); // Example perspective projection
            //viewport = glm::vec4(0.0f, 0.0f, 800.0f, 600.0f); // Example viewport of size 800x600
            //glUniformMatrix4fv(uniformMtx, 1, GL_FALSE, glm::value_ptr(projection));
            {
                ImGui::SliderFloat("rotator", &rotator, 0.0f, 360.0f);
                ImGui::InputFloat("near", &near, 0.0f, 1.0f);
                ImGui::SliderFloat("far", &far, 0.0f, 10000.0f);
                ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }
            
            glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr);       
            ImGui::Render();
            ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
 
            glfwSwapBuffers(window);
            glfwPollEvents();
        }

	glDeleteBuffers(1, &buffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(shader);

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}

