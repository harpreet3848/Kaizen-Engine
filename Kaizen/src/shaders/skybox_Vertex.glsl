#version 460 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140,binding = 0) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

void main()
{
    TexCoords = aPos;
    // Remove translation from the view matrix
    mat4 viewNoTranslation = mat4(mat3(view));

    vec4 pos = projection * viewNoTranslation * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}