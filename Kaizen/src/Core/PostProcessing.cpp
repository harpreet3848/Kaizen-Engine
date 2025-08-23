#pragma once
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

class PostProcessing
{
public:
	PostProcessing();
	~PostProcessing();
    void blur();
private:

    uint32_t m_Width;
    uint32_t m_Height;
};

PostProcessing::PostProcessing()
{
    //unsigned int hdrFBO;
    //glGenFramebuffers(1, &hdrFBO);
    //glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    //unsigned int colorBuffers[2];
    //glGenTextures(2, colorBuffers);
    //for (unsigned int i = 0; i < 2; i++)
    //{
    //    glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
    //    glTexImage2D(
    //        GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL
    //    );
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    // attach texture to framebuffer
    //    glFramebufferTexture2D(
    //        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0
    //    );
    //}

    //unsigned int rboDepth;
    //glGenRenderbuffers(1, &rboDepth);
    //glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    //unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    //glDrawBuffers(2, attachments);

    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //    std::cout << "Framebuffer not complete!" << std::endl;
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);



    //unsigned int pingpongFBO[2];
    //unsigned int pingpongColorbuffers[2];
    //glGenFramebuffers(2, pingpongFBO);
    //glGenTextures(2, pingpongColorbuffers);
    //for (unsigned int i = 0; i < 2; i++)
    //{
    //    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
    //    glBindTexture(GL_TEXTURE_2D, pingpongColorbuffers[i]);
    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, m_Width, m_Height, 0, GL_RGBA, GL_FLOAT, NULL);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
    //    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorbuffers[i], 0);
    //    // also check if framebuffers are complete (no need for depth buffer)
    //    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    //        std::cout << "Framebuffer not complete!" << std::endl;
    //}

}

PostProcessing::~PostProcessing()
{
}

void PostProcessing::blur() 
{

}