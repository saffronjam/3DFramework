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
#include <bitset>
#include <initializer_list>
#include <wrl.h>

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

template <typename T>
using ArrayList = std::vector<T>;
template <typename Type, size_t size>
using Array = std::array<Type, size>;
template <typename Key, typename Value>
using Map = std::map<Key, Value>;
template <typename Key, typename Value>
using MultiMap = std::multimap<Key, Value>;
template <typename Key, typename Value>
using UnorderedMap = std::unordered_map<Key, Value>;
template <class FunctionType>
using Function = std::function<FunctionType>;
template <class... Types>
using Tuple = std::tuple<Types...>;
template <typename ClassType>
using Atomic = std::atomic<ClassType>;
template <typename FirstClass, typename SecondClass>
using Pair = std::pair<FirstClass, SecondClass>;
template <typename T>
using Optional = std::optional<T>;
template <size_t t_NumBits>
using Bitset = std::bitset<t_NumBits>;
template <class t_ElemType>
using InitializerList = std::initializer_list<t_ElemType>;

// Memory
template <typename T>
using Unique = std::unique_ptr<T>;
template <typename T>
using Shared = std::shared_ptr<T>;
template <typename T>
using Weak = std::weak_ptr<T>;
template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

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

template <class MoveClass>
constexpr auto&& Move(MoveClass&& moveClass) noexcept
{
	return static_cast<std::remove_reference_t<MoveClass>&&>(moveClass);
}

template <class FirstClass, class SecondClass>
constexpr auto CreatePair(FirstClass&& first, SecondClass&& second) noexcept
{
	using Mypair = std::pair<std::remove_reference_t<FirstClass>, std::remove_reference_t<SecondClass>>;
	return Mypair(std::forward<FirstClass>(first), std::forward<SecondClass>(second));
}

template <class PtrClassTo, class PtrClassFrom>
constexpr Shared<PtrClassTo> DynamicPointerCast(Shared<PtrClassFrom>& ptr) noexcept
{
	return std::dynamic_pointer_cast<PtrClassTo>(ptr);
}

template <class PtrClassTo, class PtrClassFrom>
constexpr Shared<PtrClassTo> DynamicPointerCast(const Shared<PtrClassFrom>& ptr) noexcept
{
	return std::dynamic_pointer_cast<PtrClassTo>(ptr);
}

// Memory

template <typename T, typename ... Args>
constexpr Unique<T> CreateUnique(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T, typename ... Args>
constexpr Shared<T> CreateShared(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}


template <typename T>
constexpr Unique<T> CreateUniqueFromDonor(T* donor)
{
	Unique<T> unique;
	unique.reset(donor);
	return unique;
}

template <typename T>
constexpr Shared<T> CreateSharedFromDonor(T* donor)
{
	Shared<T> shared;
	shared.reset(donor);
	return shared;
}
}
