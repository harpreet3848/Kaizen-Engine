#include "PointShadowMap.h"
#include "OpenGLRenderer.h"
#include <iostream>

PointShadowMap::PointShadowMap(uint32_t width, uint32_t height)
	:m_width(width), m_height(height)
{
	glGenTextures(1, &m_depthCubeMap);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glGenFramebuffers(1, &m_depthMapFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "PointShadowMap: layered FBO not complete!\n";
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

PointShadowMap::~PointShadowMap()
{
	glDeleteFramebuffers(1, &m_depthMapFBO);
	glDeleteTextures(1, &m_depthCubeMap);

}

void PointShadowMap::BindLayeredForWrite() const
{
	glViewport(0, 0, m_width, m_height);
	glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);

	OpenglRenderer::ClearDepthBuffer();
}
void PointShadowMap::BindToTexture(GLuint slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);

	glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
}
void PointShadowMap::Unbind() const 
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}