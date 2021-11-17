#pragma once
#include <tuple>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/gl_shader.h"

//将shader的指定Uniform块绑定到某一绑定点上
void UniformBlockBind(GLShader shader, const char* blockName, unsigned int bindingPoint);
//获取指定窗口的帧缓冲尺寸
std::tuple<int, int> GetFramebufferSize(GLFWwindow* window);
