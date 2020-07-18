#include "Engine.h"
#include "App.h"
#include "GenericThrowMacros.h"

int main()
{
    try
    {
        Engine::Initialize(Engine::Graphics);
        App app;
        app.Run();
    }
    LogOnly;
}