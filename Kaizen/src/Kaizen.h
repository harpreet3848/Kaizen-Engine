#include <iostream>
#include "Core/Window.h"
#include "OpenGl/OpenGLDebugger.h"
#include "Scene.h"

class Kaizen {
public:
	void Run() 
	{
        Window::GetInstance().Init();
        OpenGLDebugger::Init();

        Scene myScene;
        myScene.Init();

        while (!Window::GetInstance().IsWindowClosed()) {

            myScene.Run();

            Window::GetInstance().UpdateEvents();
        }
	}
};