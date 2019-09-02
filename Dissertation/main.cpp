#include "External Libs/glad/glad.h"
#include "External Libs/glfw/include/GLFW/glfw3.h"
#include "stb_image.h"

#include "External Libs/GLM/GLM/glm/glm.hpp"
#include "External Libs/GLM/GLM/glm/gtc/matrix_transform.hpp"
#include "External Libs/GLM/GLM/glm/gtc/type_ptr.hpp"

#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "shader.h"
#include "camera.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0;
float lastY = 600.0 / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

int flag = 0;


using namespace cv;


float Virtual_Brightness = 0.8f;
float Camera_Brightness = 0.7f;

unsigned char* image;

float color[] = { 1.0f ,1.0f,1.0f,0.0f };

int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	glfwMakeContextCurrent(window);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	cv::VideoCapture cap(0);

	if (!cap.isOpened())
	{
		std::cout << "Camera not opened!" << std::endl;
		return -1;
	}
	Mat CameraFrame;
	cap >> CameraFrame;
	int width = CameraFrame.cols;
	int height = CameraFrame.rows;


	Shader shader("VertexShader.vs", "FragmentShader.fs");
	//Shader ourshader("Vertex.vs", "Fragment.fs");
	float vertices[] = {
		//     Position       TexCoord
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top left
		1.0f,   1.0f, 0.0f, 1.0f, 1.0f, // top right
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // below left
		1.0f,  -1.0f, 0.0f, 1.0f, 0.0f  // below right 
	};

	// Set up index
	unsigned int indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	Model ourModel("C:/Users/harsh/OneDrive/Desktop/Cook Torrance/Cook Torrance/wall.obj");

	Model ourModel1("C:/Users/harsh/OneDrive/Desktop/Cook Torrance/Cook Torrance/wall.obj");

	unsigned int diffuseMap = loadTexture("Green.jpg");

	unsigned int diffuseMap1 = loadTexture("Green.jpg");

	shader.use();
	shader.setInt("diffuseMap", 0);

	glm::vec3 lightPosition = glm::vec3(-10.0f, 10.0f, 10.0f);
	glm::vec3 lightColor = glm::vec3(300.0f, 300.0f, 300.0f);


	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	shader.use();
	shader.setMat4("projection", projection);

	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	cv::Mat img;
	cv::Mat Camera_HSV;
	cv::Mat Virtual_HSV;
	cv::Mat Virtual_Object;

	GLuint flag1 = 1;
	GLuint flag2 = 2;
	GLuint flag3 = 3;

	float color[] = { 1.0f, 1.0f,1.0f,1.0f };
	while (!glfwWindowShouldClose(window))
	{

		//Mat frame;
		//// Capture frame-by-frame
		cap >> CameraFrame;
		Virtual_Object = cv::imread("Green.jpg");

		cv::cvtColor(CameraFrame, Camera_HSV, COLOR_BGR2HSV);
		cv::cvtColor(Virtual_Object, Virtual_HSV, COLOR_BGR2HSV);

		vector<cv::Mat> Camera_Channel(3);
		vector<cv::Mat> Virtual_Channel(3);

		cv::split(Camera_HSV, Camera_Channel);
		cv::split(Virtual_HSV, Virtual_Channel);

		//cv::imshow("Value Channel", channel[2]);

		cv::Scalar Camera_Average, stddev;
		cv::Scalar Virtual_Average, BrainStddev;

		cv::meanStdDev(Camera_Channel[2], Camera_Average, stddev);
		cv::meanStdDev(Virtual_Channel[2], Virtual_Average, BrainStddev);


		if (true)
		{
			if (true)
			{
				Virtual_Brightness = 3 * Camera_Average[0] / 255;
			}
		}



		if (CameraFrame.data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, CameraFrame.data);
		}
		else
		{
			std::cout << "Failed to load texture." << std::endl;
		}
		glm::vec3 vHSV = glm::vec3(1.0, 1.0, Virtual_Brightness);
		glm::vec3 vHSV2 = glm::vec3(1.0, 1.0, 0.8);


		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);



		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("view", view);
		shader.setVec3("camPos", camera.Position);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(4.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(4.0f, 0.5f, -1.0f));
		model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
		shader.setVec3("lightPosition", lightPosition);
		shader.setVec3("lightColor", lightColor);
		shader.setVec3("vHSV", vHSV);
		shader.setInt("mean", Camera_Average[0]);
		shader.setInt("flag", flag1);
		shader.setMat4("model", model);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		ourModel.Draw(shader);


		shader.use();
		glm::mat4 model1 = glm::mat4(1.0f);
		model1 = glm::translate(model1, glm::vec3(7.0f, 0.5f, -1.8f));
		model1 = glm::scale(model1, glm::vec3(1.0f, 0.5f, 1.0f));
		shader.setVec3("lightPosition", lightPosition);
		shader.setVec3("lightColor", lightColor);
		shader.setInt("flag", flag2);
		shader.setMat4("model", model1);
		shader.setVec3("vHSV1", vHSV2);
		glBindTexture(GL_TEXTURE_2D, texture);
		ourModel1.Draw(shader);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	float cameraSpeed = 2.5 * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		flag = 1;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		flag = 0;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}


vector<cv::Mat> channel(3);


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

//unsigned int loadCubemap(vector<std::string> faces)
//{
//	unsigned int textureID;
//	glGenTextures(1, &textureID);
//	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
//
//	int width, height, nrComponents;
//	for (unsigned int i = 0; i < faces.size(); i++)
//	{
//		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
//		if (data)
//		{
//			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//			stbi_image_free(data);
//		}
//		else
//		{
//			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
//			stbi_image_free(data);
//		}
//	}
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//	return textureID;
//}

