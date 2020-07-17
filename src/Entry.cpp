#include "Engine.h"
#include "Window.h"

int main()
{
    Engine::Init(Engine::Graphics);
    Window window("Saffron Engine", 100, 500);

    while (true);
}