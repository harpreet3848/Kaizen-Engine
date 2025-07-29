#pragma once

#include <string>
#include <vector>
#include <assimp/scene.h>          // for aiNode, aiScene, aiMesh, aiMaterial, aiTextureType
#include "Mesh.h"
#include "../OpenGl/Shader.h"        
#include "../OpenGl/Texture.h"        


unsigned int TextureFromFile(const char* path, const std::string& directory, TextureType textureType, bool gamma, bool flip);

class Model
{
public:
    // model data
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::vector<TextureData> textures_loaded;   // to avoid loading duplicates
    std::string directory;
    Model(const std::string& path, bool gamma = false, bool flipTexture = false);

    void Draw(Shader& shader);

private:
    bool gammaCorrection;
    bool flipTextures;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<TextureData> loadMaterialTextures(aiMaterial* mat, aiTextureType type, TextureType typeName);
};
