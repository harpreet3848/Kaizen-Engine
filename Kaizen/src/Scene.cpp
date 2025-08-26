#include "Scene.h"
#include "OpenGl/OpenGlConfigurations.h"
#include "OpenGl/OpenGLRenderer.h"
#include "OpenGl/ShapeGenerator.h"
#include <imgui.h>
#include <array>  

Scene::Scene() : camera(glm::vec3(0.0f, 0.0f, 3.0f)),
                lastX(static_cast<float>(EngineConstants::SCR_WIDTH) / 2.0f),
                lastY(static_cast<float>(EngineConstants::SCR_HEIGHT) / 2.0f),
                firstMouse(true),
                deltaTime(0.0f),
                lastFrame(0.0f)
{}

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

  /*  OpenGLConfigurations::EnableFaceCulling(); 
    OpenGLConfigurations::SetFaceCullingMode(FaceCullMode::FRONT);
    OpenGLConfigurations::SetWindingOrder(WindingOder::ANTICLOCKWISE)*/;
    

    ourShader = std::make_shared<Shader>("Shaders/lights_Vertex_Shader.glsl", "Shaders/MultipleLights_Fragment.glsl");
    screenShader = std::make_shared<Shader>("Shaders/Post_Screen_Vertex.glsl", "Shaders/Post_Screen_Fragment.glsl");
    depthScreenShader = std::make_shared<Shader>("Shaders/ShadowMap/quad_shadowMap_Vertex.glsl", "Shaders/ShadowMap/quad_shadowMap_Fragment.glsl");
    depthShader = std::make_shared<Shader>("Shaders/ShadowMap/shadowMap_depth_Vertex.glsl", "Shaders/ShadowMap/shadowMap_depth_Fragment.glsl");
    pointShadowMapShader = std::make_shared<Shader>("Shaders/ShadowMap/PointShadowGeometry/point_Shadow_Vertex.glsl", 
                                                    "Shaders/ShadowMap/PointShadowGeometry/point_Shadow_Fragment.glsl", 
                                                    "Shaders/ShadowMap/PointShadowGeometry/point_Shadow_Geometry.glsl");

    ourModel = std::make_shared<Model>("Resources/objects/medievalCastle/medievalCastle.obj", true, false);
    groundModel = std::make_shared<Model>("Resources/objects/SimpleGround/Ground.obj", true, false);
    
    screenFrameBuffer = std::make_shared<FrameBuffer>(EngineConstants::SCR_WIDTH,EngineConstants::SCR_HEIGHT,false,true,true,2);
    dirShadowMap = std::make_shared<FrameBuffer>(4096, 4096, true, false);
    spotShadowMap = std::make_shared<FrameBuffer>(4096, 4096, true, false);

    postProcessing = std::make_shared<PostProcessingFX>();
    
    pointShadowMaps = std::make_shared<PointShadowMap>(4096, 4096);

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
    smallQuadVertexArray = ShapeGenerator::GenerateQuad(-0.75f, -0.75f, 0.25f, 0.25f);

    lightManager = std::make_shared<LightManager>();
    {
        auto directionalLight = std::make_shared<LightComponent>();
        directionalLight->type = LightType::Directional;
        directionalLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
        directionalLight->direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        directionalLight->ambientIntensity = 0.05f;
        directionalLight->diffuseIntensity = 1.0f;
        directionalLight->specularIntensity = 1.0f;

        auto pointLight = std::make_shared<LightComponent>();
        pointLight->type = LightType::Point;
        pointLight->color = glm::vec3(1.0f, 1.0f, 1.0f); // Red light
        pointLight->position = glm::vec3((-2.0f, 0.2f, 1.0f));
        pointLight->ambientIntensity = 0.05f;
        pointLight->diffuseIntensity = 0.8f;
        pointLight->specularIntensity = 1.0f;
        pointLight->constant = 1.0f;
        pointLight->linear = 0.09f;
        pointLight->quadratic = 0.032f;

        auto spotLight = std::make_shared<LightComponent>();
        spotLight->type = LightType::Spot;
        spotLight->color = glm::vec3(0.0f, 1.0f, 0.0f); // Green light
        spotLight->position = glm::vec3(0.0f, 4.0f, 4.0f);
        spotLight->direction = glm::vec3(0.0f, -0.7f, -1.0f);
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
        lightManager->AddSpotLight(spotLight);

        lightManager->SetupLights(ourShader);
    }

    // Send data blocking index 0 for all shaders
    uniformBuffer->BindBufferRange(0, 0, 2 * sizeof(glm::mat4));

    ourShader->use();
    ourShader->setFloat("material.shininess", 64.0f);
    ourShader->setInt("shadowMap", 0);

    //ourShader->use();
    //ourShader->setInt("shadowMap", 0);
}

