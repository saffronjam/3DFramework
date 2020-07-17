#include "Engine.h"
#include "Window.h"

int main()
{
    Engine::Initialize(Engine::Graphics);
    Window window("Saffron Engine", 1024, 720);

    while (true);

    Engine::Uninitialize(Engine::Graphics);
}