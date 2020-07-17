#pragma once

class Engine
{
public:
    Engine() = delete;
    ~Engine() = delete;
    Engine(Engine&) = delete;
    Engine& operator()(Engine&) = delete;

    enum Subsystem
    {
        Graphics = 0
    };
    static void Init(Subsystem subsystem);
};

