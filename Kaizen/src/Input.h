#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "Core/Window.h"

class Input {
public:
	static bool IsKeyPressed(int keyCode) 
	{
		if (glfwGetKey(Window::GetInstance().GetWindow(), keyCode) == GLFW_PRESS)
		{
			return true;
		}
		return false;
	}
	static bool IsMouseButtonPressed(int keyCode)
	{
		if (glfwGetMouseButton(Window::GetInstance().GetWindow(), keyCode) == GLFW_PRESS)
		{
			return true;
		}
		return false;
	}
};