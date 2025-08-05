#include "Scene.h"
#include "OpenGl/OpenGlConfigurations.h"
#include "OpenGl/OpenGLRenderer.h"
#include "OpenGl/ShapeGenerator.h"
#include <imgui.h>


Scene::Scene() : camera(glm::vec3(0.0f, 0.0f, 3.0f)),
                lastX(static_cast<float>(EngineConstants::SCR_WIDTH) / 2.0f),
                lastY(static_cast<float>(EngineConstants::SCR_HEIGHT) / 2.0f),
                firstMouse(true),
                deltaTime(0.0f),
                lastFrame(0.0f)
{

}

void Scene::Init() {

    //Window::GetInstance().SetWindowCursor(false);
    //Window::GetInstance().SetCursorPosCallback([this](double xpos, double ypos) {
    //    this->MouseCallback(xpos, ypos);
    //    });

    OpenGLConfigurations::EnableFaceCulling();
    OpenGLConfigurations::SetFaceCullingMode(FaceCullMode::FRONT);
    OpenGLConfigurations::SetWindingOrder(WindingOder::CLOCKWISE);
    

    ourShader = std::make_shared<Shader>("shaders/Default_Vertex.glsl", "shaders/MultipleLights_Fragment.glsl");
    screenShader = std::make_shared<Shader>("shaders/framebuffers_screen_Vertex.glsl", "shaders/framebuffers_screen_Fragment.glsl");
   
    ourModel = std::make_shared<Model>("Resources/objects/medievalCastle/medievalCastle.obj", true, false);
    groundModel = std::make_shared<Model>("Resources/objects/SimpleGround/Ground.obj", true, false);
    frameBuffer = std::make_shared<FrameBuffer>(EngineConstants::SCR_WIDTH,EngineConstants::SCR_HEIGHT,true);


    std::vector<std::string> facesFilepaths
    {
        std::filesystem::path("Resources/toonSkybox/right.png").string(),
        std::filesystem::path("Resources/toonSkybox/left.png").string(),
        std::filesystem::path("Resources/toonSkybox/top.png").string(),
        std::filesystem::path("Resources/toonSkybox/bottom.png").string(),
        std::filesystem::path("Resources/toonSkybox/front.png").string(),
        std::filesystem::path("Resources/toonSkybox/back.png").string()
    };

    skybox.Init(facesFilepaths);

    quadVertexArray = ShapeGenerator::GenerateQuad();

    lightManager = std::make_shared<LightManager>();

    auto directionalLight = std::make_shared<LightComponent>();
    directionalLight->type = LightType::Directional;
    directionalLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
    directionalLight->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
    directionalLight->ambientIntensity = 0.05f;
    directionalLight->diffuseIntensity = 1.0f;
    directionalLight->specularIntensity = 1.0f;

    auto pointLight = std::make_shared<LightComponent>();
    pointLight->type = LightType::Point;
    pointLight->color = glm::vec3(1.0f, 0.0f, 0.0f); // Red light
    pointLight->position = glm::vec3((-2.0f, 0.2f, 1.0f));
    pointLight->ambientIntensity = 0.05f;
    pointLight->diffuseIntensity = 0.8f;
    pointLight->specularIntensity = 1.0f;
    pointLight->constant = 1.0f;
    pointLight->linear = 0.09f;
    pointLight->quadratic = 0.032f;


    auto pointLight2 = std::make_shared<LightComponent>();
    pointLight2->type = LightType::Point;
    pointLight2->color = glm::vec3(1.0f, 0.0f, 0.0f); // Red light
    pointLight2->position = glm::vec3(2.0f, 0.2f, 1.0f);
    pointLight2->ambientIntensity = 0.05f;
    pointLight2->diffuseIntensity = 0.8f;
    pointLight2->specularIntensity = 1.0f;
    pointLight2->constant = 1.0f;
    pointLight2->linear = 0.09f;
    pointLight2->quadratic = 0.032f;

    auto spotLight = std::make_shared<LightComponent>();
    spotLight->type = LightType::Spot;
    spotLight->color = glm::vec3(0.0f, 1.0f, 0.0f); // Green light
    spotLight->position = camera.Position;
    spotLight->direction = camera.Front;
    spotLight->ambientIntensity = 0.0f;
    spotLight->diffuseIntensity = 1.0f;
    spotLight->specularIntensity = 1.0f;
    spotLight->constant = 1.0f;
    spotLight->linear = 0.09f;
    spotLight->quadratic = 0.032f;
    spotLight->cutOff = 12.5f;
    spotLight->outerCutOff = 15.0f;

    lightManager->AddDirectionalLight(directionalLight);
    lightManager->AddPointLight(pointLight);
    lightManager->AddPointLight(pointLight2);
    lightManager->AddSpotLight(spotLight);

    lightManager->SetupLights(ourShader);

}

void Scene::Run() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ProcessInput();

    frameBuffer->BindToFrameBuffer();

    // Enable depth testing for the entire frame by default.
    OpenGLConfigurations::EnableDepthTesting();

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

    // 

    Ref<LightComponent> mySpotLight = lightManager->GetSpotLight(0);
    if (mySpotLight)
    {
        mySpotLight->position = camera.Position;
        mySpotLight->direction = camera.Front;
    }
    lightManager->Render();

    // draw skyBox
    skybox.Draw(view, projection);

    float rotationSpeed = 200.0f;
    float floatingSpeed = 5.0f;
    float bendMulti = 6.0f;

    ourShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, static_cast<float>(glm::radians(glfwGetTime() * rotationSpeed)), glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::rotate(model, static_cast<float>(glm::radians(glm::sin(glfwGetTime() * floatingSpeed) * bendMulti)), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    ourShader->setMat4("model", model);

    ourModel->Draw(*ourShader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
    ourShader->setMat4("model", model);
    groundModel->Draw(*ourShader);

    OpenGLConfigurations::DisableFaceCulling();


    lightManager->DrawLights(view, projection);

    OpenGLConfigurations::EnableFaceCulling();
    // Render ScreenQuad
    screenShader->use();
    screenShader->setInt("screenTexture", 0);
    frameBuffer->BindToTexture();
    quadVertexArray->Bind();

    OpenglRenderer::DrawIndexed(quadVertexArray);

}

void Scene::MouseCallback(double xposIn, double yposIn)
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


void Scene::ProcessInput()
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