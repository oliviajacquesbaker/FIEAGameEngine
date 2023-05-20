/// <summary>
/// Definitions for an ActionWhile
/// </summary>

#include "pch.h"
#include "ActionWhile.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(ActionWhile);

	ActionWhile::ActionWhile(const std::string& name) :
		Action(ActionWhile::TypeIdClass(), name), preambleDatum{ &At(preambleKey) }, loopDatum{ &At(loopKey) }, incrementDatum{ &At(incrementKey) }
	{
	}

	ActionWhile::ActionWhile(RTTI::IdType typeID, const std::string& name) :
		Action(typeID, name), preambleDatum{ &At(preambleKey) }, loopDatum{ &At(loopKey) }, incrementDatum{ &At(incrementKey) }
	{
	}

	gsl::owner<ActionWhile*> ActionWhile::Clone() const {
		return new ActionWhile(*this);
	}

	Vector<Signature> ActionWhile::Signatures() {
		return Vector<Signature> {
			{ "condition"s, Datum::DatumTypes::Integer, 1, offsetof(ActionWhile, condition) },
			{ "preamble"s, Datum::DatumTypes::Table, 0, 0 },
			{ "loop"s, Datum::DatumTypes::Table, 0, 0 },
			{ "increment"s, Datum::DatumTypes::Table, 0, 0 }
		};
	}

	void ActionWhile::Update(const GameTime& gameTime) {
		RunActionsBlock(preambleDatum, gameTime);
		while (condition) {
			RunActionsBlock(loopDatum, gameTime);
			RunActionsBlock(incrementDatum, gameTime);
		}
	}

	void ActionWhile::RunActionsBlock(Datum* actions, const GameTime& gameTime) {		
#ifdef USE_EXCEPTIONS
		if (!actions) throw std::runtime_error("Could not find the given table attribute.");
#endif // USE_EXCEPTIONS

		for (std::size_t i = 0; i < actions->Size(); ++i) {
			Scope& scopeAction = actions->GetAsTable(i);
			assert(scopeAction.Is(Action::TypeIdClass()));

			Action* childAction = static_cast<Action*>(&scopeAction);
			childAction->Update(gameTime);
		}
	}

	bool ActionWhile::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const ActionWhile* other = rhs->As<ActionWhile>();
		return (other != nullptr ? *this == *other : false);
	}
}
