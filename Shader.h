#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <sstream>

class Shader
{
private:
	unsigned int ID;
	std::string vsSource;
	std::string fsSource;

public:
	Shader(std::string vsPath, std::string fsPath);

	unsigned int get_id();
	void use();
	static unsigned int CompileShader(unsigned int type, std::string source);
	void CreateProgram();
	void printSources();
	void setUniformMatrix4fv(const std::string name, const GLfloat* value);
	void setUniform1bool(const std::string name, const GLboolean value);

	~Shader();
};
#endif // !SHADER_H