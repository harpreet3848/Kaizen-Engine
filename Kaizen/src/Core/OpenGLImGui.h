#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


class OpenGLImGui
{
public:
	static void Setup();
	static void ShutDown();
	static void Begin();
	static void Render();
	static void End();
};