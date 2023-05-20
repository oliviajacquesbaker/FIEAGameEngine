/// <summary>
/// Unit tests exercising the functionality of ActionWhile
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DummyAction.h"
#include "ActionWhile.h"
#include "ActionIncrement.h"
#include "JsonParseCoordinator.h"
#include "JsonTableParseHelper.h"
#include "Factory.h"
#include "GameTime.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace glm;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<ActionWhile>(ActionWhile* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<ActionWhile>(const ActionWhile* t) {
		RETURN_WIDE_STRING(t);
	}


	template<>
	std::wstring ToString<ActionWhile>(const ActionWhile& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Scope>(const Scope& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Datum::DatumTypes>(const Datum::DatumTypes& t) {
		std::string stringResult = Datum::typeToString.At(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace LibraryDesktopTests
{
	class DummyWhile final : public ActionWhile {
		RTTI_DECLARATIONS(DummyWhile, ActionWhile);

	public:
		int extraInt;

		DummyWhile(const std::string& name = "") :
			ActionWhile(DummyWhile::TypeIdClass(), name)
		{};

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "extraInt"s, Datum::DatumTypes::Integer, 1, offsetof(DummyWhile, extraInt) },
			};
		}
	};
	RTTI_DEFINITIONS(DummyWhile);
	CONCRETE_FACTORY(DummyWhile, FIEAGameEngine::Scope)

	TEST_CLASS(ActionWhileTests)
	{
	public:

		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();
			RegisterType<Action>();
			RegisterType<DummyAction, Action>();
			Factory<Scope>::Add(make_unique<DummyActionFactory>());
			RegisterType<ActionWhile, Action>();
			Factory<Scope>::Add(make_unique<ActionWhileFactory>());
			RegisterType<ActionIncrement, Action>();
			Factory<Scope>::Add(make_unique<ActionIncrementFactory>());
		}

		TEST_CLASS_CLEANUP(TestClassCleanup)
		{
			TypeManager::Clear();
			Factory<Scope>::Clear();
		}

		TEST_METHOD_INITIALIZE(Initialize)
		{
#if defined(DEBUG) || defined(_DEBUG)
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			_CrtMemCheckpoint(&_startMemState);
#endif
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
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

		TEST_METHOD(ActionWhileRTTIBasics)
		{
			{
				ActionWhile a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ActionWhile::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(ActionWhile::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				ActionWhile* s = rtti->As<ActionWhile>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const ActionWhile a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ActionWhile::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(ActionWhile::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const ActionWhile* s = rtti->As<ActionWhile>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(ActionWhileRTTIOverrides)
		{
			ActionWhile a("nameVal");
			RTTI* rtti = &a;

			std::string expected = "nameVal";
			Assert::AreEqual(expected, rtti->ToString());

			ActionWhile e("nameVal");
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
			RegisterType<DummyWhile, ActionWhile>();
			Factory<Scope>::Add(make_unique<DummyWhileFactory>());

			DummyWhile a;
			RTTI* rtti = &a;
			Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
			Assert::IsTrue(rtti->Is(DummyWhile::TypeIdClass()));
			Assert::IsTrue(rtti->Is(ActionWhile::TypeIdClass()));
			Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
			Assert::AreEqual(DummyWhile::TypeIdClass(), rtti->TypeIdInstance());
			Assert::AreEqual(0, a["extraInt"].GetAsInt());

			//prevent perceived memory leak
			TypeManager::Remove(DummyWhile::TypeIdClass());
			Factory<Scope>::Remove("DummyWhile"s);
		}

		TEST_METHOD(CopySemantics)
		{
			ActionWhile action("test action");
			action.At("condition").Set(5);
			{
				ActionWhile copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("condition").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ActionWhile copy;
				copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("condition").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			ActionWhile action("test action");
			action.At("condition").Set(5);
			{
				ActionWhile movingCopy = action;
				ActionWhile copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("condition").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ActionWhile movingCopy = action;
				ActionWhile copy;
				copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("condition").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(ActionWhileClone)
		{
			ActionWhile action("Test Action"s);

			auto clone = action.Clone();
			ActionWhile* clonedGameObject = clone->As<ActionWhile>();
			Assert::IsNotNull(clonedGameObject);
			Assert::AreNotSame(action, *clonedGameObject);
			Assert::AreEqual(action, *clonedGameObject);

			action["name"].Set("new name"s);
			Assert::AreEqual("new name"s, action.Name());
			Assert::AreNotEqual(action, *clonedGameObject);

			DummyAction* dummyAction = new DummyAction("dummy");
			action.Adopt(*dummyAction, "preamble");

			auto clone2 = action.Clone();
			ActionWhile* clonedGameObject2 = clone2->As<ActionWhile>();
			Assert::IsNotNull(clonedGameObject2);
			Assert::AreNotSame(action, *clonedGameObject2);
			Assert::AreEqual(action, *clonedGameObject2);
			Assert::AreEqual("new name"s, clonedGameObject2->Name());
			Assert::AreEqual(size_t(1), clone2->At("preamble").Size());

			//prevent memory leaks
			delete clone;
			delete clone2;
		}

		TEST_METHOD(ActionWhileUpdate)
		{
			const string fileName = R"(TestFiles\TestActionWhile.json)"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObjectFromFile(fileName);

			Assert::AreEqual(size_t(1), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("TestActionWhile").Type());
			Assert::AreEqual(size_t(1), parentScope.At("TestActionWhile").Size());

			Assert::IsTrue(parentScope.At("TestActionWhile").GetAsTable().Is(ActionWhile::TypeIdClass()));
			ActionWhile* actionWhile = parentScope.At("TestActionWhile").GetAsTable().As<ActionWhile>();
			Assert::AreEqual(size_t(6), actionWhile->Size()); //this, name, condition, preamble, loop, increment

			Assert::AreEqual(Datum::DatumTypes::String, actionWhile->At("name").Type());
			Assert::AreEqual(size_t(1), actionWhile->At("name").Size());
			Assert::AreEqual("test action while"s, actionWhile->At("name").FrontAsString());

			Assert::IsTrue(actionWhile->At("preamble").GetAsTable(0).Is(DummyAction::TypeIdClass()));
			DummyAction* dummy1 = actionWhile->At("preamble").GetAsTable(0).As<DummyAction>();
			Assert::IsTrue(actionWhile->At("loop").GetAsTable(0).Is(DummyAction::TypeIdClass()));
			DummyAction* dummy2 = actionWhile->At("loop").GetAsTable(0).As<DummyAction>();

			GameTime gameTime;

			Assert::AreEqual(0, dummy1->updateNumber);
			Assert::AreEqual(0, dummy2->updateNumber);

			//loops 5 times
			actionWhile->Update(gameTime);
			Assert::AreEqual(1, dummy1->updateNumber);
			Assert::AreEqual(5, dummy2->updateNumber);

			//never loops, condition unmet to begin with
			actionWhile->Update(gameTime);
			Assert::AreEqual(2, dummy1->updateNumber);
			Assert::AreEqual(5, dummy2->updateNumber);
		}

		TEST_METHOD(GetSetCondition)
		{
			{
				ActionWhile actionDummy;
				Assert::AreEqual(0, actionDummy.Condition());
				actionDummy.SetCondition(2);
				Assert::AreEqual(2, actionDummy.Condition());
			}
			{
				const ActionWhile actionDummy("test"s);
				Assert::AreEqual(0, actionDummy.Condition());
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}