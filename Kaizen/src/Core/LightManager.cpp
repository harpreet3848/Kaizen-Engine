#include "LightManager.h"

LightManager::LightManager()
{
    m_DirectionalLights.clear();
    m_PointLights.clear();
    m_SpotLights.clear();

    lightVAO = ShapeGenerator::GenerateCube();

    lightCubeShader = std::make_shared<Shader>("shaders/lights_Vertex_Shader.glsl", "shaders/lightColor_Fragment.glsl");
}

void LightManager::AddDirectionalLight(const Ref<LightComponent>& directionalLight)
{
    m_DirectionalLights.emplace_back(directionalLight);
}

void LightManager::AddPointLight(const Ref<LightComponent>& pointLight)
{
    m_PointLights.emplace_back(pointLight);
}
 
void LightManager::AddSpotLight(const Ref<LightComponent>& spotLight)
{
    m_SpotLights.emplace_back(spotLight);
}

Ref<LightComponent> LightManager::GetDirectionalLight(size_t index)
{
    return m_DirectionalLights[index];
}

Ref<LightComponent> LightManager::GetPointLight(size_t index)
{
    return m_PointLights[index];
}

Ref<LightComponent> LightManager::GetSpotLight(size_t index)
{
    return m_SpotLights[index];
}

void LightManager::SetupLights(const Ref<Shader>& shader) { // call it after adding lights

    m_SceneShader = shader;
    m_SceneShader->use();
    m_SceneShader->setInt("pointLightCount", static_cast<int>(m_PointLights.size()));
    m_SceneShader->setInt("spotLightCount", static_cast<int>(m_SpotLights.size()));
}

void LightManager::Render()
{
    m_SceneShader->use();

    if (!m_DirectionalLights.empty())
    {
        LightBindings::SetDirectionalLight(m_SceneShader, m_DirectionalLights[0]);
    }

    for (size_t i = 0; i < m_PointLights.size(); i++)
    {
        LightBindings::SetPointLight(m_SceneShader, m_PointLights[i], i);
    }

    for (size_t i = 0; i < m_SpotLights.size(); i++)
    {

        LightBindings::SetSpotLight(m_SceneShader, m_SpotLights[i], i);
    }
    DrawImGuiControls();
}

void LightManager::DrawLights()
{
    // draw light cubes
    lightCubeShader->use();

    lightVAO->Bind();
    glm::mat4 model = glm::mat4(1.0f);

    for (unsigned int i = 0; i < m_PointLights.size(); i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, m_PointLights[i]->position);
        model = glm::scale(model, glm::vec3(0.1f));
        lightCubeShader->setVec3("lightColor", m_PointLights[i]->color);
        lightCubeShader->setMat4("model", model);
        OpenglRenderer::DrawIndexed(lightVAO);
    }
}

void LightManager::DrawImGuiControls() {
    ImGui::Begin("Light Manager");

    int id_counter = 0;

    if (!m_DirectionalLights.empty()) {
        if (ImGui::CollapsingHeader("Directional Light")) {
            ImGui::PushID(id_counter++);
            DrawLightEditor(m_DirectionalLights[0]);
            ImGui::PopID();
        }
    }

    if (ImGui::CollapsingHeader("Point Lights")) {
        for (int i = 0; i < m_PointLights.size(); ++i) {
            ImGui::PushID(id_counter++);
            std::string label = "Point Light " + std::to_string(i);
            if (ImGui::TreeNode(label.c_str())) {
                DrawLightEditor(m_PointLights[i]);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }

    if (ImGui::CollapsingHeader("Spot Lights")) {
        for (int i = 0; i < m_SpotLights.size(); ++i) {
            ImGui::PushID(id_counter++);
            std::string label = "Spot Light " + std::to_string(i);
            if (ImGui::TreeNode(label.c_str())) {
                DrawLightEditor(m_SpotLights[i]);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
    }

    ImGui::End();
}

void LightManager::DrawLightEditor(Ref<LightComponent> light) {

    ImGui::ColorEdit3("Color", &light->color[0]);

    ImGui::DragFloat("Ambient", &light->ambientIntensity, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Diffuse", &light->diffuseIntensity, 0.01f, 0.0f, 1.0f);
    ImGui::DragFloat("Specular", &light->specularIntensity, 0.01f, 0.0f, 1.0f);

    ImGui::Separator(); // Adds a visual line

    if (light->type == LightType::Point || light->type == LightType::Spot) {
        ImGui::DragFloat3("Position", &light->position[0], 0.1f);
    }

    if (light->type == LightType::Directional || light->type == LightType::Spot) {

        ImGui::SliderFloat3("Direction", &light->direction[0], -1.0f, 1.0f);
        if (ImGui::Button("Normalize Direction")) {
            light->direction = glm::normalize(light->direction);
        }
    }

    if (light->type == LightType::Point || light->type == LightType::Spot) {
        ImGui::Text("Attenuation");
        ImGui::DragFloat("Constant", &light->constant, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Linear", &light->linear, 0.001f, 0.0f, 1.0f);
        ImGui::DragFloat("Quadratic", &light->quadratic, 0.0001f, 0.0f, 1.0f);
    }

    if (light->type == LightType::Spot) {
        ImGui::Text("Spotlight Cones");
        ImGui::SliderFloat("Inner CutOff", &light->cutOff, 0.0f, light->outerCutOff);
        ImGui::SliderFloat("Outer CutOff", &light->outerCutOff, light->cutOff, 90.0f);
    }
}
