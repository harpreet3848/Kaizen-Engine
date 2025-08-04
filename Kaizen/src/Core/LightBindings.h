#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "OpenGl/Shader.h"
#include "Core.h"

enum class LightType {
	Directional,
	Point,
	Spot
};

struct LightComponent 
{    
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;

    float constant;
    float linear;
    float quadratic;

    float cutOff;       // Inner cone angle (in degrees)
    float outerCutOff;  // Outer cone angle (in degrees)

    LightType type;

    glm::vec3 color;

    glm::vec3 position;

    glm::vec3 direction;
};

class LightBindings 
{
public:
    static void SetDirectionalLight(Ref<Shader>& shader, const  Ref<LightComponent>& lightComponenet);

    static void SetPointLight(Ref<Shader>& shader, const  Ref<LightComponent>& lightComponenet, const size_t index);

    static void SetSpotLight(Ref<Shader>& shader, const  Ref<LightComponent>& lightComponenet, const size_t index);
};