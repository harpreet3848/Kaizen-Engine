#pragma once

#include <iostream>
#include <algorithm>
#include <filesystem>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Core.h"

#include "OpenGl/VertexArray.h"
#include "OpenGl/VertexBuffer.h"

#include "OpenGl/Shader.h"
#include "OpenGl/Texture.h"

#include "Core/Window.h"

#include <stb_image.h>
#include "Core/Model.h"
#include "Core/Camera.h"
#include "Core/EngineConstansts.h"
#include "OpenGl/FrameBuffer.h"
#include "Input.h"
#include "KeyCodes.h"
#include "Core/Skybox.h"
#include "Core/LightBindings.h"
#include "Core/LightManager.h"
#include <OpenGl/UniformBuffer.h>


class Scene {
public:
    Scene();
    void Init();

    void Run();

private:

    Ref<Model> groundModel;
    Ref<Model> ourModel;
    Ref<Shader> ourShader;
    Ref<Shader> screenShader;
    Ref<Shader> depthShader;

    Ref<FrameBuffer> frameBuffer;
    Ref<FrameBuffer> shadowMap;


    Ref<VertexArray> quadVertexArray;

    Ref<LightManager> lightManager;

    Ref<UniformBuffer> uniformBuffer;

    Skybox skybox;

    // Camera and Timing
    Camera camera;
    float lastX;
    float lastY;
    bool firstMouse;
    float deltaTime;
    float lastFrame;

    // positions of the point lights
    glm::vec3 pointLightPositions[1] = {
          glm::vec3(-2.0f,  0.2f,  1.0f),
    };

    // glfw: whenever the mouse moves, this callback is called
    void MouseCallback(double xposIn, double yposIn);
    void ProcessInput();
};

