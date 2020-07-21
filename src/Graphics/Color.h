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
    unsigned int dword;
public:
    constexpr Color() noexcept;
    constexpr Color(const Color &col) noexcept;
    constexpr explicit Color(unsigned int dw) noexcept;
    constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) noexcept;
    constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept;
    constexpr Color(Color col, unsigned char x) noexcept;
    Color &operator=(Color color) noexcept;

    [[nodiscard]] constexpr unsigned char GetX() const noexcept;
    [[nodiscard]] constexpr unsigned char GetA() const noexcept;
    [[nodiscard]] constexpr unsigned char GetR() const noexcept;
    [[nodiscard]] constexpr unsigned char GetG() const noexcept;
    [[nodiscard]] constexpr unsigned char GetB() const noexcept;
    void SetX(unsigned char x) noexcept;
    void SetA(unsigned char a) noexcept;
    void SetR(unsigned char r) noexcept;
    void SetG(unsigned char g) noexcept;
    void SetB(unsigned char b) noexcept;
};