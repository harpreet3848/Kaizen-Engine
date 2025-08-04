// Mesh.h
#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>

#include "OpenGl/VertexArray.h"
#include "OpenGl/VertexBuffer.h"
#include "OpenGl/IndexBuffer.h"
#include "OpenGl/Shader.h"
#include "OpenGl/Texture.h"


#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    int   m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct TextureData {
    uint32_t id = 0;
    TextureType  type;
    std::string  path;
};

class Mesh {
public:
    Mesh(std::vector<Vertex>      vertices,
        std::vector<uint32_t> indices,
        std::vector<TextureData>  textures);
    void Draw(Shader& shader);

private:
    // mesh data
    std::vector<Vertex>       _vertices;
    std::vector<uint32_t> _indices;
    std::vector<TextureData>  _textures;

    VertexArray               _vertexArray;

    void setupMesh();
};
