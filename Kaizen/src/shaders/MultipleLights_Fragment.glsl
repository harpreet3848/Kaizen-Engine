#version 330 core

// Define max textures per type
#define MAX_DIFFUSE_MAPS 4
#define MAX_SPECULAR_MAPS 4

struct Material {
    sampler2D texture_diffuse[MAX_DIFFUSE_MAPS];
    sampler2D texture_specular[MAX_SPECULAR_MAPS];

    // Add uniforms to hold the count of active textures
    int active_diffuse_maps;
    int active_specular_maps;

    float shininess;
};


struct DirLight {
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

#define NR_POINT_LIGHTS 2
  

out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec3 LightPos; 
in vec2 TexCoords;

uniform vec3 viewPos; 
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform Material material;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalDiffuseColor();
vec3 CalSpecularColor();


void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);  	
    
    vec3 result;
    // phase 1: Directional light
    result = CalcDirLight(dirLight, norm, viewDir);

    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    

    // phase 3: spot light
    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);

    FragColor = vec4(result, 1.0);
    //FragColor = texture(material.texture_specular1,TexCoords);
} 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);

    vec3 halfwayVec = normalize(viewDir + lightDir);     // Bling-Phong

    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);

    // combine results
    vec3 ambient = light.ambient * CalDiffuseColor();
    vec3 diffuse = light.diffuse * diff * CalDiffuseColor();
    vec3 specular = light.specular * spec * CalSpecularColor();

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayVec = normalize(viewDir + lightDir);     // Bling-Phong
    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // combine results
    vec3 ambient = light.ambient * CalDiffuseColor();
    vec3 diffuse = light.diffuse * diff * CalDiffuseColor();
    vec3 specular = light.specular * spec * CalSpecularColor();

    return (ambient + diffuse + specular) * attenuation;
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayVec = normalize(viewDir + lightDir); // Bling-Phong
    float spec = pow(max(dot(normal, halfwayVec), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    vec3 ambient = light.ambient * CalDiffuseColor();
    vec3 diffuse = light.diffuse * diff * CalDiffuseColor();
    vec3 specular = light.specular * spec * CalSpecularColor();

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
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