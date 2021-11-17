#pragma once
#include <tuple>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../include/gl_shader.h"

//��shader��ָ��Uniform��󶨵�ĳһ�󶨵���
void UniformBlockBind(GLShader shader, const char* blockName, unsigned int bindingPoint);
//��ȡָ�����ڵ�֡����ߴ�
std::tuple<int, int> GetFramebufferSize(GLFWwindow* window);
