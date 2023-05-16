#include <string>
#include <iostream>

enum shaderType {
	VERTEX_SHADER = 0,
	FRAGMENT_SHADER = 1,
	COMPUTE_SHADER = 2
};

class ShaderLoader {
private:
	unsigned int id;
	unsigned int program;

	shaderType type;
	std::string source;
	std::string path;

private:
	void Load();
	void Compile();
	void Create();

public:
	ShaderLoader(shaderType shtype, std::string filepath);
	ShaderLoader(shaderType shtype, std::string filepath, unsigned int programId);
	int getId() { return id; };
	unsigned int getProgram() { return program; };

	friend std::ostream& operator<<(std::ostream& os, 
		const ShaderLoader& obj);	
};
