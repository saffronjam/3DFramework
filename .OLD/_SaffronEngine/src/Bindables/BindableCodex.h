#pragma once

#include <type_traits>
#include <memory>
#include <unordered_map>

#include "Bindable.h"

namespace Bind
{
    class Codex
    {
    public:
        template<class T, typename...Params>
        static std::shared_ptr<T> Resolve(Params &&...p) noxnd;
        static void Destroy();

    private:
        static Codex &Get();

    private:
        static std::unordered_map<std::string, std::shared_ptr<Bindable>> binds;
    };

    template<class T, typename... Params>
    std::shared_ptr<T> Codex::Resolve(Params &&... p) noxnd
    {
        static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");

        const auto key = T::GenerateUID(std::forward<Params>(p)...);
        const auto i = binds.find(key);
        if (i == binds.end())
        {
            auto bind = std::make_shared<T>(std::forward<Params>(p)...);
            binds[key] = bind;
            return bind;
        }
        else
        {
            return std::static_pointer_cast<T>(i->second);
        }
    }
}