#pragma once

#include "Buffer.h"

class VertexBuffer {
private :
	uint32_t m_RendererID;
	BufferLayout m_Layout;
	uint32_t m_size;
public:
	VertexBuffer(const void* data, uint32_t size);
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;

	uint32_t GetSize() const;

	const BufferLayout& GetLayout() const;
	void SetLayout(const BufferLayout& layout);
};