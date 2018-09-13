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

float fWS = 0.0f;
float fAD = 0.0f;

float Zverticles[8 * 8];



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

void initialize_Zverticles()
{
	float* fNoiseSeed2D = new float[8 * 8];
	srand(time(NULL));
	for (int i = 0; i < 8 * 8; i++) fNoiseSeed2D[i] = (float)rand() / (float)RAND_MAX;


	PerlinNoise2D(8, 8, fNoiseSeed2D, 4, 1.0f, Zverticles);
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


	initialize_Zverticles();

	// GENERATE BUFFERS
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);


	glm::vec3 positions[16];

	for (int y = 0; y < 4; y++)
		for (int x = 0; x < 4; x++)
		{
			positions[y * 4 + x].x = x;
			positions[y * 4 + x].y = -y;
			positions[y * 4 + x].z = Zverticles[y * 4 + x];
		}



	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);

	// Object Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);



	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	int iIndices[] =
	{
		0, 4, 1, 5, 2, 6, 3, 7, 16, // First row, then restart
		4, 8, 5, 9, 6, 10, 7, 11, 16, // Second row, then restart
		8, 12, 9, 13, 10, 14, 11, 15, // Third row, no restart
	};
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iIndices), iIndices, GL_STATIC_DRAW);
	glEnable(GL_PRIMITIVE_RESTART);
	glPrimitiveRestartIndex(16);


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

		projection = glm::perspective(glm::radians(45.0f), (float)_WINDOW_WIDTH / _WINDOW_HEIGHT, 0.1f, 100.0f);
		/*projection = glm::ortho(0.0f, (float)_WINDOW_WIDTH, (float)_WINDOW_HEIGHT, 0.0f, 0.1f, 1000.0f);
		glm::vec3 cameraPos = glm::vec3(0, 0, 3.0f);
		glm::vec3 cameraFront = glm::vec3(0, 0, -1.0f);
		glm::vec3 cameraUp = glm::vec3(0, 1.0f, 0.0f);*/




		// VIEW
		//view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		//view = glm::translate<float>(view, glm::vec3(512.0f, 384.0f, -300.0f));
		//view = glm::scale<float>(view, glm::vec3(100, 100, 100));


		myShader.setMat4("projection", projection);
		myShader.setMat4("view", view);


		glm::mat4 model = glm::mat4(1.0f);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
		model = glm::rotate(model, glm::radians(fWS), glm::vec3(-1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(fAD), glm::vec3(0.0f, 0.0f, -1.0f));

		myShader.setMat4("model", model);


		glDrawElements(GL_TRIANGLE_STRIP, 4 * (4 - 1) * 2 + 4 - 2, GL_UNSIGNED_INT, 0);








		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	return 0;
}