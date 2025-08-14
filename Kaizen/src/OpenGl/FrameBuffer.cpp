#include "FrameBuffer.h"

#include <iostream>

#include "Core/EngineConstansts.h"
#include "OpenGLRenderer.h"
#include "OpenGLConfigurations.h"


static constexpr int MSAA_SAMPLES = 8;


FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, bool isDepthOnly, bool multiSampling)
	: m_Width(width), m_Height (height), m_MultiSampling(multiSampling), m_IsDepthOnly(isDepthOnly)
{
    glGenFramebuffers(1, &m_FramebufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

    if (!m_IsDepthOnly)
    {
        createColorDepthAttachments();
    }
    else
    {
        createDepthOnlyAttachment();
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (multiSampling && !isDepthOnly) 
    {
        createPostProcessingFBO();
	}

}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &m_FramebufferID);
    if (!m_IsDepthOnly) {
        glDeleteRenderbuffers(1, &m_RenderbufferID);
        glDeleteTextures(1, &m_ColorAttachment);
        if (m_MultiSampling) {
            glDeleteTextures(1, &m_PostProcessingTCO);
            glDeleteFramebuffers(1, &m_PostProcessingFBO);
        }
    }
    else {
        glDeleteTextures(1, &m_DepthAttachment);
    }
}

void FrameBuffer::createColorDepthAttachments()
{
    glGenTextures(1, &m_ColorAttachment);
    glBindTexture(m_MultiSampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, m_ColorAttachment);
    if (m_MultiSampling) {
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, GL_RGB, m_Width, m_Height, GL_TRUE);
    }
    else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_MultiSampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, m_ColorAttachment, 0);

    // 2. Create Depth/Stencil Renderbuffer
    glGenRenderbuffers(1, &m_RenderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
    if (m_MultiSampling) {
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, MSAA_SAMPLES, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    }
    else {
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    }
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);
}

void FrameBuffer::createDepthOnlyAttachment()
{
    // --- DEPTH-ONLY FBO: Depth Texture, No Color --- //
    glGenTextures(1, &m_DepthAttachment);
    glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

    // Tell OpenGL we are not drawing to any color buffer
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void FrameBuffer::createPostProcessingFBO()
{
    // Gen normal framebuffer for post processing
    glGenFramebuffers(1, &m_PostProcessingFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_PostProcessingFBO);

    glGenTextures(1, &m_PostProcessingTCO);
    glBindTexture(GL_TEXTURE_2D, m_PostProcessingTCO);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PostProcessingTCO, 0);

    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Post-Processing Framebuffer error: " << fboStatus << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::BindToFrameBuffer() const 
{
    glViewport(0, 0, m_Width, m_Height);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_FramebufferID);

    OpenglRenderer::ClearColor();

    OpenglRenderer::ClearColourBuffer();
    OpenglRenderer::ClearDepthBuffer();

    if(!m_IsDepthOnly)
        OpenglRenderer::ClearStencilBuffer();

}

void FrameBuffer::BindToTexture(GLuint bindIndex) const {

	if (m_MultiSampling && !m_IsDepthOnly) 
	{
        GLint prevReadFBO = 0, prevDrawFBO = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevReadFBO);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDrawFBO);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_PostProcessingFBO);

        glBlitFramebuffer(0, 0, m_Width, m_Height,0, 0, m_Width, m_Height,GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Restore previous FBO bindings (no surprises for the caller)
        glBindFramebuffer(GL_READ_FRAMEBUFFER, prevReadFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDrawFBO);
	}

    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(bindIndex));

	glBindTexture(GL_TEXTURE_2D, GetTextureID());	// use the color attachment texture as the texture of the quad plane

	//glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
GLuint FrameBuffer::GetTextureID() const
{
    if (m_IsDepthOnly) {
        return m_DepthAttachment;
    }
    return m_MultiSampling ? m_PostProcessingTCO : m_ColorAttachment;
}