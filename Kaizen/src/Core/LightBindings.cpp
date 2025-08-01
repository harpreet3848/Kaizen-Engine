#include "LightBindings.h"

void LightBindings::SetDirectionalLight(Ref<Shader> shader, const LightComponent& lightComponenet)
{
    shader->setVec3("dirLight.color", lightComponenet.color);
    shader->setVec3("dirLight.direction", lightComponenet.direction);
    shader->setFloat("dirLight.ambientIntensity", lightComponenet.ambientIntensity);
    shader->setFloat("dirLight.diffuseIntensity", lightComponenet.diffuseIntensity);
    shader->setFloat("dirLight.specularIntensity", lightComponenet.specularIntensity);
}
void LightBindings::SetPointLight(Ref<Shader> shader, const LightComponent& lightComponent, const size_t index)
{
    std::string base = "pointLights[" + std::to_string(index) + "].";

    // Point light
    shader->setVec3(base + "color", lightComponent.color);
    shader->setVec3(base + "position", lightComponent.position);
    shader->setFloat(base + "ambientIntensity", lightComponent.ambientIntensity);
    shader->setFloat(base + "diffuseIntensity", lightComponent.diffuseIntensity);
    shader->setFloat(base + "specularIntensity", lightComponent.specularIntensity);
    shader->setFloat(base + "constant", lightComponent.constant);
    shader->setFloat(base + "linear", lightComponent.linear);
    shader->setFloat(base + "quadratic", lightComponent.quadratic);
}
void LightBindings::SetSpotLight(Ref<Shader> shader, const LightComponent& lightComponenet, const size_t index)
{
    std::string base = "spotLight[" + std::to_string(index) + "].";
    // SpotLight
    shader->setVec3(base + "color", lightComponenet.color);
    shader->setVec3(base + "position", lightComponenet.position);
    shader->setVec3(base + "direction", lightComponenet.direction);
    shader->setFloat(base + "ambientIntensity", lightComponenet.ambientIntensity);
    shader->setFloat(base + "diffuseIntensity", lightComponenet.diffuseIntensity);
    shader->setFloat(base + "specularIntensity", lightComponenet.specularIntensity);
    shader->setFloat(base + "constant", lightComponenet.constant);
    shader->setFloat(base + "linear", lightComponenet.linear);
    shader->setFloat(base + "quadratic", lightComponenet.quadratic);
    shader->setFloat(base + "cutOff", glm::cos(glm::radians(lightComponenet.cutOff))); //12.5f
    shader->setFloat(base + "outerCutOff", glm::cos(glm::radians(lightComponenet.outerCutOff))); // 15.0f
}
