#pragma once
#include "Action.h"
#include "GameTime.h"

namespace UnitTests {
	class DummyAction final : public FIEAGameEngine::Action
	{
		RTTI_DECLARATIONS(DummyAction, Action);

	public:
		DummyAction(const std::string& name = "");

		[[nodiscard]] gsl::owner<DummyAction*> Clone() const override;
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		virtual void Update(const FIEAGameEngine::GameTime& gameTime);

		int updateNumber = 0;
	};

	CONCRETE_FACTORY(DummyAction, FIEAGameEngine::Scope)
}


