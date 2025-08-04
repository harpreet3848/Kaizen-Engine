#pragma once
#include <vector>
#include "LightBindings.h"
#include "Camera.h"
#include "OpenGl/OpenGLRenderer.h"
#include "OpenGl/ShapeGenerator.h"
#include <imgui.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class LightManager {
public:
    LightManager();
    void AddDirectionalLight(const Ref<LightComponent>& directionalLight);
    void AddPointLight(const Ref<LightComponent>& pointLight);
    void AddSpotLight(const Ref<LightComponent>& spotLight);

    Ref<LightComponent> GetDirectionalLight(size_t index);
    Ref<LightComponent> GetPointLight(size_t index);
    Ref<LightComponent> GetSpotLight(size_t index);

	void SetupLights(const Ref<Shader>& shader);

    void Render();

    void DrawLights(glm::mat4 view, glm::mat4 projection);

    void DrawImGuiControls();
    void DrawLightEditor(Ref<LightComponent> light);

private:
	std::vector<Ref<LightComponent>> m_DirectionalLights;
	std::vector<Ref<LightComponent>> m_PointLights;
	std::vector<Ref<LightComponent>> m_SpotLights;

    Ref<VertexArray> lightVAO;

    Ref<Shader> lightCubeShader;

    Ref<Shader> m_SceneShader;
};
