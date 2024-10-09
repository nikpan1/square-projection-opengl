Based on the repository content and its structure, here's a suggested README for the project:

---

# Square Projection in OpenGL

This project demonstrates a simple implementation of a rotating square using OpenGL. The goal is to provide a minimal example of rendering basic shapes and applying transformations like rotation, projection, and coloring using modern OpenGL techniques.

## Features

- **Rotating Square**: A simple square rotates within the window using OpenGL's transformation matrices.
- **Shader Usage**: Utilizes vertex and fragment shaders to render the square.
- **GLFW & GLEW Integration**: The project uses GLFW for window and context management, while GLEW provides access to modern OpenGL functions.
- **ImGui Integration**: The project includes an example of using ImGui for building graphical interfaces, useful for debugging and testing.

## Prerequisites

To build and run this project, you need:

- OpenGL (version 3.3 or above)
- [GLFW](https://www.glfw.org/) - A library for creating windows with OpenGL contexts.
- [GLEW](http://glew.sourceforge.net/) - The OpenGL Extension Wrangler Library.
- [GLM](https://github.com/g-truc/glm) - A mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications.
- A C++ compiler supporting C++17.

## Building the Project

1. Clone the repository:
   ```bash
   git clone https://github.com/nikpan1/square-projection-opengl.git
   ```
   
2. Open the project in your favorite C++ development environment. This project is compatible with Visual Studio and uses the MSBuild system.

3. Ensure all dependencies (GLFW, GLEW, GLM) are correctly linked.

4. Build and run the project. By default, the application opens a window and displays a rotating square.

## Structure

- **src/**: Contains the source code files:
  - `main.cpp`: Entry point and main loop.
  - `Application.cpp`: Application setup and rendering logic.
  - `ShaderLoader.cpp` & `.hpp`: Handles loading, compiling, and linking of shaders.
  - `glCall.hpp`: A utility for OpenGL function error checking.
- **shaders/**: Contains shader files:
  - `shader.vs`: Vertex shader.
  - `shader.fs`: Fragment shader.
- **imgui/**: Contains ImGui integration files, providing a GUI interface for testing purposes.

## Usage

Run the application, and a window should open displaying a rotating square. You can interact with the ImGui interface to modify parameters such as rotation speed and color.

## Contributing

Feel free to fork the repository, open issues, and submit pull requests to improve the project.

## License

This project is licensed under the MIT License - see the LICENSE file for details.

---

This should provide a comprehensive overview of the project for new developers and users interested in understanding and running the code.
