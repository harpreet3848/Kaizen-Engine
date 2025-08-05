#include "FrameBuffer.h"

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "Core/EngineConstansts.h"
#include "OpenGLRenderer.h"
#include "OpenGLConfigurations.h"


FrameBuffer::FrameBuffer(uint32_t width, uint32_t height, bool multiSampling)
	: m_Width(width), m_Height (height), m_MultiSampling(multiSampling)
{
	

	glGenFramebuffers(1, &m_FramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

	GLuint textureType = GL_TEXTURE_2D;
	if (multiSampling)
		textureType = GL_TEXTURE_2D_MULTISAMPLE;

	uint32_t sample = 8;
	// create a color attachment texture
	glGenTextures(1, &m_TextureColorbuffer);
	glBindTexture(textureType, m_TextureColorbuffer);
	if (multiSampling) 
	{
		glTexImage2DMultisample(textureType, sample, GL_RGB, width, height, GL_TRUE);
		glBindTexture(textureType, 0);
	}
    else
	{
		glTexImage2D(textureType, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}


	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureType, m_TextureColorbuffer, 0);

	glGenRenderbuffers(1, &m_RenderbufferID);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
	
	if(multiSampling)
	{
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, sample, GL_DEPTH24_STENCIL8, width, height); 
		glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}else
	{
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); 
	}

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID); 

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (multiSampling) 
	{
		glGenFramebuffers(1, &m_PostProcessingFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_PostProcessingFBO);

		glGenTextures(1, &m_PostProcessingTCO);
		glBindTexture(GL_TEXTURE_2D, m_PostProcessingTCO);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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

}

FrameBuffer::~FrameBuffer() {
	glDeleteRenderbuffers(1, &m_RenderbufferID);
	glDeleteTextures(1, &m_TextureColorbuffer);
	glDeleteFramebuffers(1, &m_FramebufferID);

	if (m_MultiSampling) {
        glDeleteTextures(1, &m_PostProcessingTCO);
        glDeleteFramebuffers(1, &m_PostProcessingFBO);
    }
}

void FrameBuffer::BindToFrameBuffer() const {
	// bind to framebuffer and draw scene as we normally would to color texture 

	glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
	// enable depth testing (is disabled for rendering screen-space quad)
	OpenGLConfigurations::EnableDepthTesting();
}

void FrameBuffer::BindToTexture() const {

	if (m_MultiSampling) 
	{
		//glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FramebufferID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_PostProcessingFBO);
		glBlitFramebuffer(0, 0, m_Width, m_Height, 0, 0, m_Width, m_Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	UnBind();

	OpenglRenderer::ClearColor();
	OpenglRenderer::ClearAllBuffer();
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, m_MultiSampling ? m_PostProcessingTCO : m_TextureColorbuffer);	// use the color attachment texture as the texture of the quad plane

	//glDrawArrays(GL_TRIANGLES, 0, 6);
}

void FrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
