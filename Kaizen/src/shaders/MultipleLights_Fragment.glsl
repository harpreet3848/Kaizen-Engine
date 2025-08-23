#version 460 core

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct Material {
    //holds the count of active textures
    int active_diffuse_maps;
    int active_specular_maps;
    int active_emission_maps;

    float shininess;

    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_emission;
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

in vec2 TexCoords;
in vec4 FragPosDirLightSpace;
in vec4 FragPosSpotLightSpace;

// These tell the shader how many lights in the array are actually active
uniform int activePointLightCount;
uniform int activeSpotLightCount;

uniform float pointLightFarPlane;
uniform vec3 viewPos; 
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];

layout(binding = 3) uniform sampler2D directionalShadowMap;
layout(binding = 4) uniform sampler2D spotShadowMap[MAX_SPOT_LIGHTS];
layout(binding = 8) uniform samplerCube pointShadowMap[MAX_POINT_LIGHTS];

uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index);

vec3 CalDiffuseColor();
vec3 CalSpecularColor();

void main()
{
    if (material.active_diffuse_maps > 0 &&
        texture(material.texture_diffuse, TexCoords).a < 0.1) discard;
  
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos); 	
    
    vec3 result = vec3(0.0);

    // phase 1: Directional light
    result = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: point lights
    for(int i = 0; i < activePointLightCount; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir, i);    

    // phase 3: spot light
    for(int i = 0; i < activeSpotLightCount; i++)
        result += CalcSpotLight(spotLight[i], norm, FragPos, viewDir, i);
        
    vec3 emission = vec3(0.0);
    if (material.active_emission_maps > 0)
        emission = texture(material.texture_emission, TexCoords).rgb;

    FragColor = vec4(result + emission, 1.0);
} 

//--------------------------------------------------------------------
//-------------------------Dir Light----------------------------------
//---------------------------------------------------------------------

float DirShadowCalculation(vec4 fragPosLightSpace)
{
       // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(-dirLight.direction);

    float slopeBias  = 0.002 * (1.0 - max(dot(normal, lightDir),0.0));
    float constBias  = 0.0002;

    float bias = constBias + slopeBias;

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(directionalShadowMap, 0);

    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(directionalShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayVec = normalize(viewDir + lightDir);   // Blinn-Phong

    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);

    // combine results
    vec3 ambientComp = light.ambientIntensity * CalDiffuseColor();
    vec3 diffuseComp = light.diffuseIntensity * diff * CalDiffuseColor();
    vec3 specularComp = light.specularIntensity * spec * CalSpecularColor();

    float shadow = DirShadowCalculation(FragPosDirLightSpace);                      
    return (ambientComp + (1.0 - shadow) * (diffuseComp + specularComp)) * light.color;
}

//--------------------------------------------------------------------
//-------------------------Point Light----------------------------------
//---------------------------------------------------------------------

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowPointCalculation(vec3 fragPos,vec3 lightPosition , int index)
{
    vec3 fragToLight = fragPos - lightPosition;

    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / pointLightFarPlane)) / 200.0;
    //float diskRadius = 0.01;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(pointShadowMap[index], fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= pointLightFarPlane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);      
    return shadow;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayVec = normalize(viewDir + lightDir);   // Blinn-Phong
    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // combine results
    vec3 ambientComp = light.ambientIntensity * CalDiffuseColor();
    vec3 diffuseComp = light.diffuseIntensity * diff * CalDiffuseColor();
    vec3 specularComp = light.specularIntensity * spec * CalSpecularColor();

    float shadow = ShadowPointCalculation(fragPos,light.position,index);

    return (ambientComp + (1.0  - shadow) * (diffuseComp + specularComp)) * attenuation * light.color;
}

//--------------------------------------------------------------------
//-------------------------Spot Light----------------------------------
//---------------------------------------------------------------------

float SpotShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int index)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;
    
    float slopeBias  = 0.002 * (1.0 - max(dot(normal, lightDir),0.0));
    float constBias  = 0.0002;

    float bias = constBias + slopeBias;


    vec2 texelSize = 1.0 / vec2(textureSize(spotShadowMap[index], 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; ++x)
    {
       for (int y = -1; y <= 1; ++y) 
       {
           float pcfDepth = texture(spotShadowMap[index], projCoords.xy + vec2(x, y) * texelSize).r;
           shadow += (projCoords.z - bias > pcfDepth) ? 1.0 : 0.0;
       }
    }
 
    return shadow / 9.0;
}
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir, int index)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
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

    float shadow = SpotShadowCalculation(FragPosSpotLightSpace, normal, lightDir,index);
     
    float vis = 1.0 - shadow;

    return ambientComp + vis * (diffuseComp + specularComp);
}


//------------------------------------
//-----------Helpers------------------
//------------------------------------

vec3 CalDiffuseColor()
{
    if (material.active_diffuse_maps > 0)
        return texture(material.texture_diffuse, TexCoords).rgb;
    return vec3(1.0);
}
vec3 CalSpecularColor()
{
    if (material.active_specular_maps > 0)
        return texture(material.texture_specular, TexCoords).rgb;
    return vec3(1.0);
}