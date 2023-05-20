#pragma once

#include <cstdint>
#include <algorithm>

namespace FIEAGameEngine {
	struct DefaultGrowth final {
		std::size_t operator()(const std::size_t currentCapacity) const;
	};
}

#include "DefaultGrowth.inl"

