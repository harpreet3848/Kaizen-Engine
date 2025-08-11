#include "OpenGLDebugger.h"
#include <string>

void OpenGLDebugger::Init()
{
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        std::cout << "OpenGL Debug Context enabled. Initializing callback." << std::endl;
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // Ensures callbacks are called immediately after an error
        glDebugMessageCallback(debugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
    else
    {
        std::cout << "OpenGL Debug Context not available." << std::endl;
    }
}

void OpenGLDebugger::checkError(const char* file, int line)
{
    GLenum errorCode;
    while ((errorCode = glGetError()) != GL_NO_ERROR)
    {
        std::string error;
        switch (errorCode)
        {
        case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        default:                               error = "UNKNOWN_ERROR"; break;
        }
        std::cerr << "Legacy OpenGL Error: " << error << " | " << file << " (" << line << ")" << std::endl;
    }
}

void APIENTRY OpenGLDebugger::debugOutput(GLenum source, GLenum type, uint32_t id,
    GLenum severity, GLsizei length,
    const char* message, const void* userParam)
{
    // Ignore non-significant error codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "--------------------------------" << std::endl;
    std::cout << "Debug Message (" << id << "): " << message << std::endl;

    switch (source)
    {
    case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
    case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
    case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
    case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    }
    std::cout << std::endl;

    switch (type)
    {
    case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break;
    case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
    case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
    case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
    default:                                std::cout << "Type: Other"; break;
    }
    std::cout << std::endl;

    switch (severity)
    {
    case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: High"; break;
    case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: Medium"; break;
    case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: Low"; break;
    case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: Notification"; break;
    }
    std::cout << std::endl;
   if(type == GL_DEBUG_SOURCE_OTHER)
    {
#ifdef _MSC_VER

        __debugbreak();
#endif
    }
    if (severity == GL_DEBUG_SEVERITY_HIGH) {
#ifdef _MSC_VER
        __debugbreak(); // Halts the program in the debugger if on MSVC
#endif
    }
}
