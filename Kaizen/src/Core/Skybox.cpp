#include "Skybox.h"
#include <imgui.h>

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
    DrawImGui();

    bool isSceneCulled = false;
    if (OpenGLConfigurations::IsStateActive(RendererStates::FaceCulling))
    {
        isSceneCulled = true;
        OpenGLConfigurations::DisableFaceCulling();
    }

    skyboxShader->use();
    skyboxShader->setFloat("skyExposureIntensity",m_Exposure);
    skyboxVertexArray.Bind();
    cubeMap->BindTexture(0);
    OpenglRenderer::DrawTriangles(36);

    if (isSceneCulled)
    {
        OpenGLConfigurations::EnableFaceCulling();
    }
}

void Skybox::DrawImGui()
{
    if (ImGui::Begin("Environment")) {
        ImGui::Separator();

        // Wide range with nice feel:
        ImGui::SliderFloat("Sky Exposure", &m_Exposure, 0.0f, 10.0f, "%.3f", ImGuiSliderFlags_Logarithmic);

        // Optional quick buttons
        if (ImGui::Button("Reset")) m_Exposure = 1.0f;
        ImGui::SameLine();
        if (ImGui::Button("Half"))  m_Exposure *= 0.5f;
        ImGui::SameLine();
        if (ImGui::Button("Double")) m_Exposure *= 2.0f;
    }
    ImGui::End();
}