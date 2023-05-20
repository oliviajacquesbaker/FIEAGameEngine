/// <summary>
/// The declaration of the class EmptyRTTI, which functions to as close to an RTTI instance as I could easily achieve.
/// </summary>

#pragma once

#include "RTTI.h"

namespace UnitTests
{
	class EmptyRTTI final : public FIEAGameEngine::RTTI
	{
		RTTI_DECLARATIONS(Foo, FIEAGameEngine::RTTI);

		bool operator==(const EmptyRTTI& rhs);
	};
}
