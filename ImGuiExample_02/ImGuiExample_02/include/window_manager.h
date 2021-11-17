#pragma once
#include <map>
#include <string>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gl_camera.h"
#include "utility.h"
class WindowManager
{
public:
	GLFWwindow* window;

	WindowManager(GLCamera& camera, 
				  const unsigned int& width, const unsigned int& height,
				  const std::string& title, GLFWmonitor* monitor, GLFWwindow* share);
	~WindowManager();

	void SetCallback();
	void ProcessInput();
	void Show(unsigned int xPos, unsigned int yPos);
	void UpData();

	unsigned int GetScrWidth() const;
	unsigned int GetScrHeight() const;
	std::string GetWindowTitle() const;

private:
	GLCamera* camera;
	unsigned int scrWidth;
	unsigned int scrHeight;
	std::string windowTitle;

	//camera
	float lastX;
	float lastY;
	bool firstMouse;
	bool moveCamera;

	//timing
	float deltaTime;	//time between current frame and last frame
	float lastTime;

	void GLFWInit();
	GLFWwindow* CreateWindow(GLFWmonitor* monitor, GLFWwindow* share);
	void SetPerFrameTimeLogic();

	//callback func
	//-------------
	//当存在多个类实例对象时，管理cbPtr
	unsigned int cbID;			//本对象ID
	static unsigned int cbSID;	//静态ID，通过cbSID = cbID来访问指定对象非静态成员
	static std::map<unsigned int, WindowManager*> cbSMap;	//静态对象映射表

	//call back pointer
	static WindowManager* cbPtr;
	//framebuffer size callback function
	static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
	//mouse callback function
	static void MouseCallback(GLFWwindow* window, double xPos, double yPos);
	//scroll callback function
	static void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
};

