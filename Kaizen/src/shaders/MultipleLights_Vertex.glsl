#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;  

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    mat3 TBN;
} vs_out;  
  
out vec4 FragPosDirLightSpace;
out vec4 FragPosSpotLightSpace;


layout (std140, binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

uniform mat4 dirLightSpaceMatrix;
uniform mat4 spotLightSpaceMatrix;

uniform mat4 model;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.TexCoords = aTexCoords;
    
    // Proper normal matrix (handles non-uniform scale)
    mat3 normalMatrix = transpose(inverse(mat3(model)));  

    vec3 N = normalize(normalMatrix * aNormal);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);

    T = normalize(T - N * dot(T, N)); // make orthogonal again
    B = normalize(cross(N, T)); 

    vs_out.TBN  = mat3(T, B, N);
    
    FragPosDirLightSpace = dirLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    FragPosSpotLightSpace = spotLightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}