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
Scene::~Scene() 
{
    Window::GetInstance().SetCursorPosCallback([](double, double) {});

}
void Scene::Init() {

    // Start with mouse-look OFF, or set m_MouseLook=true if you want it ON by default.
    ApplyMouseLook(m_MouseLook);

    // Install the callback once; guard it so it only runs when we want mouse-look
    Window::GetInstance().SetCursorPosCallback([this](double xpos, double ypos) {
        if (!this->m_MouseLook) return;                 // toggle controls it
        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureMouse) return;          // let ImGui consume UI interactions
        this->MouseCallback(xpos, ypos);
        });

    OpenGLConfigurations::EnableFaceCulling(); 
    OpenGLConfigurations::SetFaceCullingMode(FaceCullMode::FRONT);
    OpenGLConfigurations::SetWindingOrder(WindingOder::CLOCKWISE);
    

    ourShader = std::make_shared<Shader>("Shaders/lights_Vertex_Shader.glsl", "Shaders/MultipleLights_Fragment.glsl");
    screenShader = std::make_shared<Shader>("Shaders/framebuffers_screen_Vertex.glsl", "Shaders/framebuffers_screen_Fragment.glsl");
    depthScreenShader = std::make_shared<Shader>("Shaders/ShadowMap/quad_shadowMap_Vertex.glsl", "Shaders/ShadowMap/quad_shadowMap_Fragment.glsl");
    depthShader = std::make_shared<Shader>("Shaders/ShadowMap/shadowMap_depth_Vertex.glsl", "Shaders/ShadowMap/shadowMap_depth_Fragment.glsl");


    ourModel = std::make_shared<Model>("Resources/objects/medievalCastle/medievalCastle.obj", true, false);
    groundModel = std::make_shared<Model>("Resources/objects/SimpleGround/Ground.obj", true, false);
    
    frameBuffer = std::make_shared<FrameBuffer>(EngineConstants::SCR_WIDTH,EngineConstants::SCR_HEIGHT,false,true);
    shadowMap = std::make_shared<FrameBuffer>(4096, 4096, true, false);

    uniformBuffer = std::make_shared<UniformBuffer>(2 * sizeof(glm::mat4));

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

    quadVertexArray = ShapeGenerator::GenerateQuad(0,0,1,1);

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

    // Send data blocking index 0 for all shaders
    uniformBuffer->BindBufferRange(0, 0, 2 * sizeof(glm::mat4));


    ourShader->use();
    ourShader->setFloat("material.shininess", 64.0f);
    ourShader->setInt("shadowMap", 0);

    //screenShader->use();
    //screenShader->setInt("screenTexture", 0);

    depthScreenShader->use();
    depthScreenShader->setInt("depthMap", 0);

    //ourShader->use();
    //ourShader->setInt("shadowMap", 0);
    OpenGLConfigurations::EnableFaceCulling();
    OpenGLConfigurations::SetWindingOrder(WindingOder::ANTICLOCKWISE);
    OpenGLConfigurations::SetFaceCullingMode(FaceCullMode::BACK);
}
glm::vec3 lightPos(-4.3f, 11.7f, 3.30f);

