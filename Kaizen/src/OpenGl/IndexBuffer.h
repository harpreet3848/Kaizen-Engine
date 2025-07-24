#pragma once


class IndexBuffer {
private:
	unsigned int m_RendererID;
public:
	IndexBuffer(const void* data, unsigned int size);
	~IndexBuffer();

	void Bind() const;
	void UnBind() const;
};