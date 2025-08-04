#include "Window.h"

Window& Window::GetInstance()
{
    static Window s_Instance;

    return s_Instance;
}

Window::Window(uint32_t width, uint32_t height) : m_Width(width), m_Height(height), m_Window(nullptr){}

Window::~Window()
{
    if(m_Window)
        glfwDestroyWindow(m_Window);

    glfwTerminate();
    std::cout << "Window instance destroyed." << std::endl;
}

void Window::Init()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //For Debugger
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    m_Window = glfwCreateWindow(m_Width, m_Height, "GraphicsEngine", NULL, NULL);
    if (m_Window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        //return -1;
    }
    glfwMakeContextCurrent(m_Window);
    glfwSetFramebufferSizeCallback(m_Window, framebuffer_size_callback);

    glfwSetCursorPosCallback(m_Window, GlfwCursorPosCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        //return -1;
    }
}
void Window::PollEvents() const 
{
    // poll for events(keys, mouse, etc.)
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    // spawn the back buffer with the front buffer
    glfwSwapBuffers(m_Window);
}

void Window::SetWindowCursor(bool enable) const
{
    if(enable)
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    else
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}
bool Window::IsWindowClosed() const 
{
    return glfwWindowShouldClose(m_Window);
}
void Window::CloseWindow()
{
    glfwSetWindowShouldClose(m_Window,true);
}
GLFWwindow* Window::GetWindow() const
{
    if (m_Window == nullptr)
    {
        std::cout << "GLFWwindow is uninitialized" << std::endl;
    }
    return m_Window;
}

void Window::SetCursorPosCallback(const CursorPosCallback& callback)
{
    m_CursorPosCallback = callback;
}

void Window::GlfwCursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if(Window::GetInstance().m_CursorPosCallback)
        Window::GetInstance().m_CursorPosCallback(xpos, ypos);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}