void Scene::Run() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    ProcessInput();

    if (ImGui::Begin("Input / Camera")) {
        bool v = m_MouseLook;
        if (ImGui::Checkbox("Mouse Look (F1)", &v)) {
            m_MouseLook = v;
            ApplyMouseLook(m_MouseLook);
        }

        if (ImGui::IsKeyPressed(ImGuiKey_F1, false)) {
            m_MouseLook = !m_MouseLook;
            ApplyMouseLook(m_MouseLook);
        }
    }
    ImGui::End();
    ImGui::Begin("Light Settings");
    ImGui::DragFloat3("Light Position", &lightPos[0], 0.1f);
    ImGui::End();


    // Enable depth testing for the entire frame by default.
    OpenGLConfigurations::EnableDepthTesting();

    OpenGLConfigurations::SetDepthFunction(DepthMode::LESS);// change depth function so depth test passes when values are equal to depth buffer's content

    //Pass 1 // render to depth buffer
    OpenGLConfigurations::DisableFaceCulling();
    glEnable(GL_POLYGON_OFFSET_FILL); glPolygonOffset(2.0f, 2.0f);
    auto directionalLight = lightManager->GetDirectionalLight(0);

    glm::mat4 lightProjection, lightView;
    float near_plane = 0.1f, far_plane = 100.0f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    //lightProjection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(EngineConstants::SCR_WIDTH) / static_cast<float>(EngineConstants::SCR_HEIGHT), 0.1f, 100.0f);
    lightView = glm::lookAt(lightPos,
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    
    shadowMap->BindToFrameBuffer();

    renderScene(depthShader, lightProjection, lightView);

    shadowMap->UnBind();
    OpenGLConfigurations::EnableFaceCulling();
    glDisable(GL_POLYGON_OFFSET_FILL);

    ////Pass 2
    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Lighting Pass");

    frameBuffer->BindToFrameBuffer();  // sets viewport & clears color/depth/stencil

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;

    projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(EngineConstants::SCR_WIDTH) / static_cast<float>(EngineConstants::SCR_HEIGHT), 0.1f, 100.0f);
    //camera.ProcessMouseMovement(0, 0, false);
    //camera.Yaw += 180.0f;
    view = camera.GetViewMatrix();
    depthScreenShader->use();

    depthScreenShader->setFloat("near_plane", 0.1f);
    depthScreenShader->setFloat("far_plane", 100.0f);

    Ref<LightComponent> mySpotLight = lightManager->GetSpotLight(0);
    if (mySpotLight)
    {
        mySpotLight->position = camera.Position;
        mySpotLight->direction = camera.Front;
    }
    lightManager->Render();

    ourShader->use();
    ourShader->setVec3("lightPos", lightPos);

    ourShader->setVec3("viewPos", camera.Position);
    ourShader->setMat4("lightSpaceMatrix", lightProjection * lightView);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadowMap->GetTextureID());
    ourShader->setInt("shadowMap", 1);
    glObjectLabel(GL_TEXTURE, shadowMap->GetTextureID(), -1, "ShadowMapDepth");

    renderScene(ourShader,projection, view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
    ourShader->setMat4("model", model);
    groundModel->Draw(*ourShader);
    // draw skyBox

    //OpenGLConfigurations::SetDepthFunction(DepthMode::LESS_EQUAL);
    //skybox.Draw();
    //OpenGLConfigurations::SetDepthFunction(DepthMode::LESS);

    OpenGLConfigurations::DisableFaceCulling();

    lightManager->DrawLights();
    OpenGLConfigurations::EnableFaceCulling();
    //glPopDebugGroup();

    // Render ScreenQuad
    //depthScreenShader->use();
    //shadowMap->BindToTexture();
    
    
    screenShader->use();
    frameBuffer->BindToTexture();
    screenShader->use();
    screenShader->setInt("screenTexture", 0);

    OpenGLConfigurations::DisableDepthTesting(); // for rendering quad on screen always
    quadVertexArray->Bind();
    OpenglRenderer::DrawIndexed(quadVertexArray);

}
void Scene::ApplyMouseLook(bool enabled)
{
    // false => show/free cursor, true => hide/lock cursor
    Window::GetInstance().SetWindowCursor(!enabled);
    firstMouse = true; // reset deltas so the first move doesn’t jump
}
void Scene::renderScene(Ref<Shader> shader, glm::mat4& projection, glm::mat4& view)
{
    uniformBuffer->SetBufferSubData(0, sizeof(glm::mat4), glm::value_ptr(projection));
    uniformBuffer->SetBufferSubData(sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

    float rotationSpeed = 200.0f;
    float floatingSpeed = 5.0f;
    float bendMulti = 6.0f;

    shader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, static_cast<float>(glm::radians(glfwGetTime() * rotationSpeed)), glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::rotate(model, static_cast<float>(glm::radians(glm::sin(glfwGetTime() * floatingSpeed) * bendMulti)), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    shader->setMat4("model", model);

    ourModel->Draw(*shader);

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