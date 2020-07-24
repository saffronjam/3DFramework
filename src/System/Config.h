#pragma once

#ifndef NDEBUG
#define SAFFRON_DEBUG
#define SAFFRON_DEBUG_ACTIVE true
#else
#define SAFFRON_DEBUG_ACTIVE false
#endif

#define noxnd noexcept(SAFFRON_DEBUG_ACTIVE)

