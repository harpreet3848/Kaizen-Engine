#pragma once

#include <iostream>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "EngineConstansts.h"
#include <functional>

class Window {
public:
	using CursorPosCallback = std::function<void(double xpos, double ypos)>;

	void SetCursorPosCallback(const CursorPosCallback& callback);

	static Window* s_Instance;
	static Window& GetInstance();

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	GLFWwindow* GetWindow() const;

	void Init();
	void ClearColor() const;
	void ClearAllBuffer() const;
	void ClearColourBuffer() const;
	void ClearDepthBuffer() const;
	void ClearStencilBuffer() const;
	void UpdateEvents() const;
	void SetWindowCursor(bool enable) const;
	bool IsWindowClosed() const;

	void CloseWindow();

private:

	Window(uint32_t width = EngineConstants::SCR_WIDTH, uint32_t height = EngineConstants::SCR_HEIGHT);
	~Window();

	static void GlfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    GLFWwindow* m_Window = nullptr;
	uint32_t m_Width;
	uint32_t m_Height;

	CursorPosCallback m_CursorPosCallback;
};
void framebuffer_size_callback(GLFWwindow* window, int width, int height);