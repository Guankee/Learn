#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath> 
#include "shader.h"
#include "stb_image.h"
#include "camera.h"
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include "model.h"


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
void mouseClick_callback(GLFWwindow* window, int button, int action, int mods);
glm::vec3 worldPosFromViewPort(int winX, int winY);
// settings
 unsigned int SCR_WIDTH = 800;
 unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float _near = 0.1f, _far = 100.0f;
glm::mat4 projection;
glm::mat4 view;
bool LeftButtonHolding = false;
bool RightButtonHolding = false;
struct ModelInfo {
	Model* model;
	glm::vec3 worldPos;
	float pitch;
	float yaw;
	float roll;
	bool isSelected;
	string name;
};
map<string, ModelInfo> _Models;

int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	// --------------------
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouseClick_callback);
	glfwSetScrollCallback(window, scroll_callback);


	// tell GLFW to capture our mouse
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS); // always pass the depth test (same effect as glDisable(GL_DEPTH_TEST))

	// build and compile shaders
	// -------------------------
	Shader shader("../../shaders/shader.vs", "../../shaders/shader.fs");
	Shader shaderSingleColor("../../shaders/shader.vs", "../../shaders/shaderSingleColor.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	// load textures
	// -------------
	unsigned int floorTexture = loadTexture("../../../pics/rock.jpg");

	Model Zhang3 = Model("../../../pics/backpack/backpack.obj");
	_Models["zhang3"] = ModelInfo{ &Zhang3,glm::vec3(-3,0,0),0.0,0.0,0.0,false,"Zhang3" };
	Model Li4 = Model("../../../pics/backpack/backpack.obj");
	_Models["Li4"] = ModelInfo{ &Li4,glm::vec3(3,0,0),0.0,0.0,0.0,false,"Li4" };

	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		shaderSingleColor.use();
		shaderSingleColor.setMat4("view", view);
		shaderSingleColor.setMat4("projection", projection);

		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
		// model
		for (map<string, ModelInfo>::iterator _modelsIter = _Models.begin(); _modelsIter != _Models.end(); _modelsIter++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, _modelsIter->second.worldPos);
			model = glm::rotate(model, _modelsIter->second.pitch, glm::vec3(1.0f, 0.0f, 0.0f));
			model = glm::rotate(model, _modelsIter->second.yaw, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, _modelsIter->second.roll, glm::vec3(0.0f, 0.0f, 1.0f));
			shader.use();
			shader.setMat4("model", model);
			_modelsIter->second.model->Draw(shader);
			if(_modelsIter->second.isSelected)
			{
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);
				model = glm::scale(model, glm::vec3(1.1f));
				shaderSingleColor.use();
				shaderSingleColor.setMat4("model", model);
				_modelsIter->second.model->Draw(shaderSingleColor);	
			}
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
		}

		// floor
		glBindVertexArray(planeVAO);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		shader.use();
		shader.setMat4("model", glm::mat4(1.0f));
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	SCR_WIDTH=width;
	SCR_HEIGHT=height;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	if (LeftButtonHolding) {
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}

	if (RightButtonHolding) {
		for (map<string, ModelInfo>::iterator _modelsIter = _Models.begin(); _modelsIter != _Models.end(); _modelsIter++)
		{
			if (_modelsIter->second.isSelected) {
				_modelsIter->second.worldPos = worldPosFromViewPort((int)xposIn,(int)yposIn);
				break;
			}
		}
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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

void mouseClick_callback(GLFWwindow* window, int button, int action, int mods) {
	double winX, winY;
	glfwGetCursorPos(window, &winX, &winY);

	if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_LEFT))
	{
		lastX = (float)winX;
		lastY = (float)winY;
		LeftButtonHolding = true; //只有按下鼠标右键时，才会改变摄像机角度
	}
	else
		LeftButtonHolding = false;

	if ((action == GLFW_PRESS) && (button == GLFW_MOUSE_BUTTON_RIGHT)) {
		RightButtonHolding = true;
		glm::vec3 wolrdPostion = worldPosFromViewPort((int)winX,(int)winY);

		for (map<string, ModelInfo>::iterator _modelsIter = _Models.begin(); _modelsIter != _Models.end(); _modelsIter++)
		{
			float _distance = glm::distance(_modelsIter->second.worldPos, glm::vec3(wolrdPostion));
			if (_distance < 2.5) {
				cout << _modelsIter->first << "模型被选中..." << endl;
				_modelsIter->second.isSelected = true;
				//break;
			}
			else
				_modelsIter->second.isSelected = false;
		}
	}
	else
		RightButtonHolding = false;
}

glm::vec3 worldPosFromViewPort(int winX, int winY) {
	float winZ;
	glReadPixels(
		winX,
		(int)SCR_HEIGHT - winY
		, 1, 1
		, GL_DEPTH_COMPONENT, GL_FLOAT
		, &winZ);

	float x = (2.0f * (float)winX) / SCR_WIDTH - 1.0f;
	float y = 1.0f - (2.0f * (float)winY) / SCR_HEIGHT;
	float z = winZ * 2.0f - 1.0f;
	//有像素被点中 

	//float w = (2.0 * _near * _far) / (_far + _near - z * (_far - _near));
	float w = _near * _far / (_near * winZ - _far * winZ + _far);
	glm::vec4 wolrdPostion(x, y, z, 1);
	wolrdPostion *= w;
	wolrdPostion = glm::inverse(view) * glm::inverse(projection) * wolrdPostion;
	return wolrdPostion;
}