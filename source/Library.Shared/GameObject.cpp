/// <summary>
/// Definitions for a GameObject.
/// </summary>

#include "pch.h"
#include "GameObject.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(GameObject);

	GameObject::GameObject(const std::string& name) :
		Attributed(GameObject::TypeIdClass()), name{name}, actionsDatum{&At(actionsKey)}, childrenDatum{&At(childrenKey)}
	{
	}

	GameObject::GameObject(RTTI::IdType typeID, const std::string& name) :
		Attributed(typeID), name{ name }, actionsDatum{ &At(actionsKey) }, childrenDatum{ &At(childrenKey) }
	{
	}
	
	gsl::owner<GameObject*> GameObject::Clone() const {
		return new GameObject(*this);
	}

	FIEAGameEngine::Vector<FIEAGameEngine::Signature> GameObject::Signatures() {
		return Vector<Signature> {
			{ "name"s, Datum::DatumTypes::String, 1, offsetof(GameObject, name) },
			{ "position"s, Datum::DatumTypes::Vector, 1, offsetof(GameObject, position) },
			{ "rotation"s, Datum::DatumTypes::Vector, 1, offsetof(GameObject, rotation) },
			{ "scale"s, Datum::DatumTypes::Vector, 1, offsetof(GameObject, scale) },
			{ "children"s, Datum::DatumTypes::Table, 0, 0 },
			{ "actions"s, Datum::DatumTypes::Table, 0, 0 }
		};
	}

	void GameObject::Update(const GameTime& gameTime) {
		UpdateActions(gameTime);
		UpdateChildren(gameTime);
	}

	void GameObject::CreateChild(const std::string& childClassName, const std::string& childName)
	{
		Scope* newChild = Factory<Scope>::Create(childClassName);

#ifdef USE_EXCEPTIONS
		if (!newChild) throw std::runtime_error("Could not create a child of that class name.");
#endif // USE_EXCEPTIONS

		assert(newChild->Is(GameObject::TypeIdClass()));
		GameObject* newGameObject = static_cast<GameObject*>(newChild);
		newGameObject->name = childName;
		Adopt(*newGameObject, "children"s);
	}

	void GameObject::CreateAction(const std::string& actionClassName, const std::string& actionName)
	{
		Scope* newAction = Factory<Scope>::Create(actionClassName);

#ifdef USE_EXCEPTIONS
		if (!newAction) throw std::runtime_error("Could not create an action of that class name.");
#endif // USE_EXCEPTIONS

		assert(newAction->Is(Action::TypeIdClass()));
		Action* newGameObject = static_cast<Action*>(newAction);
		newGameObject->SetName(actionName);
		Adopt(*newGameObject, "actions"s);
	}

	bool GameObject::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const GameObject* other = rhs->As<GameObject>();
		return (other != nullptr ? *this == *other : false);
	}

	std::string GameObject::ToString() const {
		return name;
	}

	void GameObject::UpdateChildren(const GameTime& gameTime) {
		assert(childrenDatum != nullptr);
		for (std::size_t i = 0; i < childrenDatum->Size(); ++i) {
			Scope& scopeChild = childrenDatum->GetAsTable(i);
			assert(scopeChild.Is(GameObject::TypeIdClass()));

			GameObject* childGameObject = static_cast<GameObject*>(&scopeChild);
			childGameObject->Update(gameTime);
		}
	}

	void GameObject::UpdateActions(const GameTime& gameTime) {
		assert(actionsDatum != nullptr);
		for (std::size_t i = 0; i < actionsDatum->Size(); ++i) {
			Scope& scopeAction = actionsDatum->GetAsTable(i);
			assert(scopeAction.Is(Action::TypeIdClass()));

			Action* childAction = static_cast<Action*>(&scopeAction);
			childAction->Update(gameTime);
		}
	}
}