void Scene::Run() 
{
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    OpenglRenderer::ClearColor();
    OpenglRenderer::ClearColourBuffer();
    OpenglRenderer::ClearDepthBuffer();

    OpenglRenderer::ClearStencilBuffer();

    OpenGLConfigurations::EnableDepthTesting();

    OpenGLConfigurations::SetDepthFunction(DepthMode::LESS);

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


    static float exposure = 1.0f;
    static bool bloom = false;

    ImGui::Begin("Environment");
    
    ImGui::DragFloat("Exposure", &exposure, 0.1f, 0.0f, 30.0f, "%.1f");

    ImGui::Checkbox("Bloom", &bloom);

    ImGui::End();


    screenShader->use();
    screenShader->setFloat("exposure", exposure);
    screenShader->setInt("bloom", bloom);
    OpenGLConfigurations::DisableFaceCulling();

    //Point Light Pass
        // 0. create depth cubemap transformation matrices
        // -----------------------------------------------
    auto pointLight = lightManager->GetPointLight(0);

    glm::vec3 pointLightPos = pointLight->position;
    float aspect = (float)4098 / (float)4098;
    float near = 0.1f;
    float pointLightFarPlane = 25.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, near, pointLightFarPlane);
    
    std::array<glm::mat4, 6> shadowTransforms = {
        shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)),
        shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(-1.0,  0.0,  0.0), glm::vec3(0.0, -1.0,  0.0)),
        shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0,  1.0,  0.0), glm::vec3(0.0,  0.0,  1.0)),
        shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0, -1.0,  0.0), glm::vec3(0.0,  0.0, -1.0)),
        shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0,  0.0,  1.0), glm::vec3(0.0, -1.0,  0.0)),
        shadowProj * glm::lookAt(pointLightPos, pointLightPos + glm::vec3(0.0,  0.0, -1.0), glm::vec3(0.0, -1.0,  0.0))
    };
        // 1. render scene to depth cubemap
        // --------------------------------

    pointShadowMaps->BindLayeredForWrite();
    pointShadowMapShader->use();

    for (unsigned int i = 0; i < 6; ++i)
        pointShadowMapShader->setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    
    pointShadowMapShader->setFloat("far_plane", pointLightFarPlane);
    pointShadowMapShader->setVec3("lightPos", pointLightPos);
    glm::mat4 emptyprojection = glm::mat4(1.0f);
    glm::mat4 emptyview = glm::mat4(1.0f);
    renderScene(pointShadowMapShader, emptyprojection, emptyview);

    //pointShadowMaps->Unbind();

    //Directional Light Pass
    // render to depth buffer

    auto directionalLight = lightManager->GetDirectionalLight(0);
    glm::vec3 shadowCamPos = -directionalLight->direction * 20.f;

    glm::mat4 lightProjection, lightView;
    float near_plane = 0.1f, far_plane = 30.0f;
    lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    //lightProjection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(EngineConstants::SCR_WIDTH) / static_cast<float>(EngineConstants::SCR_HEIGHT), 0.1f, 100.0f);
    lightView = glm::lookAt(shadowCamPos,
                        glm::vec3(0.0f, 0.0f, 0.0f),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    
    dirShadowMap->BindToFrameBuffer(); 

    renderScene(depthShader, lightProjection, lightView);

    //dirShadowMap->UnBind();

    //Spot Light Pass
    //render to depth buffer

    Ref<LightComponent> mySpotLight = lightManager->GetSpotLight(0);
    float fovDeg = glm::max(2.0f * mySpotLight->outerCutOff, 1.0f);
    glm::mat4 spotProj = glm::perspective(glm::radians(fovDeg), 1.0f, 0.1f, 100.0f);
    glm::mat4 spotView = glm::lookAt(
        mySpotLight->position,
        mySpotLight->position + glm::normalize(mySpotLight->direction),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    spotShadowMap->BindToFrameBuffer();
    renderScene(depthShader, spotProj, spotView);
    //spotShadowMap->UnBind();

    // Save for lighting pass
    glm::mat4 spotLightSpace = spotProj * spotView;

    OpenGLConfigurations::EnableFaceCulling();

    //Render scene as normal

    //glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 2, -1, "Lighting Pass");

    screenFrameBuffer->BindToFrameBuffer();  // sets viewport & clears color/depth/stencil

    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(EngineConstants::SCR_WIDTH) / static_cast<float>(EngineConstants::SCR_HEIGHT), 0.1f, 100.0f);
    //camera.ProcessMouseMovement(0, 0, false);
    //camera.Yaw += 180.0f;
    view = camera.GetViewMatrix();
    depthScreenShader->use();

    depthScreenShader->setFloat("near_plane", 0.1f);
    depthScreenShader->setFloat("far_plane", 100.0f);

    lightManager->Render();

    ourShader->use();

    ourShader->setVec3("viewPos", camera.Position);
    //ourShader->setVec3("lightPos", pointLightPos);

    ourShader->setMat4("dirLightSpaceMatrix", lightProjection * lightView);
    ourShader->setMat4("spotLightSpaceMatrix", spotLightSpace);

    ourShader->setFloat("pointLightFarPlane", pointLightFarPlane);

    constexpr uint32_t SHADOWDIRMAP_TEX_UNIT = 3;
    dirShadowMap->BindToTexture(SHADOWDIRMAP_TEX_UNIT);
    ourShader->setInt("directionalShadowMap", SHADOWDIRMAP_TEX_UNIT);

    constexpr uint32_t SHADOWSPOTMAP_TEX_UNIT = 4;
    spotShadowMap->BindToTexture(SHADOWSPOTMAP_TEX_UNIT);
    ourShader->setInt("spotShadowMap[0]", SHADOWSPOTMAP_TEX_UNIT);

    constexpr uint32_t SHADOWPOINTMAP_TEX_UNIT = 8;
    pointShadowMaps->BindToTexture(SHADOWPOINTMAP_TEX_UNIT);
    ourShader->setInt("pointShadowMap[0]", SHADOWPOINTMAP_TEX_UNIT);

    renderScene(ourShader,projection, view);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
    ourShader->setMat4("model", model);
    groundModel->Draw(*ourShader);

    // draw skyBox
    OpenGLConfigurations::SetDepthFunction(DepthMode::LESS_EQUAL);
    skybox.Draw();
    OpenGLConfigurations::SetDepthFunction(DepthMode::LESS);

    OpenGLConfigurations::DisableFaceCulling();
    lightManager->DrawLights();
    OpenGLConfigurations::EnableFaceCulling();
    

    constexpr uint32_t BRIGHTCOLOR_ATTACHMENT = 1;
    Ref<FrameBuffer> blurFrameBuffer = postProcessing->Blur(screenFrameBuffer, BRIGHTCOLOR_ATTACHMENT);
    
    screenFrameBuffer->UnBind();

    // Render ScreenQuad
    constexpr uint32_t SCREENQUAD_UNIT = 0;
    screenShader->use();

    screenFrameBuffer->BindToTexture(SCREENQUAD_UNIT);
    screenShader->setInt("screenTexture", SCREENQUAD_UNIT);
    
    blurFrameBuffer->BindToTexture(1);
    screenShader->setInt("blurTexture", 1);

    OpenGLConfigurations::DisableDepthTesting(); // for rendering quad on screen always
    quadVertexArray->Bind();
    OpenglRenderer::DrawIndexed(quadVertexArray);

    depthScreenShader->use();
    spotShadowMap->BindToTexture(SCREENQUAD_UNIT);
    depthScreenShader->setInt("depthMap", SCREENQUAD_UNIT);

    OpenGLConfigurations::DisableDepthTesting(); // for rendering quad on screen always
    smallQuadVertexArray->Bind();
    OpenglRenderer::DrawIndexed(smallQuadVertexArray);
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
    model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
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