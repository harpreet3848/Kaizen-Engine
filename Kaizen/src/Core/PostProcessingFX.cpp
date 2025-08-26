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

Ref<FrameBuffer> PostProcessingFX::Blur(Ref<FrameBuffer> hdrFrameBuffer, GLint attachmentIndex)
{
    bool horizontal = true;
    int amount = 10;

    hdrFrameBuffer->BindToTexture(0, attachmentIndex);
    GLint inputTex = hdrFrameBuffer->GetTextureID(attachmentIndex);

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

        // Next pass reads from what we just wrote
        inputTex = target->GetTextureID(0);

        horizontal = !horizontal;
    }
    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return horizontal ? pingFBO : pongFBO;
}