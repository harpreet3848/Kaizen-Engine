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

    skyboxShader = std::make_unique<Shader>("shaders/skybox_Vertex.glsl", "shaders/skybox_fragment.glsl");

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

void Skybox::Draw(glm::mat4 view, glm::mat4 projection)
{
    // Render sybox
    OpenGLConfigurations::SetDepthFunction(DepthMode::LESS_EQUAL);// change depth function so depth test passes when values are equal to depth buffer's content

    bool isSceneCulled = false;
    if (OpenGLConfigurations::IsStateActive(RendererStates::FaceCulling))
    {
        isSceneCulled = true;
        OpenGLConfigurations::DisableFaceCulling();
    }

    skyboxShader->use();
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    skyboxVertexArray.Bind();
    cubeMap->BindTexture(0);
    OpenglRenderer::DrawTriangles(36);

    OpenGLConfigurations::SetDepthFunction(DepthMode::LESS);

    if (isSceneCulled)
    {
        OpenGLConfigurations::EnableFaceCulling();
    }
}