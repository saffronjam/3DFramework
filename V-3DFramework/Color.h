#pragma once

class Color
{
public:
	unsigned int dword;
public:
	constexpr Color() noexcept : dword()
	{
	}
	constexpr Color(const Color& col) noexcept
		:
		dword(col.dword)
	{
	}
	constexpr Color(unsigned int dw) noexcept
		:
		dword(dw)
	{
	}
	constexpr Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) noexcept
		:
		dword((r << 24u) | (g << 16u) | (b << 8u) | a)
	{
	}
	constexpr Color(unsigned char r, unsigned char g, unsigned char b) noexcept
		:
		dword((r << 24u) | (g << 16u) | (b << 8u) | 255u)
	{
	}
	constexpr Color(Color col, unsigned char a) noexcept
		:
		Color(a | col.dword)
	{
	}
	Color& operator =(Color color) noexcept
	{
		dword = color.dword;
		return *this;
	}

	constexpr unsigned char GetR() const noexcept { return dword >> 24u; }
	constexpr unsigned char GetG() const noexcept { return (dword >> 16u) & 0xFFu; }
	constexpr unsigned char GetB() const noexcept { return (dword >> 8u) & 0xFFu; }
	constexpr unsigned char GetA() const noexcept { return dword & 0xFFu; }

	void SetR(unsigned char r) noexcept { dword = (dword & 0xFFFFFFu) | (r << 24u); }
	void SetG(unsigned char g) noexcept { dword = (dword & 0xFF00FFFFu) | (g << 16u); }
	void SetB(unsigned char b) noexcept { dword = (dword & 0xFFFF00FFu) | (b << 8u); }
	void SetA(unsigned char a) noexcept { dword = (dword & 0xFFFFFF00u) | a; }
};
