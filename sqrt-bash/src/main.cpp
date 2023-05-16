#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "ShaderLoader.hpp"

int main( )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
        GLFWwindow* window = glfwCreateWindow( 1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		glfwTerminate();
		return -1;
	}

        std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	// glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

        float positions[] = {
            -0.5f, -0.5f, // 0
             0.5f, -0.5f, // 1
             0.5f,  0.5f, // 2
            -0.5f,  0.5f  // 3
        };

        unsigned int indices[] = {
          0, 1, 2,
          2, 3, 0
        };

        // Create buffer and copy data
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, 4 * 2 * sizeof(float), positions, GL_STATIC_DRAW);

        // define vertex layout
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
        glEnableVertexAttribArray(0);

        // Create index buffer
        unsigned int ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

		
		ShaderLoader vs(VERTEX_SHADER, "shaders/shader.vs");
		auto program = vs.getProgram();
		ShaderLoader fs(FRAGMENT_SHADER, "shaders/shader.fs", program);
		glUseProgram(program);

	do{
		// Clear the screen
		glClear( GL_COLOR_BUFFER_BIT );
		glClearColor(0.3f, 0.3f, 1.f, 1.f);


		// Draw the triangle !
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &buffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(program);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

