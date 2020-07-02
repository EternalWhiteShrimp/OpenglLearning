#pragma  once
#define GLEW_STATIC
#include <GL/glew.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;

		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();


			if (geometryPath != nullptr)
			{
				gShaderFile.open(geometryPath);
				std::stringstream gShaderStream;
				gShaderStream << gShaderFile.rdbuf();
				gShaderFile.close();
				geometryCode = gShaderStream.str();
			}
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// compile shaders
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];


		GLuint vertexId, fragmentId;
		//vertexId = glCreateShader(GL_VERTEX_SHADER);
		//glShaderSource(vertexId, 1, &vertexSourceC, NULL);
		//glCompileShader(vertexId);
		//CheckCompileErrors(vertexId, "VERTEXSHADER");
		//fragmentId = glCreateShader(GL_FRAGMENT_SHADER);
		//glShaderSource(fragmentId, 1, &fragmentSourceC, NULL);
		//glCompileShader(fragmentId);
		//CheckCompileErrors(fragmentId, "FRAGMENTSHADER");


		// vertex shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex, "VERTEX");

		//fragment shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment, "FRAGMENT");

		unsigned int geometry;
		if (geometryPath != nullptr)
		{
			const char* gShaderCode = geometryCode.c_str();
			geometry = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometry, 1, &gShaderCode, NULL);
			glCompileShader(geometry);
			checkCompileErrors(geometry, "GEOMETRY");
		}

		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		if (geometryPath != nullptr)
		{
			glAttachShader(ID, geometry);
		}
		glLinkProgram(ID);
		checkCompileErrors(ID, "PROGRAM");

		// delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		if (geometryPath != nullptr)
			glDeleteShader(geometry);
	}


	void Use()
	{
		glUseProgram(ID);
	}

	void SetBool(const std::string& Name, bool Value) const
	{
		glUniform1i(glGetUniformLocation(ID, Name.c_str()), (int)Value);
	}

	void SetInt(const std::string& Name, int Value) const
	{
		glUniform1i(glGetUniformLocation(ID, Name.c_str()), Value);
	}

	void SetFloat(const std::string& Name, float Value) const
	{
		glUniform1f(glGetUniformLocation(ID, Name.c_str()), Value);
	}

	void SetVec2(const std::string& Name, const glm::vec2& Value) const
	{
		glUniform2fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
	}

	void SetVec2(const std::string& Name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, Name.c_str()), x, y);
	}

	void SetVec3(const std::string& Name, const glm::vec3& Value) const
	{
		glUniform3fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
	}

	void SetVec3(const std::string& Name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, Name.c_str()), x, y, z);
	}

	void SetVec4(const std::string& Name, glm::vec4& Value) const
	{
		glUniform4fv(glGetUniformLocation(ID, Name.c_str()), 1, &Value[0]);
	}

	void SetVec4(const std::string& Name, float x, float y, float z, float w) const
	{
		glUniform4f(glGetUniformLocation(ID, Name.c_str()), x, y, z, w);
	}

	void SetMat2(const std::string& Name, const glm::mat2& Mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
	}

	void SetMat3(const std::string& Name, const glm::mat3& Mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
	}

	void SetMat4(const std::string& Name, const glm::mat4& Mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, Name.c_str()), 1, GL_FALSE, &Mat[0][0]);
	}

private:

	void checkCompileErrors(GLuint Shader, std::string Type)
	{
		GLint success;
		GLchar infoLog[1024];

		if (Type != "PROGRAM")
		{
			glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(Shader, 1024, NULL, infoLog);
				std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << Type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
			}

		}
		else
		{
			glGetProgramiv(Shader, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(Shader, 1024, NULL, infoLog);
				std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << Type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;

			}
		}
	}

};

