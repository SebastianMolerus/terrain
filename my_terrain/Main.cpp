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
#include <vector>

#define _WINDOW_WIDTH 1024
#define _WINDOW_HEIGHT 768


float fElapsed = 0.0f;
float fLastFrame = 0.0f;

float fWS = 0.0f;
float fAD = 0.0f;

const int map_width = 16;
const int map_height = 16;

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
		fWS += 1.1;
	}
	else if (glfwGetKey(window, GLFW_KEY_S))
	{
		fWS -= 1.1;
	}
	else if (glfwGetKey(window, GLFW_KEY_D))
	{
		fAD += 1.1;
	}
	else if (glfwGetKey(window, GLFW_KEY_A))
	{
		fAD -= 1.1;
	}
}

void PerlinNoise2D(int nHeight, int nWidth, float* fSeed, int nOctaves, float fBias, float* fOutput)
{

	// 1D perlin noise
	for (int x = 0; x < nWidth; x++)
		for (int y = 0; y < nHeight; y++)
		{
			float fNoise = 0.0f;
			float fScale = 1.0f;
			float fScaleAcc = 0.0f; // this variable is used to keep output values between 0.0f and 1.0f;

			for (int o = 0; o < nOctaves; o++)
			{
				int nPitch = nWidth >> o; // distance between two samples 

				int nSampleX1 = (x / nPitch) * nPitch;
				int nSampleY1 = (y / nPitch) * nPitch;

				int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
				int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

				float fBlendX = (float)(x - nSampleX1) / (float)nPitch;  // values 0 - 1.0 on X axis. How far we are on current pitch
				float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

				float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
				float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

				fNoise = (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
				fScaleAcc += fScale;
				fScale /= fBias;
			}

			fOutput[y * nWidth + x] = fNoise / fScaleAcc;
		}

}

// 2d Size
void Heights(int width, int height, int octaves, float bias, float* heights)
{
	float* fNoiseSeed2D = new float[width * height];
	srand(time(NULL));
	for (int i = 0; i < width * height; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;
	PerlinNoise2D(width, height, fNoiseSeed2D, octaves, bias, heights);
}

void SetIndicies(int width, int height, std::vector<int>& indicies )
{
	int x = 0;
	int y = width;

	int i = 0;
	while(y < width* height)
	{ 
		indicies.push_back(x);
		indicies.push_back(y);

		
		x++; y++;

		if (i == width - 1 && y != width * height -1) {
			indicies.push_back(width * height); i = 0;
		}
		else
			i++;
	}	

	indicies.pop_back();
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


	float* Noise = new float[map_width * map_height];
	Heights(map_width, map_height, 3, 2.0f, Noise);

	// GENERATE BUFFERS
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);


	glm::vec3 positions[map_width * map_height];

	for (int y = 0; y < map_height; y++)
		for (int x = 0; x < map_height; x++)
		{
			positions[y * map_width + x].x = x;
			positions[y * map_width + x].y = y;
			positions[y * map_width + x].z = Noise[y * map_width + x] * 10;
		}



	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	// Object Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	vector<int> in;
	
	SetIndicies(map_width, map_height, in);


	int size = 0;
	for (std::vector<int>::iterator p = in.begin(); p != in.end(); p++)
	{
		size += sizeof(int);
	}
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, in.data() , GL_STATIC_DRAW);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(map_height * map_width);


	myShader.Use();
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

		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f);

		//projection = glm::perspective(glm::radians(45.0f), (float)_WINDOW_WIDTH / _WINDOW_HEIGHT, 0.1f, 100.0f);
		projection = glm::ortho(0.0f, (float)_WINDOW_WIDTH, (float)_WINDOW_HEIGHT, 0.0f, 0.1f, 2000.0f);



		// VIEW	
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 300.0f);
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -50.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		view = glm::lookAt(cameraPos, cameraFront, cameraUp);
		view = glm::translate(view, glm::vec3(25.f, 75.f, -500.0f));

		// Model
		model = glm::rotate(model, glm::radians(fWS), glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(fAD), glm::vec3(0.0f, 0.0f, -1.0f));
		model = glm::scale<float>(model, glm::vec3(50.0f, 50.f, 50.f));

		myShader.setMat4("projection", projection);
		myShader.setMat4("model", model);
		myShader.setMat4("view", view);

		glDrawElements(GL_TRIANGLE_STRIP, in.size(), GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}