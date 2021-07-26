#pragma once

#include "wrl/client.h"

namespace Se
{
using uchar = unsigned char;
using ushort = unsigned short;
using uint = unsigned int;
using ulong = unsigned long long;

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;
}
