#version 460 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float skyExposureIntensity;

void main()
{
    
    FragColor = texture(skybox, TexCoords) * skyExposureIntensity;
}