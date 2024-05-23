#include "Shader.h"
#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(std::string vsPath, std::string fsPath) {

	std::ifstream vsFile, fsFile;
	std::stringstream vsSourceStream, fsSourceStream; // can't directly copy the contents of the file into a string (can't do vsSource << vsFile.rdbuf());

	vsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fsFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		// open files
		vsFile.open(vsPath);
		fsFile.open(fsPath);
		// read file's buffer contents into streams
		vsSourceStream << vsFile.rdbuf();
		fsSourceStream << fsFile.rdbuf();
		// close file handlers
		vsFile.close();
		fsFile.close();
		// convert stream into string
		vsSource = vsSourceStream.str();
		fsSource = fsSourceStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
	}
}

unsigned int Shader::get_id() {
	return ID;
}

void Shader::printSources() {
	std::cout << "vs source: " << vsSource << std::endl;
	std::cout << "fs source: " << fsSource << std::endl;
}

void Shader::use() {
	glUseProgram(ID);
}

unsigned int Shader::CompileShader(unsigned int type, std::string source) {
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str(); //c_str return pointer to first char in the string, just like &source[0]
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int success;
	char infoLog[512];

	glGetShaderiv(id, GL_COMPILE_STATUS, &success);

	if (success == GL_FALSE) {
		glGetShaderInfoLog(id, 512, nullptr, infoLog);
		std::cout << "COMPILATION FAILED\n" << infoLog << std::endl;
	}

	return id;
}

void Shader::CreateProgram() {
	unsigned int vs, fs;

	vs = CompileShader(GL_VERTEX_SHADER, vsSource);
	fs = CompileShader(GL_FRAGMENT_SHADER, fsSource);

	ID = glCreateProgram();
	glAttachShader(ID, vs);
	glAttachShader(ID, fs);

	glLinkProgram(ID);

	int success;
	char infoLog[512];

	//glValidateProgram(program); used as an ulterior check of the execution of the program, used with glGetProgram (similar to glGetProgramiv ecc.)
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE) {
		glGetProgramInfoLog(ID, 512, nullptr, infoLog);
		std::cout << "LINKING FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

void Shader::setUniformMatrix4fv(const std::string name, const GLfloat* value) {

	int location;

	location = glGetUniformLocation(ID, name.c_str()); // oppure &name[0]
	glUniformMatrix4fv(location, 1, GL_FALSE, value);
}

void Shader::setUniform1bool(const std::string name, const GLboolean value) {

	int location = glGetUniformLocation(ID, name.c_str());
	glUniform1i(location, value);
}

Shader::~Shader() {
	glDeleteProgram(ID);
}