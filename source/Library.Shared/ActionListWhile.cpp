#include "pch.h"
#include "ActionListWhile.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	ActionListWhile::ActionListWhile(const std::string& name) :
		ActionList(ActionListWhile::TypeIdClass(), name)
	{
	}

	ActionListWhile::ActionListWhile(RTTI::IdType typeID, const std::string& name) :
		ActionList(typeID, name)
	{
	}

	gsl::owner<ActionListWhile*> ActionListWhile::Clone() const {
		return new ActionListWhile(*this);
	}

	Vector<Signature> ActionListWhile::Signatures() {
		return Vector<Signature> {
			{ "condition"s, Datum::DatumTypes::Integer, 1, offsetof(ActionListWhile, condition) },
			{ "preamble"s, Datum::DatumTypes::Table, 0, 0 },
			{ "loop"s, Datum::DatumTypes::Table, 0, 0 },
			{ "increment"s, Datum::DatumTypes::Table, 0, 0 }
		};
	}

	void ActionListWhile::Update(const GameTime& gameTime) {
		RunActionsBlock("preamble"s, gameTime);
		while (condition) {
			RunActionsBlock("loop"s, gameTime);
			RunActionsBlock("increment"s, gameTime);
		}
	}

	void ActionListWhile::RunActionsBlock(const std::string& tableName, const GameTime& gameTime) {
		Datum& actions = At(tableName);
		for (std::size_t i = 0; i < actions.Size(); ++i) {
			Scope& scopeAction = actions.GetAsTable(i);
			assert(scopeAction.Is(Action::TypeIdClass()));

			Action* childAction = static_cast<Action*>(&scopeAction);
			childAction->Update(gameTime);
		}
	}
}
