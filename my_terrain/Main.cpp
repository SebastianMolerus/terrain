#include <glad/glad.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>     /* srand, rand */
#include <time.h>  
#include <sstream>
#include "Shader.h"


#define _WINDOW_WIDTH 1024
#define _WINDOW_HEIGHT 768

float fElapsed = 0.0f;
float fLastFrame = 0.0f;

using namespace std;

void InitializeGLFW() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
// resize callback
void resize_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
// exit program
void escape(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, TRUE);
}
// fps title
void setfpstitle(GLFWwindow* window, string title, float fElapsed)
{
	stringstream ss;
	ss << title << ". ";
	ss << (int)(1.0f / fElapsed);	
	ss << " FPS.";
	glfwSetWindowTitle(window, ss.str().c_str());
}
// optional : max atribs support
void howmanyatribs()
{
	int nAttribsCount;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nAttribsCount);
	cout << "Max supported attribs: " << nAttribsCount << endl;
}

int main()
{	
	InitializeGLFW();

	GLFWwindow* window = glfwCreateWindow(_WINDOW_WIDTH, _WINDOW_HEIGHT, "Ruslemo terrain", NULL, NULL);
	if (!window)
	{
		cerr << "Failed to create window" << endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cerr << "Failet to initialize GLAD" << endl;
		return -1;
	}

	// resize callback
	glfwSetFramebufferSizeCallback(window, resize_callback);

	// Z-BUFFER
	//glEnable(GL_DEPTH_TEST);

	//=========================================================================================

	Shader myShader("vertex.txt", "fragment.txt");
	

	float verticles[]{
		 -0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticles), verticles, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	//=========================================================================================
	// MAIN GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		// check exit state
		escape(window);

		glClearColor(0.754f, 0.754f, 0.753f, 1.0f); 
		glClear(GL_COLOR_BUFFER_BIT); 
		myShader.Use();

		// set fps title
		float fCurrentFrame = (float)glfwGetTime();
		fElapsed = fCurrentFrame - fLastFrame;
		fLastFrame = fCurrentFrame;
		setfpstitle(window, "Ruslemo terrain", fElapsed);



		
		//glUniform3fv(glGetUniformLocation(myShader.m_nID, "Color"), sizeof(color), glm::value_ptr(color));
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}