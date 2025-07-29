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
	unsigned int m_VertexCount;
public:
	VertexArray();
	uint32_t GetId() const;
	~VertexArray();

	void Bind() const;
	void UnBind() const;

	void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
	void AddIndexBuffer(const  Ref<IndexBuffer>& indexBuffer);

	unsigned int GetVertexCount() const { return m_VertexCount; }

	_NODISCARD inline virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
	_NODISCARD inline virtual const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }
};