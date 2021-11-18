#include "..\include\window_manager.h"

//static
unsigned int WindowManager::cbSID = 0;
std::map<unsigned int, WindowManager*> WindowManager::cbSMap;
WindowManager* WindowManager::cbPtr = nullptr;
//---------------------------------------------------------------
//public
//---------------------------------------------------------------

//init
WindowManager::WindowManager(GLCamera& camera,
							 const unsigned int& width, const unsigned int& height,
							 const std::string& title, GLFWmonitor* monitor, GLFWwindow* share)
	:camera(&camera), scrWidth(width), scrHeight(height), windowTitle(title)
{
	//camera
	this->lastX = static_cast<float>(width) / 2.0f;
	this->lastY = static_cast<float>(height) / 2.0f;
	this->firstMouse = true;
	//timing
	this->deltaTime = 0.0f;	//time between current frame and last frame
	this->lastTime = 0.0f;

	this->window = CreateWindow(monitor, share);

	if (cbSMap.empty())
		cbID = 1;
	else
		cbID = cbSMap.size() + 1;
	cbSMap.emplace(cbID, this);
}
WindowManager::~WindowManager() { }

void WindowManager::SetCallback()
{
	WindowManager::cbSID = this->cbID;
	glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetScrollCallback(window, ScrollCallback);
}

void WindowManager::ProcessInput()
{
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		moveCamera = true;
	}	
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		moveCamera = false;
	}
		

	this->SetPerFrameTimeLogic();
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->ProcessKeyboard(CM::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->ProcessKeyboard(CM::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->ProcessKeyboard(CM::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->ProcessKeyboard(CM::RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera->ProcessKeyboard(CM::RISE, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera->ProcessKeyboard(CM::FALL, deltaTime);
}

void WindowManager::Show(unsigned int xPos, unsigned int yPos)
{
	//glfwSetWindowPos(this->window, xPos, yPos);
	glfwShowWindow(this->window);
}

void WindowManager::UpData()
{
	auto [_width, _height] = GetFramebufferSize(this->window);
	this->scrWidth = _width;
	this->scrHeight = _height;
	//std::cout << scrWidth << ", " << scrHeight << std::endl;
	glfwSwapBuffers(this->window);
	glfwPollEvents();
}

unsigned int WindowManager::GetScrWidth()const { return scrWidth; }
unsigned int WindowManager::GetScrHeight()const { return scrHeight; }
std::string WindowManager::GetWindowTitle()const { return windowTitle; }

//---------------------------------------------------------------
//private
//---------------------------------------------------------------

//init glfw
void WindowManager::GLFWInit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
#ifdef _APPLE_
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // _APPLE_
}

//create window
GLFWwindow* WindowManager::CreateWindow(GLFWmonitor* monitor, GLFWwindow* share)
{
	GLFWInit();
	GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, windowTitle.c_str(), monitor, share);
	if(window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return nullptr;
	}
	glfwMakeContextCurrent(window);

	moveCamera = false;

	//glad
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return nullptr;
	}
	return window;
}

void WindowManager::SetPerFrameTimeLogic()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastTime;
	lastTime = currentFrame;
}

//framebuffer size callback function
void WindowManager::FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	if (cbSMap.empty())
		return;
	cbPtr = cbSMap.at(cbSID);

	//glViewport(0, 0, width, height);
}

//mouse callback function
void WindowManager::MouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	if (cbSMap.empty())
		return;
	cbPtr = cbSMap.at(cbSID);

	if (cbPtr->firstMouse)
	{
		cbPtr->lastX = xPos;
		cbPtr->lastY = yPos;
		cbPtr->firstMouse = false;
	}
	float xOffset = xPos - (cbPtr->lastX);
	float yOffset = (cbPtr->lastY) - yPos;
	cbPtr->lastX = xPos;
	cbPtr->lastY = yPos;
	if(cbPtr->moveCamera == true)
		cbPtr->camera->ProcessMouseMovement(xOffset, yOffset);
}

//scroll callback function
void WindowManager::ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	if (cbSMap.empty())
		return;
	cbPtr = cbSMap.at(cbSID);

	cbPtr->camera->ProcessMouseScroll(yOffset);
}
