#pragma once

#include <chrono>

class Timer
{
public:
    Timer() noexcept;

    static void UpdateGlobalTimer(float dt);

    float Mark() noexcept;
    [[nodiscard]] float Peek() const noexcept;
    static float PeekGlobal() noexcept;
private:
    std::chrono::steady_clock::time_point m_last;
    static float m_globalTimer;
};


