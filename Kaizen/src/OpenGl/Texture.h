#pragma once

#include <glad/glad.h>  // for GLuint
#include <string>

enum TextureType {
    diffuse,
    specular,
    normal,
    height
};

std::string TextureTypeToString(TextureType textureType);

struct TextureSettings {
    TextureType textureType;
    bool isGammaCorrection;
};

class Texture {
private:
    GLuint m_textureId;
public:
    // Constructs and uploads a 2D texture from the given file path.
    Texture(const char* filePath,const TextureSettings& textureSettings);

    // Activate the specified texture unit and bind this texture.
    void ActivateAndBind(GLuint unit) const;

    GLuint GetId() const;
};
