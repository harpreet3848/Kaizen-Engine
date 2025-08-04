#include "OpenGLRenderer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void OpenglRenderer::ClearColor()
{
    // Specify the color of the background
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
}
void OpenglRenderer::ClearAllBuffer()
{
    // Clean the back buffre and assign the new color to it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}
void OpenglRenderer::ClearColourBuffer()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenglRenderer::ClearDepthBuffer()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenglRenderer::ClearStencilBuffer()
{
    glClear(GL_STENCIL_BUFFER_BIT);
}
void OpenglRenderer::DrawTriangles(const uint32_t numberOfVertices)
{
	glDrawArrays(GL_TRIANGLES, 0, numberOfVertices);
}
void OpenglRenderer::DrawIndexed(const Ref<VertexArray>& vertexArray)
{
	glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}