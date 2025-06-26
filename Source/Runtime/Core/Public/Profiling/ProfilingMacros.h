#pragma once


#ifdef BS_PROFILING_ON
#include "Profiling/ProfileTimer.h"

#include <source_location>

#define BS_PROFILE_RANGE(Name) ::Bloodshot::FProfileTimer __InternalProfileTimer(Name)
#define BS_PROFILE_FUNCTION() ::Bloodshot::FProfileTimer __InternalProfileTimer(std::source_location::current().function_name())
#else
#define BS_PROFILE_RANGE(Name)
#define BS_PROFILE_FUNCTION()
#endif
