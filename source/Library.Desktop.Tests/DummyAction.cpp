#include "pch.h"
#include "DummyAction.h"

using namespace FIEAGameEngine;
using namespace std::string_literals;

namespace UnitTests {
	RTTI_DEFINITIONS(DummyAction);

	DummyAction::DummyAction(const std::string& name) :
		Action(DummyAction::TypeIdClass(), name)
	{};

	[[nodiscard]] gsl::owner<DummyAction*> DummyAction::Clone() const {
		return new DummyAction(*this);
	}

	Vector<Signature> DummyAction::Signatures() {
		return Vector<Signature> {
			{ "updateNumber"s, Datum::DatumTypes::Integer, 1, offsetof(DummyAction, updateNumber) }
		};
	}

	void DummyAction::Update(const GameTime& gameTime) {
		UNREFERENCED_LOCAL(gameTime);
		++updateNumber;
	}
}