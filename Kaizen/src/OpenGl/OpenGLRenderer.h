#pragma once


#include "VertexArray.h"
#include "IndexBuffer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenglRenderer {
public:

	static void ClearColor();
	static void ClearAllBuffer();
	static void ClearColourBuffer();
	static void ClearDepthBuffer();
	static void ClearStencilBuffer();

	static void DrawTriangles(const unsigned numberOfVertices);
	static void DrawIndexed(const Ref<VertexArray>& vertexArray);
};