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
#include "OpenGl/Cubemap.h"
#include "Input.h"
#include "KeyCodes.h"


class Scene {
public:
    // Constructor to initialize member variables in cpp
    Scene();
    void Init();

    void Run();

private:

    std::unique_ptr<Model> ourModel;
    std::unique_ptr<Shader> ourShader;
    std::unique_ptr<Shader> lightCubeShader;
    std::unique_ptr<Shader> shaderSingleColor;
    std::unique_ptr<Shader> screenShader;
    std::unique_ptr<Shader> skyboxShader;
    std::unique_ptr<Cubemap> cubeMap;

    VertexArray vertexArray;
    VertexArray lightVAO;
    FrameBuffer frameBuffer;


    // Camera and Timing
    Camera camera;
    float lastX;
    float lastY;
    bool firstMouse;
    float deltaTime;
    float lastFrame;

    // positions of the point lights
    glm::vec3 pointLightPositions[2] = {
          glm::vec3(0.7f,  0.2f,  2.0f),
          glm::vec3(2.3f, -3.3f, -4.0f),
    };

    // glfw: whenever the mouse moves, this callback is called
    void mouse_callback(double xposIn, double yposIn);
    void processInput();
};

