#pragma once
#include <cstdint>


class IndexBuffer {
private:
	uint32_t m_RendererID;
	uint32_t m_count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	uint32_t GetCount() const;


	void Bind() const;
	void UnBind() const;
};