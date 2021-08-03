#pragma once

#include <cstddef>

#include "wrl/client.h"

namespace Se
{
using uchar = uint8_t;
using ushort = uint16_t;
using uint = uint32_t;
using ulong = uint64_t;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
}
