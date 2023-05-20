/// <summary>
/// Definitions for Reaction.
/// </summary>

#include "pch.h"
#include "Reaction.h"

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(Reaction);

	Reaction::Reaction(RTTI::IdType typeID, const std::string& name) :
		ActionList(typeID, name)
	{
	}
}

