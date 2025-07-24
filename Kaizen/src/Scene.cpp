#include "Scene.h"

Scene::Scene() : camera(glm::vec3(0.0f, 0.0f, 3.0f)),
                lastX(static_cast<float>(EngineConstants::SCR_WIDTH) / 2.0f),
                lastY(static_cast<float>(EngineConstants::SCR_HEIGHT) / 2.0f),
                firstMouse(true),
                deltaTime(0.0f),
                lastFrame(0.0f)
{
    Window::GetInstance().SetWindowCursor(false);
    Window::GetInstance().SetCursorPosCallback([this](double xpos, double ypos) {
        this->mouse_callback(xpos, ypos);
        });
}

void Scene::Init() {
    float vertices[] = {
        // positions         // textureCoord     // colors          
         0.5f,  0.5f, 0.0f,   1.0f, 1.0f,     1.0f, 0.0f, 0.0f, // top right
         0.5f, -0.5f, 0.0f,   1.0f, 0.0f,     0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f,     0.0f, 0.0f, 1.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   0.0f, 1.0f,      1.0f, 1.0f, 0.0f // top left 
    };
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
    };
    float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };
    float skyCubeVertices[] = {
        // positions        
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };
    float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    stbi_set_flip_vertically_on_load(true);

    ourModel = std::make_unique<Model>("Resources/objects/backpack/backpack.obj", true);

    ourShader = std::make_unique<Shader>("shaders/Default_Vertex.glsl", "shaders/MultipleLights_Fragment.glsl");
    lightCubeShader = std::make_unique<Shader>("shaders/Default_Vertex.glsl", "shaders/lightColor_Fragment.glsl");
    shaderSingleColor = std::make_unique<Shader>("shaders/Default_Vertex.glsl", "shaders/stencil_single_color.glsl");
    screenShader = std::make_unique<Shader>("shaders/framebuffers_screen_Vertex.glsl", "shaders/framebuffers_screen_Fragment.glsl");
    skyboxShader = std::make_unique<Shader>("shaders/skybox_Vertex.glsl", "shaders/skybox_fragment.glsl");

    auto skyVertexBuffer = std::make_shared<VertexBuffer>(skyCubeVertices, sizeof(skyCubeVertices));
    const BufferLayout skyBufferlayout = {
     {ShaderDataType::Float3, "a_Position",false },
    };

    skyVertexBuffer->SetLayout(skyBufferlayout);

    auto vertexBuffer = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));

    const BufferLayout bufferlayout = {
         {ShaderDataType::Float2, "a_Position",false },
         { ShaderDataType::Float2, "a_TexCoords",false }
    };

    vertexBuffer->SetLayout(bufferlayout);

    vertexArray.AddVertexBuffer(skyVertexBuffer);
    vertexArray.AddVertexBuffer(vertexBuffer);

    vertexArray.Bind();

    // Default cube VertexBuffer
    auto vertexBufferCube = std::make_shared<VertexBuffer>(cubeVertices, sizeof(cubeVertices));

    const BufferLayout BufferLayoutCube = {
         {ShaderDataType::Float3, "a_Position",false },
         { ShaderDataType::Float3, "a_Noraml",false },
         { ShaderDataType::Float2, "a_TexCoords",false }
    };

    vertexBufferCube->SetLayout(BufferLayoutCube);

    // Create the vector of strings by calling .string() on each path
    std::vector<std::string> faceFilepaths
    {
        std::filesystem::path("Resources/skybox/right.jpg").string(),
        std::filesystem::path("Resources/skybox/left.jpg").string(),
        std::filesystem::path("Resources/skybox/top.jpg").string(),
        std::filesystem::path("Resources/skybox/bottom.jpg").string(),
        std::filesystem::path("Resources/skybox/front.jpg").string(),
        std::filesystem::path("Resources/skybox/back.jpg").string()
    };


    cubeMap = std::make_unique<Cubemap>(faceFilepaths, true);
    // Light VertexArray
    lightVAO.AddVertexBuffer(vertexBufferCube);
    lightVAO.Bind();


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CW);

    skyboxShader->use();
    skyboxShader->setInt("skybox", 0);

    //Texture containerTexture("Resources/container2.png");
    //// ---------
    //Texture containerSpecular("Resources/container2_specular.png");

    //ourShader.use();
    //ourShader.setInt("texture1", 0);
    //ourShader.setInt("texture2", 1);
    //ourShader.use();
    //ourShader.setInt("material.diffuse", 0);
    //ourShader.setInt("material.specular", 1);
}

void Scene::Run() {
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput();

    frameBuffer.BindToFrameBuffer();

    // Enable depth testing for the entire frame by default.
    glEnable(GL_DEPTH_TEST);
    // Set the stencil operation. This will be used when we draw the object to be outlined.
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);


    Window::GetInstance().ClearColor();
    Window::GetInstance().ClearAllBuffer();


    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection;

    projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(EngineConstants::SCR_WIDTH)
        / static_cast<float>(EngineConstants::SCR_HEIGHT), 0.1f, 100.0f);
    //camera.ProcessMouseMovement(0, 0, false);
    //camera.Yaw += 180.0f;
    view = camera.GetViewMatrix();

    ourShader->use();
    ourShader->setVec3("viewPos", camera.Position);
    ourShader->setFloat("material.shininess", 64.0f);


    ourShader->setMat4("view", view);
    ourShader->setMat4("projection", projection);

    // directional light
    ourShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    ourShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    ourShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    ourShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
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

    glStencilFunc(GL_ALWAYS, 1, 0xFF); // The stencil test should always pass.
    glStencilMask(0xFF); // Enable writing to the stencil buffer.

    // Pass 1
    ourShader->use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
    ourShader->setMat4("model", model);


    ourModel->Draw(*ourShader);
    //glDrawArrays(GL_TRIANGLES, 0, 36);


    // Pass 2
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    float scaleFactor = 1.02f;
    shaderSingleColor->use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

    shaderSingleColor->setMat4("model", model);
    ourModel->Draw(*shaderSingleColor);

    //glDrawArrays(GL_TRIANGLES, 0, 36);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);

    // also draw the lamp object(s)
    lightCubeShader->use();
    lightCubeShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    lightCubeShader->setMat4("projection", projection);
    lightCubeShader->setMat4("view", view);

    lightVAO.Bind();

    // we now draw as many light bulbs as we have point lights.
    for (unsigned int i = 0; i < 2; i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        lightCubeShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Render sybox
    glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
    glDisable(GL_CULL_FACE);
    skyboxShader->use();
    skyboxShader->setMat4("view", view);
    skyboxShader->setMat4("projection", projection);
    cubeMap->Bind(0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // set depth function back to default
    glEnable(GL_CULL_FACE);

    // Render ScreenQuad
    screenShader->use();
    screenShader->setInt("screenTexture", 0);
    glEnable(GL_CULL_FACE);
    frameBuffer.BindToTexture(true, false);
    vertexArray.Bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
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