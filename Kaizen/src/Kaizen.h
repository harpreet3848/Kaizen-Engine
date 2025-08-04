#include <iostream>
#include "Core/Window.h"
#include "OpenGl/OpenGLDebugger.h"
#include "Scene.h"

#include "Core/OpenGLImGui.h"


class Kaizen {
public:
	void Run() 
	{
        Window::GetInstance().Init();

        OpenGLDebugger::Init();

        OpenGLImGui::Setup();

        Scene myScene;
        myScene.Init();

        while (!Window::GetInstance().IsWindowClosed()) 
        {
            Window::GetInstance().PollEvents();

            OpenGLImGui::Begin();

            myScene.Run();

            OpenGLImGui::End();

            Window::GetInstance().SwapBuffers();
        }
        OpenGLImGui::ShutDown();
	}
};