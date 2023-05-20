/// <summary>
/// Definitions for an ActionIncrement.
/// </summary>

#include "pch.h"
#include "ActionIncrement.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(ActionIncrement);

	ActionIncrement::ActionIncrement(const std::string& name) :
		Action(ActionIncrement::TypeIdClass(), name)
	{
	}

	ActionIncrement::ActionIncrement(RTTI::IdType typeID, const std::string& name) :
		Action(typeID, name)
	{
	}

	gsl::owner<ActionIncrement*> ActionIncrement::Clone() const {
		return new ActionIncrement(*this);
	}

	Vector<Signature> ActionIncrement::Signatures() {
		return Vector<Signature> {
			{ "incrementAmount"s, Datum::DatumTypes::Integer, 1, offsetof(ActionIncrement, incrementAmount) },
			{ "target"s, Datum::DatumTypes::String, 1, offsetof(ActionIncrement, target) },
			{ "targetIndex"s, Datum::DatumTypes::Integer, 1, offsetof(ActionIncrement, targetIndex) }
		};
	}

	void ActionIncrement::Update(const GameTime& gameTime) {
		UNREFERENCED_LOCAL(gameTime);
		Datum* targetDatum = Search(target); //Unsure how to cache this in a way where if the target datum is removed, it won't be accessing bad data...

#ifdef USE_EXCEPTIONS
		if (!targetDatum) throw std::runtime_error("Could not find the requested variable to increment.");
#endif // USE_EXCEPTIONS

		targetDatum->Set(targetDatum->GetAsInt(targetIndex) + incrementAmount, targetIndex);
	}

	bool ActionIncrement::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const ActionIncrement* other = rhs->As<ActionIncrement>();
		return (other != nullptr ? *this == *other : false);
	}
}

