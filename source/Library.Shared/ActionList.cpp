#include "pch.h"
#include "ActionList.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(ActionList);

	ActionList::ActionList(const std::string& name) :
		Action(ActionList::TypeIdClass(), name), actionsDatum{ &At(actionsKey) }
	{
	}

	ActionList::ActionList(RTTI::IdType typeID, const std::string& name) :
		Action(typeID, name), actionsDatum{ &At(actionsKey) }
	{
	}

	gsl::owner<ActionList*> ActionList::Clone() const {
		return new ActionList(*this);
	}

	Vector<Signature> ActionList::Signatures() {
		return Vector<Signature> {
			{ "actions"s, Datum::DatumTypes::Table, 0, 0 }
		};
	}

	void ActionList::Update(const GameTime& gameTime) {
		assert(actionsDatum != nullptr);
		for (std::size_t i = 0; i < actionsDatum->Size(); ++i) {
			Scope& scopeAction = actionsDatum->GetAsTable(i);
			assert(scopeAction.Is(Action::TypeIdClass()));

			Action* childAction = static_cast<Action*>(&scopeAction);
			childAction->Update(gameTime);
		}
	}
	
	void ActionList::CreateAction(const std::string& actionClassName, const std::string& actionName) {
		Scope* newAction = Factory<Scope>::Create(actionClassName);

#ifdef USE_EXCEPTIONS
		if (!newAction) throw std::runtime_error("Could not create an action of that class name.");
#endif // USE_EXCEPTIONS

		assert(newAction->Is(Action::TypeIdClass()));
		Action* newGameObject = static_cast<Action*>(newAction);
		newGameObject->SetName(actionName);
		Adopt(*newGameObject, "actions"s);
	}

	bool ActionList::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const ActionList* other = rhs->As<ActionList>();
		return (other != nullptr ? *this == *other : false);
	}
}
