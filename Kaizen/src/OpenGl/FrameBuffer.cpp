#include "FrameBuffer.h"

#include <iostream>

#include "Core/EngineConstansts.h"
#include "OpenGLRenderer.h"
#include "OpenGLConfigurations.h"
#include <cassert>


static constexpr int MSAA_SAMPLES = 8;


FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, bool isDepthOnly, bool multiSampling, bool hdr, int colorAttachmentCount)
	: m_Width(width), m_Height (height), m_MultiSampling(multiSampling), m_IsDepthOnly(isDepthOnly) , m_HDR(hdr), m_ColorCount(std::max<uint32_t>(isDepthOnly ? 0u : colorAttachmentCount, 0u))
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
        createPostProcessingResolve();
	}

}

FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &m_FramebufferID);
    if (!m_IsDepthOnly) {
        if (m_RenderbufferID) glDeleteRenderbuffers(1, &m_RenderbufferID);

        if (!m_ColorAttachment.empty()) glDeleteTextures((GLsizei)m_ColorAttachment.size(), m_ColorAttachment.data());

        if (m_ResolveFBO) {
            glDeleteFramebuffers(1, &m_ResolveFBO);
        }
        if (!m_ResolvedTex.empty()) {
            glDeleteTextures((GLsizei)m_ResolvedTex.size(), m_ResolvedTex.data());
        }
    }
    else {
        if (m_DepthAttachment) glDeleteTextures(1, &m_DepthAttachment);
    }
}

void FrameBuffer::createColorDepthAttachments()
{
    const GLenum internalFmt = m_HDR ? GL_RGBA16F : GL_RGB8;   // 16f is a great default
    const GLenum dataFmt = m_HDR ? GL_RGBA : GL_RGB;
    const GLenum dataType = m_HDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

    m_ColorAttachment.resize(m_ColorCount, 0);
    
    glGenTextures((GLsizei)m_ColorAttachment.size(), m_ColorAttachment.data());
    
    const GLenum target = m_MultiSampling ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;


    for (uint32_t i = 0; i < m_ColorCount; i++)
    {
        glBindTexture(target, m_ColorAttachment[i]);
        if (m_MultiSampling) {
            glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, MSAA_SAMPLES, internalFmt, m_Width, m_Height, GL_TRUE);
        }
        else {
            glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, m_Width, m_Height, 0, dataFmt, dataType, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, target, m_ColorAttachment[i], 0);
    }
    
    
    std::vector<GLenum> drawBuffers(m_ColorCount);
    for (uint32_t i = 0; i < m_ColorCount; ++i)
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers((GLsizei)drawBuffers.size(), drawBuffers.data());
    

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

void FrameBuffer::createPostProcessingResolve()
{
    glGenFramebuffers(1, &m_ResolveFBO);

    const GLenum internalFmt = m_HDR ? GL_RGBA16F : GL_RGB8;
    const GLenum dataFmt = m_HDR ? GL_RGBA : GL_RGB;
    const GLenum dataType = m_HDR ? GL_FLOAT : GL_UNSIGNED_BYTE;

    m_ResolvedTex.resize(m_ColorCount, 0);

    glGenTextures((GLsizei)m_ResolvedTex.size(), m_ResolvedTex.data());

    for (uint32_t i = 0; i < m_ColorCount; ++i) {
        glBindTexture(GL_TEXTURE_2D, m_ResolvedTex[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFmt, m_Width, m_Height, 0, dataFmt, dataType, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
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

void FrameBuffer::BindToTexture(GLuint bindIndex, GLuint attachmentIndex) const 
{
    if (m_IsDepthOnly) {
        glActiveTexture(GL_TEXTURE0 + bindIndex);
        glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
        return;
    }

    if (attachmentIndex >= m_ColorCount) 
    {
#ifndef NDEBUG
        assert(false && "FrameBuffer: attachmentIndex out of range");
#endif
        attachmentIndex = 0; // safe fallback for release builds
    }


	if (m_MultiSampling) 
	{
        // Resolve this specific attachment into its paired single-sample texture
       
        // Store FBO bindings
        GLint prevReadFBO = 0, prevDrawFBO = 0;
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &prevReadFBO);
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &prevDrawFBO);

        // Read from multisample FBO, COLOR_ATTACHMENTi
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferID);
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);

        // Attach the target single-sample texture as COLOR_ATTACHMENT0 on resolve FBO
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_ResolveFBO);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_2D, m_ResolvedTex[attachmentIndex], 0);

        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        // Copies to m_FramebufferID
        glBlitFramebuffer(0, 0, m_Width, m_Height,0, 0, m_Width, m_Height,
                            GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // Restore previous FBO bindings
        glBindFramebuffer(GL_READ_FRAMEBUFFER, prevReadFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, prevDrawFBO);

        glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(bindIndex));
        glBindTexture(GL_TEXTURE_2D, m_ResolvedTex[attachmentIndex]);
    }
    else {
        glActiveTexture(GL_TEXTURE0 + bindIndex);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachment[attachmentIndex]);
    }
}

void FrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
GLuint FrameBuffer::GetTextureID(GLuint attachmentIndex) const
{
    if (m_IsDepthOnly) return m_DepthAttachment;

    if (attachmentIndex >= m_ColorCount) attachmentIndex = 0;
    if (m_MultiSampling)
        return m_ResolvedTex.empty() ? 0 : m_ResolvedTex[attachmentIndex];
    return m_ColorAttachment[attachmentIndex];
}