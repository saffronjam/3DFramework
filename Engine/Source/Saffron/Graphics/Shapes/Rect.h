﻿#pragma once

#include "Saffron/Base.h"

namespace Se
{
template <typename T>
struct Rect
{
public:
	T x, y, Width, Height;

	auto Contains(T ix, T iy) const noexcept -> bool;
	auto Contains(const Vector2& point) const noexcept -> bool;
	auto Contains(const Rect<T>& r) const noexcept -> bool;

	auto Min() const -> Vector2;
	auto Max() const -> Vector2;

	auto Expand(T x, T y) -> Rect&;
	auto Offset(T x, T y) -> Rect&;
};

template <typename T>
auto Rect<T>::Contains(T ix, T iy) const noexcept -> bool
{
	return x <= ix && ix < x + Width && y <= iy && iy < y + Height;
}

template <typename T>
auto Rect<T>::Contains(const Vector2& point) const noexcept -> bool
{
	return static_cast<float>(x) <= point.x && point.x < static_cast<float>(x + Width) && static_cast<float>(y) <= point
		.y && point.y < static_cast<float>(y + Height);
}

template <typename T>
auto Rect<T>::Contains(const Rect<T>& r) const noexcept -> bool
{
	return x <= r.x && r.x + r.Width <= x + Width && y <= r.y && r.y + r.Height <= y + Height;
}

template <typename T>
auto Rect<T>::Min() const -> Vector2
{
	return {x, y};
}

template <typename T>
auto Rect<T>::Max() const -> Vector2
{
	return {x + Width, y + Height};
}

template <typename T>
auto Rect<T>::Expand(T x, T y) -> Rect&
{
	this->x -= x;
	this->y -= y;
	this->Width += x * static_cast<T>(2);
	this->Height += y * static_cast<T>(2);

	return *this;
}

template <typename T>
auto Rect<T>::Offset(T x, T y) -> Rect&
{
	this->x += x;
	this->y += y;
	return *this;
}


using FloatRect = Rect<float>;
using IntRect = Rect<int>;
}
