#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// Macro for the legacy error check, now part of the class
#define glCheckError() OpenGLDebugger::checkError(__FILE__, __LINE__)

class OpenGLDebugger
{
public:
    static void Init();
    static void checkError(const char* file, int line);
private:

    static void APIENTRY debugOutput(GLenum source,
        GLenum type,
        uint32_t id,
        GLenum severity,
        GLsizei length,
        const char* message,
        const void* userParam);
};
