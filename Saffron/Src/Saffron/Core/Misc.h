#pragma once


namespace Se
{
namespace Misc
{
namespace Details
{
struct SturcturedVoid {};
inline SturcturedVoid NoValue{};

template <typename T>
struct NonVoid { using Type = T; };
template <>
struct NonVoid<void> { using Type = SturcturedVoid; };
};

template <typename T>
using NonVoid = typename Details::NonVoid<T>::Type;

};

}