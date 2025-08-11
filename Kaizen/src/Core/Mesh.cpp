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
    unsigned int diffuseNr = 0;
    unsigned int specularNr = 0;
    unsigned int normalNr = 0;
    unsigned int heightNr = 0;
    unsigned int emissionNr = 0;

    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
        const auto& tex = _textures[i];

        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        if (tex.type == TextureType::Diffuse)  number = std::to_string(diffuseNr++);
        else if (tex.type == TextureType::Specular) number = std::to_string(specularNr++); // transfer unsigned int to string
        else if (tex.type == TextureType::Normal)   number = std::to_string(normalNr++);  // transfer unsigned int to string
        else if (tex.type == TextureType::Height)   number = std::to_string(heightNr++); // transfer unsigned int to string
        else if (tex.type == TextureType::Emission) number = std::to_string(emissionNr++);

        std::string uniformName = "material." + TextureTypeToString(tex.type) + "[" + number + "]";

        // now set the sampler to the correct texture unit
        shader.setInt(uniformName.c_str(), i);
        //glUniform1i(glGetUniformLocation(shader.ID, uniformName.c_str()), i);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, tex.id);
    }
    shader.setInt("material.active_diffuse_maps", diffuseNr);
    shader.setInt("material.active_specular_maps", specularNr);
    shader.setInt("material.active_emission_maps", emissionNr);
    // draw mesh
    _vertexArray.Bind();
    glDrawElements(
        GL_TRIANGLES,
        static_cast<uint32_t>(_indices.size()),
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
        static_cast<uint32_t>(_vertices.size() * sizeof(Vertex))
    );
    auto indexBuffer = std::make_shared<IndexBuffer>(
        _indices.data(),
        static_cast<uint32_t>(_indices.size())
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
