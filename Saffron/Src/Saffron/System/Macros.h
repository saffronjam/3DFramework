#pragma once

#define BIT(x) (1 << x)
#define TO_STRING(Var) #Var
#define SE_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }
#define SE_EXT_EVENT_FN(obj, fn) [this](auto&&... args) -> decltype(auto) { return (obj).fn(std::forward<decltype(args)>(args)...); }