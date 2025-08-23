#include "PostProcessingFX.h"
#include <OpenGl/OpenGLRenderer.h>

PostProcessingFX::PostProcessingFX()
{
    pingFBO = std::make_shared<FrameBuffer>(EngineConstants::SCR_WIDTH, EngineConstants::SCR_HEIGHT, false, false, true);
    pongFBO = std::make_shared<FrameBuffer>(EngineConstants::SCR_WIDTH, EngineConstants::SCR_HEIGHT, false, false, true);

    shaderBlur = std::make_shared<Shader>("Shaders/Bloom/BloomBlur_Vertex.glsl", "Shaders/Bloom/BloomBlur_Fragment.glsl");

    quadVertices = ShapeGenerator::GenerateQuad(0, 0, 1, 1);
}

PostProcessingFX::~PostProcessingFX()
{
}

GLint PostProcessingFX::Blur(Ref<FrameBuffer> screenFrameBuffer)
{
    bool horizontal = true;
    int amount = 10;

    screenFrameBuffer->BindToTexture(0, 1); 
    GLint inputTex = screenFrameBuffer->GetTextureID(1);
    shaderBlur->use();
    shaderBlur->setInt("image", 0);

    for (int i = 0; i < amount; i++)
    {

        Ref<FrameBuffer>& target = horizontal ? pingFBO : pongFBO;
        target->BindToFrameBuffer();

        shaderBlur->setInt("horizontal", horizontal);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, inputTex);

        //RenderQuad
        quadVertices->Bind();
        OpenglRenderer::DrawIndexed(quadVertices);

        inputTex = target->GetTextureID(0);

        horizontal = !horizontal;
    }
    GLuint blurredTex = (horizontal ? pingFBO : pongFBO )->GetTextureID(0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return blurredTex;
}