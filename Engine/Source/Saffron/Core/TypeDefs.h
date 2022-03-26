#pragma once

#include <cstdint>

#include "wrl/client.h"

namespace Se
{
using uchar = std::uint8_t;
using ushort = std::uint16_t;
using uint = std::uint32_t;
using ulong = std::uint64_t;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
}
