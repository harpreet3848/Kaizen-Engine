#pragma once

#include "Buffer.h"

struct FrameBufferSettings 
{
	bool multiSampling;
};

class FrameBuffer {
private:
	uint32_t m_FramebufferID;
	uint32_t m_RenderbufferID;
	uint32_t m_TextureColorbuffer;

	uint32_t m_PostProcessingFBO;
	uint32_t m_PostProcessingTCO;

	bool m_MultiSampling;

	uint32_t m_Width;
	uint32_t m_Height;

public:
	FrameBuffer(uint32_t width, uint32_t height, bool multiSampling);
	~FrameBuffer();

	void BindToFrameBuffer() const;
	void BindToTexture() const;
	void UnBind() const;
};