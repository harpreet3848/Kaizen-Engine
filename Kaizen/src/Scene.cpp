#include "Scene.h"
#include "OpenGl/OpenGlConfigurations.h"
#include "OpenGl/OpenGLRenderer.h"
#include "OpenGl/ShapeGenerator.h"

Scene::Scene() : camera(glm::vec3(0.0f, 0.0f, 3.0f)),
                lastX(static_cast<float>(EngineConstants::SCR_WIDTH) / 2.0f),
                lastY(static_cast<float>(EngineConstants::SCR_HEIGHT) / 2.0f),
                firstMouse(true),
                deltaTime(0.0f),
                lastFrame(0.0f)
{

}

void Scene::Init() {

    Window::GetInstance().SetWindowCursor(false);
    Window::GetInstance().SetCursorPosCallback([this](double xpos, double ypos) {
        this->mouse_callback(xpos, ypos);
        });

    ourShader = std::make_unique<Shader>("shaders/Default_Vertex.glsl", "shaders/MultipleLights_Fragment.glsl");
    lightCubeShader = std::make_unique<Shader>("shaders/Default_Vertex.glsl", "shaders/lightColor_Fragment.glsl");
    shaderSingleColor = std::make_unique<Shader>("shaders/Default_Vertex.glsl", "shaders/stencil_single_color.glsl");
    screenShader = std::make_unique<Shader>("shaders/framebuffers_screen_Vertex.glsl", "shaders/framebuffers_screen_Fragment.glsl");
   
    ourModel = std::make_unique<Model>("Resources/objects/UfoBasic/ufo.obj", true, false);

    std::vector<std::string> facesFilepaths
    {
        std::filesystem::path("Resources/skybox/right.jpg").string(),
        std::filesystem::path("Resources/skybox/left.jpg").string(),
        std::filesystem::path("Resources/skybox/top.jpg").string(),
        std::filesystem::path("Resources/skybox/bottom.jpg").string(),
        std::filesystem::path("Resources/skybox/front.jpg").string(),
        std::filesystem::path("Resources/skybox/back.jpg").string()
    };

    skybox.Init(facesFilepaths);

    quadVertexArray = ShapeGenerator::GenerateQuad();
    lightVAO = ShapeGenerator::GenerateCube();

    OpenGLConfigurations::EnableDepthTesting();

    OpenGLConfigurations::EnableFaceCulling();
    OpenGLConfigurations::SetFaceCullingMode(FaceCullMode::FRONT);
    OpenGLConfigurations::SetWindingOrder(WindingOder::CLOCKWISE);
}

