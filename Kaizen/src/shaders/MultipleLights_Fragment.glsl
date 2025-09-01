#version 460 core

#define MAX_POINT_LIGHTS 4
#define MAX_SPOT_LIGHTS 4

struct Material {
    //holds the count of active textures
    int active_diffuse_maps;
    int active_normal_maps;
    int active_specular_maps;
    int active_emission_maps;
    int active_height_maps;

    float shininess;
};

layout (binding = 0) uniform sampler2D texture_diffuse;
layout (binding = 1) uniform sampler2D texture_normal;
layout (binding = 2) uniform sampler2D texture_specular;
layout (binding = 3) uniform sampler2D texture_emission;
layout (binding = 4) uniform sampler2D texture_height;

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

layout (location = 0)out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
    vec4 FragPosDirLightSpace;
    vec4 FragPosSpotLightSpace;
} fs_in;

layout(binding = 5) uniform sampler2D directionalShadowMap;
layout(binding = 6) uniform sampler2D spotShadowMap[MAX_SPOT_LIGHTS];
layout(binding = 10) uniform samplerCube pointShadowMap[MAX_POINT_LIGHTS];

// These tell the shader how many lights in the array are actually active
uniform int activePointLightCount;
uniform int activeSpotLightCount;

uniform float pointLightFarPlane;
uniform vec3 viewPos; 
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLight[MAX_SPOT_LIGHTS];

uniform Material material;
uniform bool useNormal;
uniform bool useParallelMapping;

uniform float height_scale;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir,vec2 texCoods);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec2 texCoods, int index);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir,vec2 texCoods,  int index);

vec3 CalDiffuseColor(vec2 texCoords);
vec3 CalSpecularColor(vec2 texCoords);

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
    viewDir = normalize(viewDir);
    if (viewDir.z <= 0.001) return texCoords; 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * height_scale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(texture_height, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(texture_height, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(texture_height, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
} 

void main()
{

    vec2 texCoords;
    if(material.active_height_maps > 0 && useParallelMapping)
    {
        vec3 viewDirTangentSpace = normalize(viewPos - fs_in.FragPos); 	
        viewDirTangentSpace = transpose(fs_in.TBN) * viewDirTangentSpace;
        viewDirTangentSpace.y *= -1.0;

        texCoords = ParallaxMapping(fs_in.TexCoords,  viewDirTangentSpace);
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            discard;
    }else
    {
        texCoords = fs_in.TexCoords;
    }

    if (material.active_diffuse_maps > 0 &&
        texture(texture_diffuse, texCoords).a < 0.1) discard;
    
    vec3 N;
    if (material.active_normal_maps > 0 && useNormal) 
    {
        vec3 nrm = texture(texture_normal, texCoords).rgb * 2.0 - 1.0;
        N = normalize(fs_in.TBN * nrm);
    } else {
        // fs_in.TBN’s third column is the world normal if TBN is built correctly
        N = normalize(fs_in.TBN[2]);
    }

    vec3 norm = N;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos); 	
    
    vec3 result = vec3(0.0);

    // phase 1: Directional light
    result = CalcDirLight(dirLight, norm, viewDir,texCoords);

    // phase 2: point lights
    for(int i = 0; i < activePointLightCount; i++)
        result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir, texCoords, i);    

    // phase 3: spot light
    for(int i = 0; i < activeSpotLightCount; i++)
        result += CalcSpotLight(spotLight[i], norm, fs_in.FragPos, viewDir, texCoords,i);
        
    vec3 emission = vec3(0.0);
    if (material.active_emission_maps > 0)
        emission = texture(texture_emission, texCoords).rgb;

    FragColor = vec4(result + emission, 1.0);


    float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
} 

//--------------------------------------------------------------------
//-------------------------Dir Light----------------------------------
//---------------------------------------------------------------------

float DirShadowCalculation(vec4 fragPosLightSpace,vec3 norm)
{
       // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(norm);
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

vec3 CalcDirLight(DirLight light, vec3 norm, vec3 viewDir,vec2 texCoods)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayVec = normalize(viewDir + lightDir);   // Blinn-Phong

    float spec = pow(max(dot(norm, halfwayVec), 0.0), material.shininess);

    // combine results
    vec3 ambientComp = light.ambientIntensity * CalDiffuseColor(texCoods);
    vec3 diffuseComp = light.diffuseIntensity * diff * CalDiffuseColor(texCoods);
    vec3 specularComp = light.specularIntensity * spec * CalSpecularColor(texCoods);

    float shadow = DirShadowCalculation(fs_in.FragPosDirLightSpace,norm);                      
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
    float diskRadius = (1.0 + (viewDistance / pointLightFarPlane)) / 200.0; // as view distance gets closer disk radius become smalller
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

vec3 CalcPointLight(PointLight light, vec3 norm, vec3 fragPos, vec3 viewDir,vec2 texCoords, int index)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayVec = normalize(viewDir + lightDir);   // Blinn-Phong
    float spec = pow(max(dot(norm, halfwayVec), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // combine results
    vec3 ambientComp = light.ambientIntensity * CalDiffuseColor(texCoords);
    vec3 diffuseComp = light.diffuseIntensity * diff * CalDiffuseColor(texCoords);
    vec3 specularComp = light.specularIntensity * spec * CalSpecularColor(texCoords);

    float shadow = ShadowPointCalculation(fragPos,light.position,index);

    return (ambientComp + (1.0  - shadow) * (diffuseComp + specularComp)) * attenuation * light.color;
}

//--------------------------------------------------------------------
//-------------------------Spot Light----------------------------------
//---------------------------------------------------------------------

float SpotShadowCalculation(vec4 fragPosLightSpace, vec3 norm, vec3 lightDir, int index)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    if (projCoords.z > 1.0) return 0.0;
    
    float slopeBias  = 0.002 * (1.0 - max(dot(norm, lightDir),0.0));
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
vec3 CalcSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec2 texCoords, int index)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(norm, lightDir), 0.0);

    // specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayVec = normalize(viewDir + lightDir); // Blinn-Phong
    float spec = pow(max(dot(norm, halfwayVec), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambientComp = light.color * light.ambientIntensity * CalDiffuseColor(texCoords);
    vec3 diffuseComp = light.color * light.diffuseIntensity * diff * CalDiffuseColor(texCoords);
    vec3 specularComp = light.color * light.specularIntensity * spec * CalSpecularColor(texCoords);

    ambientComp *= attenuation * intensity;
    diffuseComp *= attenuation * intensity;
    specularComp *= attenuation * intensity;

    float shadow = SpotShadowCalculation(fs_in.FragPosSpotLightSpace, norm, lightDir,index);
     
    float vis = 1.0 - shadow;

    return ambientComp + vis * (diffuseComp + specularComp);
}


//------------------------------------
//-----------Helpers------------------
//------------------------------------

vec3 CalDiffuseColor(vec2 texCoords)
{
    if (material.active_diffuse_maps > 0)
        return texture(texture_diffuse, texCoords).rgb;
    return vec3(1.0);
}
vec3 CalSpecularColor(vec2 texCoords)
{
    if (material.active_specular_maps > 0)
        return texture(texture_specular, texCoords).rgb;
    return vec3(1.0);
}