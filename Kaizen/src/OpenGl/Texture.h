#pragma once

#include <glad/glad.h>  // for GLuint
#include <string>

enum TextureType {
    Diffuse,
    Specular,
    Normal,
    Height,
    Emission
};

enum class TextureWrapMode {
    Repeat = GL_REPEAT,
    MirroredRepeat = GL_MIRRORED_REPEAT,
    ClampToEdge = GL_CLAMP_TO_EDGE,
    ClampToBorder = GL_CLAMP_TO_BORDER
};

enum class TextureFilterMode {
    Nearest = GL_NEAREST,
    Linear = GL_LINEAR,
    NearestMipmapNearest = GL_NEAREST_MIPMAP_NEAREST,
    LinearMipmapNearest = GL_LINEAR_MIPMAP_NEAREST,
    NearestMipmapLinear = GL_NEAREST_MIPMAP_LINEAR,
    LinearMipmapLinear = GL_LINEAR_MIPMAP_LINEAR
};

struct TextureSettings {
    TextureType textureType;

    TextureWrapMode wrapU;
    TextureWrapMode wrapV;

    TextureFilterMode minFilter;
    TextureFilterMode magFilter;

    bool isGammaCorrection;
    bool flipTexture;

    TextureSettings() :
        textureType(TextureType::Diffuse),
        isGammaCorrection(false),
        flipTexture(true),
        wrapU(TextureWrapMode::Repeat),
        wrapV(TextureWrapMode::Repeat),
        minFilter(TextureFilterMode::LinearMipmapLinear),
        magFilter(TextureFilterMode::Linear) {
    }
};

std::string TextureTypeToString(TextureType textureType);

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

