#pragma once

#include "OpenGl/VertexArray.h"

#include "OpenGl/VertexBuffer.h"
#include "OpenGl/Shader.h"
#include "OpenGl/Cubemap.h"
#include "OpenGl/OpenGLConfigurations.h"
#include "OpenGl/OpenGLRenderer.h"

class Skybox 
{
public:
    void Init(const std::vector<std::string>& facesFilepaths);

    void Draw();

private:
    VertexArray skyboxVertexArray;
    std::unique_ptr<Shader> skyboxShader;
    std::unique_ptr<Cubemap> cubeMap;
};
