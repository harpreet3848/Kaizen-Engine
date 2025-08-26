#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include "Core.h"
#include "EngineConstansts.h"
#include "OpenGl/FrameBuffer.h"
#include "OpenGl/Shader.h"
#include "OpenGl/VertexArray.h"
#include "OpenGl/ShapeGenerator.h"

class PostProcessingFX
{
public:
    PostProcessingFX();
    ~PostProcessingFX();

    Ref<FrameBuffer> Blur(Ref<FrameBuffer> screenFrameBuffer, GLint attachmentIndex);
private:
    Ref<FrameBuffer> pingFBO;
    Ref<FrameBuffer> pongFBO;

    Ref<Shader> shaderBlur;

    Ref<VertexArray> quadVertices;
};

