/// <summary>
/// Unit tests exercising the functionality of ActionEvent
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DummyAction.h"
#include "ActionEvent.h"
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
	std::wstring ToString<ActionEvent>(ActionEvent* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<ActionEvent>(const ActionEvent* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<ActionEvent>(const ActionEvent& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace LibraryDesktopTests
{
	class DummyActionEvent final : public ActionEvent {
		RTTI_DECLARATIONS(DummyActionEvent, ActionEvent);

	public:
		int extraInt;

		DummyActionEvent(const std::string& name = "") :
			ActionEvent(DummyActionEvent::TypeIdClass(), name)
		{};

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "extraInt"s, Datum::DatumTypes::Integer, 1, offsetof(DummyActionEvent, extraInt) },
			};
		}
	};
	RTTI_DEFINITIONS(DummyActionEvent);
	CONCRETE_FACTORY(DummyActionEvent, FIEAGameEngine::Scope)

	TEST_CLASS(ActionEventTests) {
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

	TEST_METHOD(ActionEventRTTIBasics)
	{
		{
			ActionEvent a;
			RTTI* rtti = &a;
			Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
			Assert::IsTrue(rtti->Is(ActionEvent::TypeIdClass()));
			Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
			Assert::AreEqual(ActionEvent::TypeIdClass(), rtti->TypeIdInstance());

			Bar* b = rtti->As<Bar>();
			Assert::IsNull(b);

			ActionEvent* s = rtti->As<ActionEvent>();
			Assert::IsNotNull(s);
			Assert::AreEqual(&a, s);

			Attributed* s2 = rtti->As<Attributed>();
			Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
			Assert::IsNotNull(s2);
		}
		{
			const ActionEvent a;
			const RTTI* rtti = &a;
			Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
			Assert::IsTrue(rtti->Is(ActionEvent::TypeIdClass()));
			Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
			Assert::AreEqual(ActionEvent::TypeIdClass(), rtti->TypeIdInstance());

			const Bar* b = rtti->As<Bar>();
			Assert::IsNull(b);

			const ActionEvent* s = rtti->As<ActionEvent>();
			Assert::IsNotNull(s);
			Assert::AreEqual(&a, s);

			const Attributed* s2 = rtti->As<Attributed>();
			Assert::IsNotNull(s2);
		}
	}

	TEST_METHOD(ActionEventRTTIOverrides)
	{
		ActionEvent a("nameVal");
		RTTI* rtti = &a;

		std::string expected = "nameVal";
		Assert::AreEqual(expected, rtti->ToString());

		ActionEvent e("nameVal");
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
		RegisterType<DummyActionEvent, ActionEvent>();
		Factory<Scope>::Add(make_unique<DummyActionEventFactory>());

		DummyActionEvent a;
		RTTI* rtti = &a;
		Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
		Assert::IsTrue(rtti->Is(DummyActionEvent::TypeIdClass()));
		Assert::IsTrue(rtti->Is(ActionEvent::TypeIdClass()));
		Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
		Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
		Assert::AreEqual(DummyActionEvent::TypeIdClass(), rtti->TypeIdInstance());
		Assert::AreEqual(0, a["extraInt"].GetAsInt());

		//prevent perceived memory leak
		TypeManager::Remove(DummyActionEvent::TypeIdClass());
		Factory<Scope>::Remove("DummyActionEvent"s);
	}

	TEST_METHOD(CopySemantics)
	{
		ActionEvent action("test action");
		action.At("subtype").Set("test subtype");
		{
			ActionEvent copy = action;
			Assert::AreEqual(action.Size(), copy.Size());
			Assert::AreEqual("test action"s, copy.At("name").GetAsString());
			Assert::AreEqual("test subtype"s, copy.At("subtype").GetAsString());
			Assert::AreEqual(action, copy);
			Assert::AreNotSame(action, copy);
		}
		{
			ActionEvent copy;
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
		ActionEvent action("test action");
		action.At("subtype").Set("test subtype");
		{
			ActionEvent movingCopy = action;
			ActionEvent copy = std::move(movingCopy);
			Assert::AreEqual(action.Size(), copy.Size());
			Assert::AreEqual("test action"s, copy.At("name").GetAsString());
			Assert::AreEqual("test subtype"s, copy.At("subtype").GetAsString());
			Assert::AreEqual(action, copy);
			Assert::AreNotSame(action, copy);
		}
		{
			ActionEvent movingCopy = action;
			ActionEvent copy;
			copy = std::move(movingCopy);
			Assert::AreEqual(action.Size(), copy.Size());
			Assert::AreEqual("test action"s, copy.At("name").GetAsString());
			Assert::AreEqual("test subtype"s, copy.At("subtype").GetAsString());
			Assert::AreEqual(action, copy);
			Assert::AreNotSame(action, copy);
		}
	}

	TEST_METHOD(ActionEventClone)
	{
		ActionEvent action("Test Action"s, "Test Subtype"s);

		auto clone = action.Clone();
		ActionEvent* clonedGameObject = clone->As<ActionEvent>();
		Assert::IsNotNull(clonedGameObject);
		Assert::AreNotSame(action, *clonedGameObject);
		Assert::AreEqual(action, *clonedGameObject);

		action["name"].Set("new name"s);
		Assert::AreEqual("new name"s, action.Name());
		Assert::AreNotEqual(action, *clonedGameObject);

		action["subtype"].Set("some subtype");
		action["delay"].Set(15);

		auto clone2 = action.Clone();
		ActionEvent* clonedGameObject2 = clone2->As<ActionEvent>();
		Assert::IsNotNull(clonedGameObject2);
		Assert::AreNotSame(action, *clonedGameObject2);
		Assert::AreEqual(action, *clonedGameObject2);
		Assert::AreEqual("new name"s, clonedGameObject2->Name());
		Assert::AreEqual("some subtype"s, clonedGameObject2->Subtype());
		Assert::AreEqual(15, clonedGameObject2->Delay());

		//prevent memory leaks
		delete clone;
		delete clone2;
	}

	TEST_METHOD(UpdateFullSystem)
	{
		GameObject gameObject;
		GameState::SetRootObject(gameObject);
		EventQueue& eventQueue = GameState::GetEventQueue();
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

		gameObject.CreateChild("GameObject"s, "Test Child Game Object"s);
		GameObject* childGameObject = gameObject.At("children"s).GetAsTable().As<GameObject>();
		Assert::IsNotNull(childGameObject);
		childGameObject->CreateAction("ActionEvent"s, "Test Action"s);
		ActionEvent* actionEvent = childGameObject->At("actions").GetAsTable().As<ActionEvent>();
		Assert::IsNotNull(actionEvent);

		actionEvent->SetSubtype(subtype);
		actionEvent->AppendAuxiliaryAttribute("target"s) = "A"s;
		actionEvent->AppendAuxiliaryAttribute("incrementAmount"s) = 2;
		Assert::AreEqual(0, actionEvent->Delay());

		GameState::GetGameTime().SetCurrentTime(std::chrono::high_resolution_clock::time_point());
		Assert::AreEqual(size_t(0), eventQueue.Size());
		GameState::Update();
		Assert::AreEqual(size_t(1), eventQueue.Size());
		GameState::Update();

		Assert::AreEqual("A"s, incrementAction->Target());
		Assert::AreEqual(2, incrementAction->IncrementAmount());
		Assert::AreEqual(3, a.GetAsInt());

		GameState::GetEventQueue().Clear();
	}

	TEST_METHOD(GetSetSubtype)
	{
		{
			ActionEvent actionDummy;
			Assert::AreEqual(""s, actionDummy.Subtype());
			actionDummy.SetSubtype("subtype test"s);
			Assert::AreEqual("subtype test"s, actionDummy.Subtype());
		}
		{
			const ActionEvent actionDummy("test name"s, "test"s);
			Assert::AreEqual("test"s, actionDummy.Subtype());
		}
	}

	TEST_METHOD(GetSetDelay)
	{
		{
			ActionEvent actionDummy;
			Assert::AreEqual(0, actionDummy.Delay());
			actionDummy.SetDelay(17);
			Assert::AreEqual(17, actionDummy.Delay());
		}
		{
			const ActionEvent actionDummy("test name"s, "test"s, 3);
			Assert::AreEqual(3, actionDummy.Delay());
		}
	}

private:
	inline static _CrtMemState _startMemState;
	};
}