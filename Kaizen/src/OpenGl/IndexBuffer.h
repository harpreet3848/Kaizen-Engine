#pragma once
#include <cstdint>


class IndexBuffer {
private:
	uint32_t m_RendererID;
	uint32_t m_count;
public:
	IndexBuffer(const uint32_t* data, uint32_t count);
	~IndexBuffer();

	uint32_t GetCount() const;


	void Bind() const;
	void UnBind() const;
};