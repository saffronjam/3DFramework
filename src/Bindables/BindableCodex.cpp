#include "BindableCodex.h"

namespace Bind
{
    std::unordered_map<std::string, std::shared_ptr<Bindable>> Codex::binds;

    Codex &Codex::Get()
    {
        static Codex codex;
        return codex;
    }

    void Codex::Destroy()
    {
        Codex::binds.clear();
    }
}