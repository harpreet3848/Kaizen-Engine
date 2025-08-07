#include "UniformBuffer.h"

#include "glad/glad.h"


UniformBuffer::UniformBuffer(GLsizeiptr bufferSize)
{
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
	glBufferData(GL_UNIFORM_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

UniformBuffer::~UniformBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void UniformBuffer::Bind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
}

void UniformBuffer::Unbind() const
{
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::SetBufferSubData(GLintptr offset, GLsizeiptr size, const void* data) const
{
	Bind();
	glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	Unbind();
}
void UniformBuffer::BindBufferRange(const uint32_t blockBindingIndex, const GLsizeiptr offset, const GLsizeiptr size) const
{
	glBindBufferRange(GL_UNIFORM_BUFFER, blockBindingIndex, m_RendererID, offset, size);
}
