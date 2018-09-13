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
#include "stb_image.h"


#define _WINDOW_WIDTH 1024
#define _WINDOW_HEIGHT 768


float fElapsed = 0.0f;
float fLastFrame = 0.0f;

float fTest = 0.0f;

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

void process_input(GLFWwindow* window) {

	if (glfwGetKey(window, GLFW_KEY_W))
	{
		fTest += 0.01;
	}
	else if (glfwGetKey(window, GLFW_KEY_S))
	{
		fTest -= 0.01;
	}
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
	glEnable(GL_DEPTH_TEST);

	//=========================================================================================

	Shader myShader("vertex.txt", "fragment.txt");


	float verticles[] = {
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticles), verticles, GL_STATIC_DRAW);

	// Object Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture positions
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);


	//=========================================================================================
	// TEXTURES

	unsigned int grass_texture;
	glGenTextures(1, &grass_texture);

	glBindTexture(GL_TEXTURE_2D, grass_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("grass.jpg", &width, &height, &nrChannels, STBI_rgb_alpha);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cerr << "Failed to load texture " << endl;
	}
	stbi_image_free(data);


	glActiveTexture(GL_TEXTURE0);
	myShader.Use();
	myShader.setInt("texture1", 0); // connect sampler in fragment shader with active texture ( GL_TEXTURE0 );

	//=========================================================================================
	// MAIN GAME LOOP
	while (!glfwWindowShouldClose(window))
	{
		// check exit state
		escape(window);

		process_input(window);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		// set fps title
		float fCurrentFrame = (float)glfwGetTime();
		fElapsed = fCurrentFrame - fLastFrame;
		fLastFrame = fCurrentFrame;
		setfpstitle(window, "Ruslemo terrain", fElapsed);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		//projection = glm::perspective(glm::radians(45.0f), (float)_WINDOW_WIDTH / _WINDOW_HEIGHT, 0.1f, 100.0f);
		projection = glm::ortho(0.0f, (float)_WINDOW_WIDTH, (float)_WINDOW_HEIGHT, 0.0f, -1000.0f, 1000.0f);
		glm::vec3 cameraPos   = glm::vec3(0, 0, 3.0f );
		glm::vec3 cameraFront = glm::vec3(0, 0, -1.0f);
		glm::vec3 cameraUp    = glm::vec3(0, 1.0f, 0.0f);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		model = glm::rotate(model, glm::radians(fTest), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate<float>(model, glm::vec3(300.0f, 300.0f, 300.0f));
		model = glm::scale<float>(model, glm::vec3(100, 100, 100));
	

		myShader.setMat4("model", model);
		myShader.setMat4("view", view);
		myShader.setMat4("projection", projection);
		
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}