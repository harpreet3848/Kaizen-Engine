#pragma once

#include "Buffer.h"

#include <iostream>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "../Core/EngineConstansts.h"
#include "OpenGLRenderer.h"

class FrameBuffer {
private:
	uint32_t m_framebufferID;
	unsigned int textureColorbuffer;
	unsigned int rbo;
public:
	FrameBuffer()
	{
		glGenFramebuffers(1, &m_framebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);

		// create a color attachment texture
		glGenTextures(1, &textureColorbuffer);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,EngineConstants::SCR_WIDTH, EngineConstants::SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	
		glGenRenderbuffers(1, &rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, rbo);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, EngineConstants::SCR_WIDTH, EngineConstants::SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
		// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	~FrameBuffer() {
		glDeleteRenderbuffers(1, &rbo);
		glDeleteTextures(1, &textureColorbuffer);
		glDeleteFramebuffers(1, &m_framebufferID);
	}

	void BindToFrameBuffer() const {
		// bind to framebuffer and draw scene as we normally would to color texture 
		glBindFramebuffer(GL_FRAMEBUFFER, m_framebufferID);
		// enable depth testing (is disabled for rendering screen-space quad)
		glEnable(GL_DEPTH_TEST);
	}
	void BindToTexture() const {
		
		UnBind();

		OpenglRenderer::ClearColor();
		OpenglRenderer::ClearAllBuffer();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane

		//glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	void UnBind() const 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
};