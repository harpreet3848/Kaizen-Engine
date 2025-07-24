#include "VertexBuffer.h"

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "Buffer.h"


VertexBuffer::VertexBuffer(const void* data, uint32_t size)
{
	// Generate 1 Vertex Buffer Object
	glGenBuffers(1, &m_RendererID);
	
	// Bind m_RendererID to the GL_ARRAY_BUFFER target
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	
	// Upload our vertex data array into the GPU buffer
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

const BufferLayout& VertexBuffer::GetLayout() const
{
	return m_Layout;
}

void VertexBuffer::SetLayout(const BufferLayout& layout) 
{
	m_Layout = layout;
}