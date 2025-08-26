#include "Texture.h"

#include <iostream>
#include <filesystem>      // C++17
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Core/Core.h"

Texture::Texture(const char* filePath,const TextureSettings& textureSettings)
{
    // Generate & bind
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);

    // Set wrapping/filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_enum_cast(textureSettings.wrapU));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_enum_cast(textureSettings.wrapV));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_enum_cast(textureSettings.minFilter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_enum_cast(textureSettings.magFilter));

    stbi_set_flip_vertically_on_load(textureSettings.flipTexture);
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);

    GLenum internalFormat = 0;
    if (textureSettings.textureType == TextureType::Diffuse && textureSettings.isGammaCorrection) {
        internalFormat = (nrChannels == 4) ? GL_SRGB_ALPHA : GL_SRGB;
    }
    else {
        if (nrChannels == 4) internalFormat = GL_RGBA;
        else if (nrChannels == 3) internalFormat = GL_RGB;
        else if (nrChannels == 1) internalFormat = GL_RED;
    }

    GLenum dataFormat = 0;
    if (nrChannels == 4) dataFormat = GL_RGBA;
    else if (nrChannels == 3) dataFormat = GL_RGB;
    else if (nrChannels == 1) dataFormat = GL_RED;

    if (data && internalFormat && dataFormat) {
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cerr << "[Texture] failed to load '" << filePath
            << "': " << stbi_failure_reason()
            << " (cwd: " << std::filesystem::current_path() << ")\n";
    }

    stbi_image_free(data);
}

void Texture::ActivateAndBind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
}
GLuint Texture::GetId() const 
{
    return m_textureId;
}

std::string TextureTypeToString(TextureType textureType)
{
    switch (textureType)
    {
    case Diffuse:
        return "texture_diffuse";
    case Specular:
        return "texture_specular";
    case Normal:
        return "texture_normal";
    case Height:
        return "texture_height";
    case Emission:
        return "texture_emission";
    }
    return "";
}