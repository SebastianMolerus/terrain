#ifndef SHADER_H
#define SHADER_H

#include <glad\glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

class Shader
{
public:
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		ifstream ifs1;
		ifstream ifs2;
		ifs1.exceptions(ifstream::failbit | ifstream::badbit);
		ifs2.exceptions(ifstream::failbit | ifstream::badbit);

		stringstream ss1;
		stringstream ss2;


		try {
			// Load Vertex Shader Code
			ifs1.open(vertexPath);

			ss1 << ifs1.rdbuf();

			m_VC = ss1.str();

			m_VertexCode = m_VC.c_str();

			ifs1.close();
			

			// Load Fragment Shader Code
			ifs2.open(fragmentPath);

			ss2 << ifs2.rdbuf();

			m_FC = ss2.str();
			m_FragmentCode = m_FC.c_str();

			ifs2.close();
		}
		catch (ifstream::failure& e)
		{
			cerr << "There was problem with loading shader file: " << e.what() << endl;
		}


		CreateShaders();
	}
	void Use() {
		glUseProgram(m_nID);
	}

	void setBool(const std::string & name, bool value) const
	{
		glUniform1i(glGetUniformLocation(m_nID, name.c_str()), (int)value);
	}

	void setInt(const std::string & name, int value) const
	{
		glUniform1i(glGetUniformLocation(m_nID, name.c_str()), value);
	}

	void setFloat(const std::string & name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_nID, name.c_str()), value);
	}

	void setMat4(const std::string & name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_nID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
	}


	GLint m_nID;
private:
	void CreateShaders() {

		int success;
		char infoLog[512];

		//==========================================================================================
		//Vertex Shader
		m_nVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(m_nVertex, 1, &m_VertexCode, NULL);
		glCompileShader(m_nVertex);

		glGetShaderiv(m_nVertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_nVertex, sizeof(infoLog), NULL, infoLog);
			cerr << "Error while compiling Vertex Shader.\n" << infoLog << endl;
		}
		//==========================================================================================
		//Fragment Shader
		m_nFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(m_nFragment, 1, &m_FragmentCode, NULL);
		glCompileShader(m_nFragment);

		glGetShaderiv(m_nFragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(m_nFragment, sizeof(infoLog), NULL, infoLog);
			cerr << "Error while compiling Fragment Shader.\n" << infoLog << endl;
		}
		//==========================================================================================
		// Create Program , Link Shaders, delete shaders
		m_nID = glCreateProgram();
		glAttachShader(m_nID, m_nVertex);
		glAttachShader(m_nID, m_nFragment);
		glLinkProgram(m_nID);

		glGetProgramiv(m_nID, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(m_nID, sizeof(infoLog), NULL, infoLog);
			std::cerr << "ERROR::PROGRAM::LINKING_PROBLEM \n" << infoLog << std::endl;
		}

		glDeleteShader(m_nVertex);
		glDeleteShader(m_nFragment);
	}
	unsigned int m_nVertex, m_nFragment;
	
	const char* m_FragmentCode;
	const char* m_VertexCode;
	string m_FC;
	string m_VC;
};


#endif