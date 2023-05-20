/// <summary>
/// Definitions for ActionEvent.
/// </summary>

#include "pch.h"
#include "ActionEvent.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(ActionEvent);

	ActionEvent::ActionEvent(const std::string& name, std::string subtype, int delay) :
		Action(ActionEvent::TypeIdClass(), name), _subtype{subtype}, _delay{delay}
	{
	}

	ActionEvent::ActionEvent(RTTI::IdType typeID, const std::string& name, std::string subtype, int delay) :
		Action(typeID, name), _subtype{ subtype }, _delay{ delay }
	{
	}

	gsl::owner<ActionEvent*> ActionEvent::Clone() const {
		return new ActionEvent(*this);
	}

	Vector<Signature> ActionEvent::Signatures() {
		return Vector<Signature> {
			{ "subtype"s, Datum::DatumTypes::String, 1, offsetof(ActionEvent, _subtype) },
			{ "delay"s, Datum::DatumTypes::Integer, 1, offsetof(ActionEvent, _delay) }
		};
	}

	void ActionEvent::Update(const GameTime& gameTime) {
		EventMessageAttributed payload(_subtype);		
		ForEachAuxiliaryAttribute([&payload](const std::string& key, Datum& value) {
			payload.Append(key) = value;
			return false;
		});

		GameState::GetEventQueue().Enqueue(std::make_shared<Event<EventMessageAttributed>>(Event<EventMessageAttributed>(payload)), gameTime, std::chrono::milliseconds(_delay));
		UNREFERENCED_LOCAL(gameTime);
	}

	bool ActionEvent::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const ActionEvent* other = rhs->As<ActionEvent>();
		return (other != nullptr ? *this == *other : false);
	}
}



