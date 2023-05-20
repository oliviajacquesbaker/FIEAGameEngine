/// <summary>
/// Unit tests exercising the functionality of ReactionAttributed
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DummyAction.h"
#include "ReactionAttributed.h"
#include "ActionIncrement.h"
#include "Action.h"
#include "ActionEvent.h"
#include "JsonParseCoordinator.h"
#include "JsonTableParseHelper.h"
#include "Factory.h"
#include "GameObject.h"
#include "GameTime.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace glm;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<ReactionAttributed>(ReactionAttributed* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<ReactionAttributed>(const ReactionAttributed* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<ReactionAttributed>(const ReactionAttributed& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace LibraryDesktopTests
{
	class DummyReactionAttributed final : public ReactionAttributed {
		RTTI_DECLARATIONS(DummyReactionAttributed, ReactionAttributed);

	public:
		int extraInt;

		DummyReactionAttributed(const std::string& name = "") :
			ReactionAttributed(DummyReactionAttributed::TypeIdClass(), name)
		{};

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "extraInt"s, Datum::DatumTypes::Integer, 1, offsetof(DummyReactionAttributed, extraInt) },
			};
		}
	};
	RTTI_DEFINITIONS(DummyReactionAttributed);
	CONCRETE_FACTORY(DummyReactionAttributed, FIEAGameEngine::Scope)

		TEST_CLASS(ReactionAttributedTests) {
	public:

		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();
			RegisterType<GameObject>();
			RegisterType<Action>();
			RegisterType<EventMessageAttributed>();
			RegisterType<ActionIncrement, Action>();
			RegisterType<ActionList, Action>();
			RegisterType<ActionEvent, Action>();
			RegisterType<ReactionAttributed, ActionList>();
			Factory<Scope>::Add(make_unique<GameObjectFactory>());
			Factory<Scope>::Add(make_unique<EventMessageAttributedFactory>());
			Factory<Scope>::Add(make_unique<ActionListFactory>());
			Factory<Scope>::Add(make_unique<ActionEventFactory>());
			Factory<Scope>::Add(make_unique<ActionIncrementFactory>());
			Factory<Scope>::Add(make_unique<ReactionAttributedFactory>());
		}

		TEST_CLASS_CLEANUP(TestClassCleanup)
		{
			TypeManager::Clear();
			Factory<Scope>::Clear();
		}

		TEST_METHOD_INITIALIZE(Initialize)
		{
			Event<EventMessageAttributed>::RequestClearSubscribers();
			GameState::RequestCleanup();

#if defined(DEBUG) || defined(_DEBUG)
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			_CrtMemCheckpoint(&_startMemState);
#endif
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
			Event<EventMessageAttributed>::RequestClearSubscribers();

#if defined(DEBUG) || defined(_DEBUG)
			_CrtMemState endMemState, diffMemState;
			_CrtMemCheckpoint(&endMemState);
			if (_CrtMemDifference(&diffMemState, &_startMemState, &endMemState))
			{
				_CrtMemDumpStatistics(&diffMemState);
				Assert::Fail(L"Memory Leaks!");
			}
#endif
		}

		TEST_METHOD(ReactionAttributedRTTIBasics)
		{
			{
				ReactionAttributed a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ReactionAttributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(ReactionAttributed::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				ReactionAttributed* s = rtti->As<ReactionAttributed>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const ReactionAttributed a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ReactionAttributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(ReactionAttributed::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const ReactionAttributed* s = rtti->As<ReactionAttributed>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(ReactionAttributedRTTIOverrides)
		{
			ReactionAttributed a("nameVal");
			RTTI* rtti = &a;

			std::string expected = "nameVal";
			Assert::AreEqual(expected, rtti->ToString());

			ReactionAttributed e("nameVal");
			RTTI* eRtti = &e;
			Bar d;
			RTTI* dRtti = &d;

			Assert::IsTrue(rtti->Equals(eRtti));
			Assert::IsFalse(rtti->Equals(dRtti));
			Assert::IsFalse(rtti->Equals(nullptr));

			a.Append("key2") = { "A"s, "B"s };
			Assert::AreEqual(expected, rtti->ToString());
			Assert::IsFalse(rtti->Equals(eRtti));
		}

		TEST_METHOD(ActionIncrementDerivedPopulation)
		{
			RegisterType<DummyReactionAttributed, ReactionAttributed>();
			Factory<Scope>::Add(make_unique<DummyReactionAttributedFactory>());

			DummyReactionAttributed a;
			RTTI* rtti = &a;
			Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
			Assert::IsTrue(rtti->Is(DummyReactionAttributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(ReactionAttributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
			Assert::AreEqual(DummyReactionAttributed::TypeIdClass(), rtti->TypeIdInstance());
			Assert::AreEqual(0, a["extraInt"].GetAsInt());

			//prevent perceived memory leak
			TypeManager::Remove(DummyReactionAttributed::TypeIdClass());
			Factory<Scope>::Remove("DummyReactionAttributed"s);
		}

		TEST_METHOD(CopySemantics)
		{
			ReactionAttributed action("test action");
			action.At("subtype").Set("test subtype");
			{
				ReactionAttributed copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual("test subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ReactionAttributed copy;
				copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual("test subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			ReactionAttributed action("test action");
			action.At("subtype").Set("test subtype");
			{
				ReactionAttributed movingCopy = action;
				ReactionAttributed copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual("test subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ReactionAttributed movingCopy = action;
				ReactionAttributed copy;
				copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual("test subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(ReactionAttributedClone)
		{
			ReactionAttributed action("Test Action"s, "Test Subtype"s);

			auto clone = action.Clone();
			ReactionAttributed* clonedGameObject = clone->As<ReactionAttributed>();
			Assert::IsNotNull(clonedGameObject);
			Assert::AreNotSame(action, *clonedGameObject);
			Assert::AreEqual(action, *clonedGameObject);

			action["name"].Set("new name"s);
			Assert::AreEqual("new name"s, action.Name());
			Assert::AreNotEqual(action, *clonedGameObject);

			action["subtype"].Set("some subtype");

			auto clone2 = action.Clone();
			ReactionAttributed* clonedGameObject2 = clone2->As<ReactionAttributed>();
			Assert::IsNotNull(clonedGameObject2);
			Assert::AreNotSame(action, *clonedGameObject2);
			Assert::AreEqual(action, *clonedGameObject2);
			Assert::AreEqual("new name"s, clonedGameObject2->Name());
			Assert::AreEqual("some subtype"s, clonedGameObject2->Subtype());

			//prevent memory leaks
			delete clone;
			delete clone2;
		}

		TEST_METHOD(NotifyAndUpdate)
		{
			GameObject gameObject;
			GameState::SetRootObject(gameObject);
			GameState::SetGameClockEnabled(false);

			Datum& a = gameObject.AppendAuxiliaryAttribute("A"s) = 1;
			const string name = "Test Reaction"s;
			const string subtype = "Test"s;
			ReactionAttributed* reaction = new ReactionAttributed(name, subtype);
			gameObject.Adopt(*reaction, "reactions"s);

			reaction->CreateAction("ActionIncrement"s, "Increment Action"s);
			ActionIncrement* incrementAction = reaction->At("actions"s).GetAsTable().As<ActionIncrement>();
			Assert::IsNotNull(incrementAction);
			Assert::IsTrue(incrementAction->Target().empty());
			Assert::AreEqual(1, incrementAction->IncrementAmount());

			EventMessageAttributed payload(subtype);
			payload.AppendAuxiliaryAttribute("target"s) = "A"s;
			payload.AppendAuxiliaryAttribute("incrementAmount"s) = 2;
			Event<EventMessageAttributed> eventPayload(payload);

			reaction->Notify(eventPayload);

			Assert::AreEqual("A"s, incrementAction->Target());
			Assert::AreEqual(2, incrementAction->IncrementAmount());
			Assert::AreEqual(3, a.GetAsInt());

			GameState::GetEventQueue().Clear();
		}

		TEST_METHOD(NotifyWrongSubtype)
		{
			GameObject gameObject;
			GameState::SetRootObject(gameObject);
			GameState::SetGameClockEnabled(false);

			Datum& a = gameObject.AppendAuxiliaryAttribute("A"s) = 1;
			const string name = "Test Reaction"s;
			const string subtype = "Test"s;
			ReactionAttributed* reaction = new ReactionAttributed(name, subtype);
			gameObject.Adopt(*reaction, "reactions"s);

			reaction->CreateAction("ActionIncrement"s, "Increment Action"s);
			ActionIncrement* incrementAction = reaction->At("actions"s).GetAsTable().As<ActionIncrement>();
			Assert::IsNotNull(incrementAction);
			Assert::IsTrue(incrementAction->Target().empty());
			Assert::AreEqual(1, incrementAction->IncrementAmount());

			EventMessageAttributed payload("wrong subtype!"s);
			payload.AppendAuxiliaryAttribute("target"s) = "A"s;
			payload.AppendAuxiliaryAttribute("incrementAmount"s) = 2;
			Event<EventMessageAttributed> eventPayload(payload);

			reaction->Notify(eventPayload);

			Assert::AreEqual(""s, incrementAction->Target());
			Assert::AreEqual(1, incrementAction->IncrementAmount());
			Assert::AreEqual(1, a.GetAsInt());

			GameState::GetEventQueue().Clear();
		}

		TEST_METHOD(GetSetSubtype)
		{
			{
				ReactionAttributed actionDummy;
				Assert::AreEqual(""s, actionDummy.Subtype());
				actionDummy.SetSubtype("subtype test"s);
				Assert::AreEqual("subtype test"s, actionDummy.Subtype());
			}
			{
				const ReactionAttributed actionDummy("test name"s, "test"s);
				Assert::AreEqual("test"s, actionDummy.Subtype());
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}