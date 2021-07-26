#pragma once

namespace Se
{
#ifndef SE_PLATFORM_WINDOWS
#error Saffron only support Windows!
#endif

enum class AppConfiguration
{
	Debug,
	Release,
	Dist
};

#if defined(SE_DEBUG)
static constexpr AppConfiguration Configuration = AppConfiguration::Debug;
#elif defined(SE_RELEASE)
static constexpr AppConfiguration Configuration = AppConfiguration::Release;
#elif defined(SE_DIST)
static constexpr AppConfiguration Configuration = AppConfiguration::Dist;
#endif

static constexpr bool ConfDebug = Configuration == AppConfiguration::Debug;
static constexpr bool ConfRelease = Configuration == AppConfiguration::Release;
static constexpr bool ConfDist = Configuration == AppConfiguration::Dist;
}
