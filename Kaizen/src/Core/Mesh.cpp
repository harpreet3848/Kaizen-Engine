// Mesh.cpp
#include "Mesh.h"
#include <glad/glad.h>

Mesh::Mesh(std::vector<Vertex>      vertices, std::vector<unsigned int> indices, std::vector<TextureData>  textures)
    : _vertices(std::move(vertices)), _indices(std::move(indices)), _textures(std::move(textures))
{
    setupMesh();
}

void Mesh::Draw(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        const auto& tex = _textures[i];

        // retrieve texture number (the N in diffuse_textureN)

        std::string number;
        if (tex.type == TextureType::diffuse)  number = std::to_string(diffuseNr++);
        if (tex.type == TextureType::specular) number = std::to_string(specularNr++); // transfer unsigned int to string
        if (tex.type == TextureType::normal)   number = std::to_string(normalNr++);  // transfer unsigned int to string
        if (tex.type == TextureType::height)   number = std::to_string(heightNr++); // transfer unsigned int to string

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, (TextureTypeToString(tex.type) + number).c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, tex.id);
    }

    // draw mesh
    _vertexArray.Bind();
    glDrawElements(
        GL_TRIANGLES,
        static_cast<unsigned int>(_indices.size()),
        GL_UNSIGNED_INT,
        0
    );

    // always good practice to set everything back to defaults once configured.
    _vertexArray.UnBind();
    glActiveTexture(GL_TEXTURE0);
}

// initializes all the buffer objects/arrays
void Mesh::setupMesh()
{
    auto vertexBuffer = std::make_shared<VertexBuffer>(
        _vertices.data(),
        _vertices.size() * sizeof(Vertex)
    );
    auto indexBuffer = std::make_shared<IndexBuffer>(
        _indices.data(),
        _indices.size() * sizeof(unsigned int)
    );

    BufferLayout layout = {
        { ShaderDataType::Float3, "a_Position", false },
        { ShaderDataType::Float3, "a_Normal",   false },
        { ShaderDataType::Float2, "a_TexCoords",false },
        { ShaderDataType::Float3, "a_Tangent",  false },
        { ShaderDataType::Float3, "a_Bitangent",false },
        { ShaderDataType::Float4, "a_Bone",     false },
        { ShaderDataType::Float4, "a_Weights",  false },
    };

    vertexBuffer->SetLayout(layout);
    _vertexArray.AddVertexBuffer(vertexBuffer);
    _vertexArray.AddIndexBuffer(indexBuffer);
    _vertexArray.UnBind();
}
