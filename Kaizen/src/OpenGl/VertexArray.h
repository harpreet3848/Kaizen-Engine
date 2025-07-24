#pragma once
#include <vector>

#include "../Core/Core.h"

class VertexBuffer;
class IndexBuffer;

class VertexArray {
private:
	uint32_t m_RendererID;
	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;
public:
	VertexArray();
	uint32_t GetId() const;
	~VertexArray();

	void Bind() const;
	void UnBind() const;

	void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
	void AddIndexBuffer(const  Ref<IndexBuffer>& indexBuffer);
};