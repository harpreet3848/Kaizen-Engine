#pragma once

#include "Buffer.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

struct FrameBufferSettings 
{
	bool multiSampling;
};

class FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height,bool isDepthOnly, bool multiSampling);
	~FrameBuffer();

	void BindToFrameBuffer() const;
	void BindToTexture(GLuint bindIndex) const;
	void UnBind() const;
	GLuint GetTextureID() const;
private:
	uint32_t m_FramebufferID;
	uint32_t m_RenderbufferID;

	uint32_t m_PostProcessingFBO;
	uint32_t m_PostProcessingTCO;

	GLuint m_ColorAttachment;
	GLuint m_DepthAttachment;

	bool m_MultiSampling;
	bool m_IsDepthOnly;

	uint32_t m_Width;
	uint32_t m_Height;

	void createColorDepthAttachments();
	void createDepthOnlyAttachment();
	void createPostProcessingFBO();
};