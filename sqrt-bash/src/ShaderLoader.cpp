#include "ShaderLoader.hpp"
#include "glCall.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>

ShaderLoader::ShaderLoader(shaderType shtype, std::string filepath)
		: source(" "), type(shtype), path(filepath) {
    program = glCreateProgram(); 
    
    Load();
    Compile();
    Create();
}

ShaderLoader::ShaderLoader(shaderType shtype, std::string filepath, unsigned int programId)
		: source(" "), type(shtype), path(filepath), program(programId) {
    Load();
    Compile();
    Create();
}

void ShaderLoader::Load() {
    std::ifstream file(path);
    if (!file.is_open()) __debugbreak();

    std::string line;
    while (std::getline(file, line)) {
        source.append(line + '\n');
    }

    file.close();
}

void ShaderLoader::Compile() {
    id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));

       // __debugbreak();
    }

}

void ShaderLoader::Create() {
    glAttachShader(program, id);
    //glLinkProgram(program);
    //glValidateProgram(program);

    glDeleteShader(id);
}


std::ostream& operator<<(std::ostream& os,
    const ShaderLoader& obj) {
    os << "ShaderLoader: \n" <<
        "type: " << obj.type <<
        "\npath=" << obj.path <<
        "\nid=" << obj.id <<
        "\nsource:\n" << obj.source << "\n\n";

    return os;
}
