#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <tuple>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <optional>

namespace Se
{
typedef char Int8;
typedef unsigned char Uint8;

// 16 bits integer types
typedef short Int16;
typedef unsigned short Uint16;

// 32 bits integer types
typedef int Int32;
typedef unsigned int Uint32;

// 64 bits integer types
#if defined(_MSC_VER)
typedef __int64 Int64;
typedef unsigned __int64 Uint64;
#else
typedef long long Int64;
typedef unsigned long long Uint64;
#endif

template<typename T>
using ArrayList = std::vector<T>;
template<typename Key, typename Value>
using Map = std::map<Key, Value>;
template<typename Key, typename Value>
using MultiMap = std::multimap<Key, Value>;
template<typename Key, typename Value>
using UnorderedMap = std::unordered_map<Key, Value>;
template<class FunctionType>
using Function = std::function<FunctionType>;
template<class... Types>
using Tuple = std::tuple<Types...>;
template<typename ClassType>
using Atomic = std::atomic<ClassType>;
template<typename FirstClass, typename SecondClass>
using Pair = std::pair<FirstClass, SecondClass>;
template<typename T>
using Optional = std::optional<T>;

using String = std::string;
using WideString = std::wstring;
using Filepath = std::filesystem::path;
using DirectoryEntry = std::filesystem::directory_entry;
using OutputStream = std::ofstream;
using InputStream = std::ifstream;
using StringStream = std::stringstream;
using OutputStringStream = std::ostringstream;
using InputStringStream = std::istringstream;
using Thread = std::thread;
using Mutex = std::mutex;
using ConditionVariable = std::condition_variable;

template<class MoveClass>
constexpr auto &&Move(MoveClass &&moveClass)  noexcept
{
	return static_cast<std::remove_reference_t<MoveClass> &&>(moveClass);
}

template<class FirstClass, class SecondClass>
constexpr auto CreatePair(FirstClass &&first, SecondClass &second)  noexcept
{
	return std::make_pair(first, second);
}

}