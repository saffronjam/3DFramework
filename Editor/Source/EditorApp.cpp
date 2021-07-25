#include <memory>

#define SE_ENTRY_POINT

#include "EditorApp.h"

namespace Se
{
std::unique_ptr<App> CreateApp()
{
	return std::make_unique<App>(AppSpec{WindowSpec{.Title="Hello, world"}});
}
}
