#pragma once
#include "DefaultGrowth.h"

namespace FIEAGameEngine {
	inline std::size_t DefaultGrowth::operator()(const std::size_t currentCapacity ) const {
		return std::max(currentCapacity / std::size_t(2), std::size_t(1)); //default cap to 0 in param
	}
}