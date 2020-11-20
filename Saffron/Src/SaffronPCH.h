#pragma once

#ifdef SE_PLATFORM_WINDOWS
#define NOMINMAX
#include <Windows.h>
#endif

#include <memory>
#include <functional>
#include <mutex>
#include <random>
#include <fstream>
#include <optional>
#include <iomanip>
#include <filesystem>
#include <bitset>
#include <iostream>

#include <atomic>
#include <sstream>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>
#include <deque>
#include <set>
#include <cstdio>

#include "Saffron/Debug/Instrumentor.h"
#include "Saffron/Core/Log.h"
#include "Saffron/Core/Assert.h"

// Math
#include <Saffron/Core/Math/Mat4.h>
