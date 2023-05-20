/// <summary>
/// Definitions for eventMessageAttributed.
/// </summary>

#include "pch.h"
#include "EventMessageAttributed.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(EventMessageAttributed);

	EventMessageAttributed::EventMessageAttributed(std::string subtype) :
		Attributed(EventMessageAttributed::TypeIdClass()), _subtype{ std::move(subtype) }
	{
	}
	
	gsl::owner<EventMessageAttributed*> EventMessageAttributed::Clone() const {
		return new EventMessageAttributed(*this);
	}
	
	FIEAGameEngine::Vector<FIEAGameEngine::Signature> EventMessageAttributed::Signatures() {
		return Vector<Signature> {
			{ "subtype"s, Datum::DatumTypes::String, 1, offsetof(EventMessageAttributed, _subtype) }
		};
	}

	bool EventMessageAttributed::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const EventMessageAttributed* other = rhs->As<EventMessageAttributed>();
		return (other != nullptr ? *this == *other : false);
	}

	std::string EventMessageAttributed::ToString() const {
		return "EventMessageAttributed with subtype "s + _subtype;
	}
}
