#pragma once

#include "Base.h"
#include "Header.h"

namespace Se
{
class Generator
{
public:
	void Run();

private:
	static auto LoadHeaderPaths(const fs::path& root) -> std::vector<fs::path>;

private:
	static constexpr const char* RelativeEngineRoot = "../Engine/Source";
};
}
