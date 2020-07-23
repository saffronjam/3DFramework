#pragma once

struct BGRAColor
{
    unsigned char a;
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

class Color
{
public:
    explicit Color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f);

public:
    float r, g, b, a;
};