#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
	switch (type)
	{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
	}
	//Log(false, "Unknown ShaderDataType!");
	return 0;
}


VertexArray::VertexArray()
{
    // Generate 1 Vertex Array Object 
    glGenVertexArrays(1, &m_RendererID);
	glBindVertexArray(m_RendererID);
}

uint32_t VertexArray::GetId() const {
	return m_RendererID;
}


VertexArray::~VertexArray()
{
	//std::cout << "VertexArray Destroyed" << std::endl;
    glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}

void VertexArray::UnBind() const
{
    glBindVertexArray(0);
}
void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
	glBindVertexArray(m_RendererID);
	vertexBuffer->Bind();

	uint32_t index = 0;
	const auto& layout = vertexBuffer->GetLayout();

	for (const auto& element : layout)
	{
		glEnableVertexAttribArray(index);
		// Tell OpenGL how to interpret the vertex data, also Bind VBOs
		glVertexAttribPointer(index,								  //   index=0 matches "layout(location = 0)" in vertex shader
			element.GetComponentCount(),							  //   size=3 floats per vertex, type=float
			ShaderDataTypeToOpenGLBaseType(element.Type),			  //   OpenGL memory store type
			element.Normalized ? GL_TRUE : GL_FALSE,				  //   
			layout.GetStride(),										  //   stride = 3 * sizeof(float) (distance between consecutive vertices)
			reinterpret_cast<const void*>(element.Offset));			  //   offset = 0 (data begins at buffer start)
		index++;
		//std::cout << "Stride :" << layout.GetStride() << " size :"  << element.GetComponentCount() << " DataType :" << ShaderDataTypeToOpenGLBaseType(element.Type) << std::endl; 
	}
	m_VertexBuffers.push_back(vertexBuffer);

	// Unbind VAO to avoid accidental modifications
	glBindVertexArray(0);
}

void VertexArray::AddIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
	glBindVertexArray(m_RendererID);
	indexBuffer->Bind();
	m_IndexBuffer = indexBuffer;
}