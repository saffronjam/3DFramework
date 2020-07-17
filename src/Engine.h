#pragma once

#include <functional>

typedef void(* GLFWerrorfun) (int, const char *);

class Engine
{
public:
    Engine() = delete;
    ~Engine() = delete;
    Engine(Engine &) = delete;
    Engine &operator()(Engine &) = delete;

    enum Subsystem
    {
        Graphics = 0
    };

    static void Initialize(Subsystem subsystem);
    static void Uninitialize(Subsystem subsystem);

    static void BindErrorCallback(GLFWerrorfun callback);
    static void UnbindErrorCallback();
};

