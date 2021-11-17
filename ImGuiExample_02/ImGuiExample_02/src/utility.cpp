#include "../include/utility.h"

//��shader��ָ��Uniform��󶨵�ĳһ�󶨵���
void UniformBlockBind(GLShader shader, const char* blockName, unsigned int bindingPoint)
{
	glUniformBlockBinding(shader.ID, glGetUniformBlockIndex(shader.ID, blockName), bindingPoint);
}

//��ȡָ�����ڵ�֡����ߴ�
std::tuple<int, int> GetFramebufferSize(GLFWwindow* window)
{
	int _width, _height;
	glfwGetFramebufferSize(window, &_width, &_height);
	return std::make_tuple(_width, _height);
}