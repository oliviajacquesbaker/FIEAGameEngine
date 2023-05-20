/// <summary>
/// Definitions for an Action.
/// </summary>

#include "pch.h"
#include "Action.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(Action);

	Action::Action(RTTI::IdType typeID, const std::string& name) :
		Attributed(typeID), name{ name }
	{
	}

	FIEAGameEngine::Vector<FIEAGameEngine::Signature> Action::Signatures() {
		return Vector<Signature> {
			{ "name"s, Datum::DatumTypes::String, 1, offsetof(Action, name) },
		};
	}

	bool Action::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const Action* other = rhs->As<Action>();
		return (other != nullptr ? *this == *other : false);
	}

	std::string Action::ToString() const {
		return name;
	}
}