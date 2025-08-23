#pragma once

#include "Buffer.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

class FrameBuffer {
public:
	FrameBuffer(uint32_t width, uint32_t height,bool isDepthOnly, bool multiSampling, bool hdr = false, int colorAttachmentCount = 1);
	~FrameBuffer();

	void BindToFrameBuffer() const;
	void BindToTexture(GLuint bindIndex = 0,GLuint attachmentIndex = 0) const;
	void UnBind() const;
	GLuint GetTextureID(GLuint attachmentIndex) const;

private: 
	void createColorDepthAttachments();
	void createDepthOnlyAttachment();
	void createPostProcessingResolve();

private:
	bool m_MultiSampling;
	bool m_IsDepthOnly;
	bool m_HDR;

	uint32_t m_FramebufferID;
	uint32_t m_RenderbufferID;
	GLuint m_DepthAttachment;

	uint32_t m_PostProcessingFBO;
	uint32_t m_PostProcessingTCO;

	std::vector<GLuint> m_ColorAttachment;

	uint32_t m_ColorCount;

	GLuint m_ResolveFBO = 0;
	std::vector<GLuint> m_ResolvedTex; // size == m_ColorAttachments.size()

	uint32_t m_Width;
	uint32_t m_Height;
};