#version 460 core

// Define max textures per type
#define MAX_DIFFUSE_MAPS 4
#define MAX_SPECULAR_MAPS 4
#define MAX_EMISSION_MAPS 4


struct Material {
    // Add uniforms to hold the count of active textures
    int active_diffuse_maps;
    int active_specular_maps;
    int active_emission_maps;

    float shininess;

    sampler2D texture_diffuse[MAX_DIFFUSE_MAPS];
    sampler2D texture_specular[MAX_SPECULAR_MAPS];
    sampler2D texture_emission[MAX_EMISSION_MAPS];
};


struct DirLight 
{
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;

    vec3 direction;
    vec3 color;
};

struct PointLight 
{
    float constant;
    float linear;
    float quadratic;
	
    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;

    vec3 color;
    vec3 position;
};

struct SpotLight {
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;

    float ambientIntensity;
    float diffuseIntensity;
    float specularIntensity;       
      
    vec3 color;

    vec3 position;
    vec3 direction;

};

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec3 LightPos; 
in vec2 TexCoords;
in vec4 FragPosLightSpace;

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

// These tell the shader how many lights in the array are actually active
uniform int pointLightCount;
uniform int spotLightCount;

uniform vec3 viewPos; 
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];

uniform sampler2D shadowMap;
uniform Material material;


uniform vec3 lightPos; 


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalDiffuseColor();
vec3 CalSpecularColor();


float ShadowCalculation(vec4 fragPosLightSpace)
{
       // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);


    float slopeBias  = 0.05 * (1.0 - dot(normal, lightDir));
    float constBias  = 0.005;

    float bias = gl_FrontFacing ? max(slopeBias, constBias) : constBias * 0.5;

    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
    if (material.active_diffuse_maps > 0 &&
        texture(material.texture_diffuse[0], TexCoords).a < 0.1) discard;
  
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos); 	
    
    vec3 result = vec3(0.0);
    // phase 1: Directional light
    result = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: point lights
    for(int i = 0; i < pointLightCount; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    

    // phase 3: spot light
    for(int i = 0; i < spotLightCount; i++)
        result += CalcSpotLight(spotLight[i], norm, FragPos, viewDir);
        
    vec3 emission = vec3(0.0);
    if (material.active_emission_maps > 0)
        emission = texture(material.texture_emission[0], TexCoords).rgb;

    FragColor = vec4(result + emission, 1.0);
} 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayVec = normalize(viewDir + lightDir);   // Bling-Phong

    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);

    // combine results
    vec3 ambientComp = light.ambientIntensity * CalDiffuseColor();
    vec3 diffuseComp = light.diffuseIntensity * diff * CalDiffuseColor();
    vec3 specularComp = light.specularIntensity * spec * CalSpecularColor();

    float shadow = ShadowCalculation(FragPosLightSpace);                      
    return (ambientComp + (1.0 - shadow) * (diffuseComp + specularComp)) * light.color;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayVec = normalize(viewDir + lightDir);   // Bling-Phong
    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // combine results
    vec3 ambientComp = light.color * light.ambientIntensity * CalDiffuseColor();
    vec3 diffuseComp = light.color * light.diffuseIntensity * diff * CalDiffuseColor();
    vec3 specularComp = light.color * light.specularIntensity * spec * CalSpecularColor();

    return (ambientComp + diffuseComp + specularComp) * attenuation;
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayVec = normalize(viewDir + lightDir); // Blinn-Phong
    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambientComp = light.color * light.ambientIntensity * CalDiffuseColor();
    vec3 diffuseComp = light.color * light.diffuseIntensity * diff * CalDiffuseColor();
    vec3 specularComp = light.color * light.specularIntensity * spec * CalSpecularColor();

    ambientComp *= attenuation * intensity;
    diffuseComp *= attenuation * intensity;
    specularComp *= attenuation * intensity;

    return (ambientComp + diffuseComp + specularComp);
}

vec3 CalDiffuseColor()
{
    vec3 diffuseColor = vec3(1.0); 
    for(int i = 0; i < material.active_diffuse_maps; i++)
    {
        diffuseColor *= texture(material.texture_diffuse[i], TexCoords).rgb;
    }
    return diffuseColor;
}
vec3 CalSpecularColor()
{
    vec3 specularColor = vec3(1.0);
    
    for(int i = 0; i < material.active_specular_maps; i++)
    {
        specularColor *= texture(material.texture_specular[i], TexCoords).rgb;
    }
    return specularColor;
}