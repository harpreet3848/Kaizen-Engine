#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class PointShadowMap
{
public:

    PointShadowMap(uint32_t width, uint32_t height);
    ~PointShadowMap();

    void BindLayeredForWrite() const;

    void BindToTexture(GLuint slot) const;
    void Unbind() const;

private:
    uint32_t m_depthCubeMap;
    uint32_t m_depthMapFBO;

    uint32_t m_width;
    uint32_t m_height;
};