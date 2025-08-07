#pragma once

#include "Core/Core.h"

class UniformBuffer
{
public:
	UniformBuffer(GLsizeiptr bufferSize);
	virtual ~UniformBuffer();
	uint32_t GetRendererID() const { return m_RendererID; }
	void Bind() const;
	void Unbind() const;
	void SetBufferSubData(GLintptr offset, GLsizeiptr size, const void* data) const;
	void BindBufferRange(const uint32_t blockBindingIndex, const GLsizeiptr offset, const GLsizeiptr size) const;
private:
	uint32_t m_RendererID = 0;
};