void Scene::Run() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput();

    frameBuffer.BindToFrameBuffer();

    OpenGLConfigurations::EnableStencilTesting();
    // Enable depth testing for the entire frame by default.
    OpenGLConfigurations::EnableDepthTesting();
    // Set the stencil operation. This will be used when we draw the object to be outlined.
    OpenGLConfigurations::SetStencilActions(StencilAction::KEEP, StencilAction::KEEP, StencilAction::REPLACE);

    OpenglRenderer::ClearColor();
    OpenglRenderer::ClearAllBuffer();


    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;

    projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(EngineConstants::SCR_WIDTH) / static_cast<float>(EngineConstants::SCR_HEIGHT), 0.1f, 100.0f);
    //camera.ProcessMouseMovement(0, 0, false);
    //camera.Yaw += 180.0f;
    view = camera.GetViewMatrix();

    ourShader->use();
    ourShader->setVec3("viewPos", camera.Position);
    ourShader->setFloat("material.shininess", 64.0f);

    ourShader->setMat4("view", view);
    ourShader->setMat4("projection", projection);

    float lightingScale = 1.0f;
    // directional light
    ourShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    ourShader->setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f) * lightingScale);
    ourShader->setVec3("dirLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f) * lightingScale);
    ourShader->setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f) * lightingScale);
    // point light 1
    ourShader->setVec3("pointLights[0].position", pointLightPositions[0]);
    ourShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    ourShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    ourShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    ourShader->setFloat("pointLights[0].constant", 1.0f);
    ourShader->setFloat("pointLights[0].linear", 0.09f);
    ourShader->setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    ourShader->setVec3("pointLights[1].position", pointLightPositions[1]);
    ourShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    ourShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    ourShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    ourShader->setFloat("pointLights[1].constant", 1.0f);
    ourShader->setFloat("pointLights[1].linear", 0.09f);
    ourShader->setFloat("pointLights[1].quadratic", 0.032f);
    // spotLight
    ourShader->setVec3("spotLight.position", camera.Position);
    ourShader->setVec3("spotLight.direction", camera.Front);
    ourShader->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    ourShader->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    ourShader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    ourShader->setFloat("spotLight.constant", 1.0f);
    ourShader->setFloat("spotLight.linear", 0.09f);
    ourShader->setFloat("spotLight.quadratic", 0.032f);
    ourShader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    ourShader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


    shaderSingleColor->use();
    shaderSingleColor->setMat4("view", view);
    shaderSingleColor->setMat4("projection", projection);


    skybox.Draw(view, projection);

    OpenGLConfigurations::SetStencilFunction(StencilAction::ALWAYS, 1, 0xFF);// The stencil test should always pass.
    OpenGLConfigurations::SetStencilMaskWriteALL();// Enable writing to the stencil buffer.


    float rotationSpeed = 200.0f;
    float floatingSpeed = 5.0f;
    float bendMulti = 6.0f;

    // Pass 1
    ourShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, static_cast<float>(glm::radians(glfwGetTime() * rotationSpeed)), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, static_cast<float>(glm::radians(glm::sin(glfwGetTime() * floatingSpeed) * bendMulti)), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    ourShader->setMat4("model", model);


    ourModel->Draw(*ourShader);

    // Pass 2
    OpenGLConfigurations::SetStencilFunction(StencilAction::NOT_EQUAL, 1, 0xFF);
    OpenGLConfigurations::SetStencilMaskReadOnly();
    OpenGLConfigurations::DisableDepthTesting();
    //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //glStencilMask(0x00);
    //glDisable(GL_DEPTH_TEST);

    float scaleFactor = 0.0108f;
    shaderSingleColor->use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::rotate(model, static_cast<float>(glm::radians(glfwGetTime() * rotationSpeed)), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, static_cast<float>(glm::radians(glm::sin(glfwGetTime() * floatingSpeed) * bendMulti)), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    shaderSingleColor->setMat4("model", model);
    ourModel->Draw(*shaderSingleColor);

    OpenGLConfigurations::SetStencilMaskWriteALL();
    OpenGLConfigurations::SetStencilFunction(StencilAction::ALWAYS, 0, 0xFF);
    OpenGLConfigurations::DisableStencilTesting();
    OpenGLConfigurations::EnableDepthTesting();

    OpenGLConfigurations::DisableFaceCulling();


    // also draw the lamp object(s)
    lightCubeShader->use();
    lightCubeShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    lightCubeShader->setMat4("projection", projection);
    lightCubeShader->setMat4("view", view);

    lightVAO->Bind();

    // we now draw as many light bulbs as we have point lights.
    for (unsigned int i = 0; i < 2; i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        lightCubeShader->setMat4("model", model);
        OpenglRenderer::DrawIndexed(lightVAO);

    }
    OpenGLConfigurations::EnableFaceCulling();
    // Render ScreenQuad
    screenShader->use();
    screenShader->setInt("screenTexture", 0);
    frameBuffer.BindToTexture();
    quadVertexArray->Bind();
    //std::cout << vertexArray.GetVertexBuffers()[0].get()->GetLayout().GetElements()[0].Size << std::endl;
    OpenglRenderer::DrawIndexed(quadVertexArray);
}


// glfw: whenever the mouse moves, this callback is called
void Scene::mouse_callback(double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}


void Scene::processInput()
{
    if (Input::IsKeyPressed(KZ_KEY_ESCAPE))
        Window::GetInstance().CloseWindow();

    if (Input::IsKeyPressed(KZ_KEY_W))
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (Input::IsKeyPressed(KZ_KEY_S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (Input::IsKeyPressed(KZ_KEY_A))
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (Input::IsKeyPressed(KZ_KEY_D))
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (Input::IsKeyPressed(KZ_KEY_E))
        camera.ProcessKeyboard(UP, deltaTime);

    if (Input::IsKeyPressed(KZ_KEY_Q))
        camera.ProcessKeyboard(Down, deltaTime);
}