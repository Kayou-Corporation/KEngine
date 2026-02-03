#include <iostream>

#include "Window/Window.hpp"

int main()
{
    RefCountPtr<Window> window = WindowInterface::InitWindow(WindowAPI::SDL);

    WindowSpecs specs;
    specs.width = 720;
    specs.height = 480;
    specs.name = "KEngine";
    specs.allowResize = true;

    window->Create(specs);

    while (!window->ShouldClose())
    {
        window->PollEvents();
    }
    
    window->Destroy();

    return 0;
}