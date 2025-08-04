#include "ShapeGenerator.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Buffer.h" // For BufferLayout



Ref<VertexArray> ShapeGenerator::GenerateSquare()
{
    static float vertices[] = {
        // positions          // textureCoord 
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,
    };

    static uint32_t indices[] = {
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };

    auto vertexBuffer = std::make_shared<VertexBuffer>(vertices, static_cast<uint32_t>(sizeof(vertices)));

    const BufferLayout bufferlayout =
    {
         {ShaderDataType::Float3, "a_Position",false },
         {ShaderDataType::Float2, "a_TextureCoord",false },
    };
    vertexBuffer->SetLayout(bufferlayout);

    auto vertexArray = std::make_shared<VertexArray>();
    vertexArray->AddVertexBuffer(vertexBuffer);

    auto indexBuffer = std::make_shared<IndexBuffer>(indices, static_cast<uint32_t>(sizeof(indices)));
    vertexArray->AddIndexBuffer(indexBuffer);

    return vertexArray;
}

Ref<VertexArray> ShapeGenerator::GenerateQuad()
{
    static float quadVertices[] = {
        // positions      // texture Coords
         1.0f,  1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 1.0f,
        -1.0f, -1.0f,   0.0f, 0.0f,
         1.0f, -1.0f,   1.0f, 0.0f
    };

    static uint32_t indices[] = {
        1, 2, 3,
        1, 3, 0
    };

    auto vertexBuffer = std::make_shared<VertexBuffer>(quadVertices, static_cast<uint32_t>(sizeof(quadVertices)));

    const BufferLayout bufferlayout =
    {
         {ShaderDataType::Float2, "a_Position",false },
         {ShaderDataType::Float2, "a_TextureCoord",false },
    };
    vertexBuffer->SetLayout(bufferlayout);

    auto vertexArray = std::make_shared<VertexArray>();
    auto indexBuffer = std::make_shared<IndexBuffer>(indices, static_cast<uint32_t>(sizeof(indices)));

    vertexArray->AddVertexBuffer(vertexBuffer);
    vertexArray->AddIndexBuffer(indexBuffer);

    return vertexArray;
}

Ref<VertexArray> ShapeGenerator::GenerateCube()
{
    static float cubeVertices[] = {
        // positions          // texture Coords
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 1
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 3
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 4
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 5
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 6
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 7
        // Left face
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 8
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 9
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 10
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 11
        // Right face
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 12
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 13
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 14
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 15
         // Bottom face
         -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 16
          0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // 17
          0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 18
         -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 19
         // Top face
         -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 20
          0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 21
          0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 22
         -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // 23
    };

    static uint32_t indices[] = {
        // Back face
        0, 2, 1, 2, 0, 3,
        // Front face
        4, 6, 5, 6, 4, 7,
        // Left face
        8, 10, 9, 10, 8, 11,
        // Right face
        12, 14, 13, 14, 12, 15,
        // Bottom face
        16, 18, 17, 18, 16, 19,
        // Top face
        20, 22, 21, 22, 20, 23
    };

    auto vertexBuffer = std::make_shared<VertexBuffer>(cubeVertices, static_cast<uint32_t>(sizeof(cubeVertices)));

    const BufferLayout bufferlayout =
    {
         {ShaderDataType::Float3, "a_Position", false },
         {ShaderDataType::Float2, "a_TextureCoord", false },
    };
    vertexBuffer->SetLayout(bufferlayout);

    auto vertexArray = std::make_shared<VertexArray>();

    auto indexBuffer = std::make_shared<IndexBuffer>(indices, static_cast<uint32_t>(sizeof(indices)));

    vertexArray->AddVertexBuffer(vertexBuffer);
    vertexArray->AddIndexBuffer(indexBuffer);

    return vertexArray;
}
