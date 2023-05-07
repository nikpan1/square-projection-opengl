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
#include <array>
#include "projection.cpp"

int main() {
    if (!glfwInit()) {
        std::cout << "GLFW was not initialized\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Window", NULL, NULL);
    if (!window) {
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

    // --------------------------
    float positions[] = {
        0.f, 1.f, 0.f, 0.01f,
        1.f, 1.f, 0.f, 0.02f,
        1.f, 0.f, 0.f, 0.03f,
        0.f, 0.f, 0.f, 0.04f,
        0.f, 1.f, 0.1f, 0.05f, 
        1.f, 1.f, 0.1f, 0.06f,
        1.f, 0.f, 0.1f, 0.07f,
        0.f, 0.f, 0.1f, 0.09f
    };
    float aositions[] = {
        0.f, 1.f, 0.f,
        1.f, 1.f, 0.f,
        1.f, 0.f, 0.f,
        0.f, 0.f, 0.f,
        0.f, 1.f, 0.1f, 
        1.f, 1.f, 0.1f,
        1.f, 0.f, 0.1f,
        0.f, 0.f, 0.1f
    };
    for (int i = 0; i < sizeof(positions) / sizeof(float); i++) positions[i] *= 10.f;

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        7, 5, 4,
        7, 6, 5,
        3, 2, 5,
        3, 5, 6,
        2, 1, 4,
        2, 4, 5,
        0, 7, 6,
        0, 6, 4,
        0, 1, 4,
        0, 7, 4
    };
    // -------------------------


    unsigned int VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

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
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    auto uniformColor = glGetUniformLocation(shader, "u_Color");
    auto uniformMtx = glGetUniformLocation(shader, "mtx");
    GLfloat color[] = { 1.0f, 1.0f, 1.0f, 1.0f };

// ---------------
    float fov = 90.f;
    float near = .1f;
    float far = 100.0f;
    float zCam = -14.f;
    glm::mat4 prj = glm::ortho(0.f, 800.f, 0.f, 600.f, near, far);
    glm::mat4 model = glm::mat4(1.0f); // Initialize as identity matrix for this example
    glm::mat4 projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, near, far); // Example perspective projection
    //glm::vec4 viewport = glm::vec4(0.0f, 0.0f, 800.0f, 600.0f); // Example viewport of size 800x600
    glm::mat4 viewport = glm::lookAt(
        glm::vec3(0, 0, zCam), // Camera is at (4,3,3), in World Space
        glm::vec3(0, 0, 0), // and looks at the origin
        glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
    
    float x_angle = 0.0f;
    float y_angle = 0.0f;  // rotate 45 degrees around the y-axis

    glm::mat4 x_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(x_angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 y_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(y_angle), glm::vec3(1.0f, 1.0f, 0.0f));
// ------------------
        
        while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
            glClearColor(0.3f, 0.1f, 0.4f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplGlfwGL3_NewFrame();

            {
                ImGui::SliderFloat("XAngle", &x_angle, 0.f, 360.f);
                ImGui::SliderFloat("YAngle", &y_angle, 0.f, 360.f);
                ImGui::SliderFloat("zCam", &zCam, -10.f, 10.f);
                ImGui::SliderFloat("Red", &color[0], 0.f, 1.f);
                ImGui::SliderFloat("FOV", &fov, 0.0f, 360.0f);
                ImGui::SliderFloat("near", &near, -2.0f, 2.0f);
                ImGui::SliderFloat("far", &far, -100.0f, 1000.0f);
                ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            }

             viewport = glm::lookAt(
                glm::vec3(0, 0, zCam), // Camera is at (4,3,3), in World Space
                glm::vec3(0, 0, 0), // and looks at the origin
                glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
            );
            x_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(x_angle), glm::vec3(1.0f, 0.0f, 0.0f));
            y_rotation = glm::rotate(glm::mat4(1.0f), glm::radians(y_angle), glm::vec3(0.0f, 1.0f, 0.0f));
            projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, near, far)*viewport*x_rotation*y_rotation;  
            glUniformMatrix4fv(uniformMtx, 1, GL_FALSE, glm::value_ptr(projection));
            glUniform4f(uniformColor, color[0], color[1], color[2], color[3]);
            
            glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, nullptr);
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

