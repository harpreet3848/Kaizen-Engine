#include "Skybox.h"

void Skybox::Init(const std::vector<std::string>& facesFilepaths)
{
    float skyCubeVertices[] =
    {
        // positions        
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    skyboxShader = std::make_unique<Shader>("Shaders/skybox_Vertex.glsl", "Shaders/skybox_fragment.glsl");

    auto skyVertexBuffer = std::make_shared<VertexBuffer>(skyCubeVertices, static_cast<unsigned int>(sizeof(skyCubeVertices)));

    const BufferLayout skyBufferlayout = {
        {ShaderDataType::Float3, "a_Position",false },
    };

    skyVertexBuffer->SetLayout(skyBufferlayout);

    skyboxVertexArray.AddVertexBuffer(skyVertexBuffer);

    skyboxVertexArray.Bind();

    cubeMap = std::make_unique<Cubemap>(facesFilepaths, true);

    skyboxShader->use();
    skyboxShader->setInt("skybox", 0);
}

void Skybox::Draw()
{


    bool isSceneCulled = false;
    if (OpenGLConfigurations::IsStateActive(RendererStates::FaceCulling))
    {
        isSceneCulled = true;
        OpenGLConfigurations::DisableFaceCulling();
    }

    skyboxShader->use();
    skyboxVertexArray.Bind();
    cubeMap->BindTexture(0);
    OpenglRenderer::DrawTriangles(36);

    if (isSceneCulled)
    {
        OpenGLConfigurations::EnableFaceCulling();
    }
}