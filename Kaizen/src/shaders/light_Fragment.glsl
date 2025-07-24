#version 330 core

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
}; 

struct Light {
    vec3 position;
  
    vec3 ambient;   
    vec3 diffuse;   
    vec3 specular;  
};

  
uniform Material material;
uniform Light light;  


out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
in vec3 LightPos; 
in vec2 TexCoords;

uniform vec3 viewPos; 

void main()
{
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse  = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoords)));
    
    // specular
    float specularStrength = 0.9;
    vec3 viewDir = normalize(viewPos-FragPos);
    vec3 reflectDir = reflect(-lightDir,Normal);

    float spec = pow(max(0.0,dot(viewDir,reflectDir)),material.shininess);
    vec3 specular = light.specular * spec *  vec3(texture(material.specular,TexCoords));  
        
    vec3 emissionMask = step(vec3(1.0f),vec3(1.0f) - vec3(texture(material.specular,TexCoords)));

    vec3 emission =  vec3(texture(material.emission,TexCoords));
    emission *= emissionMask;
    vec3 result = (ambient + diffuse + specular + emission);
    FragColor = vec4(result, 1.0);
} 