/// <summary>
/// Unit tests exercising the functionality of DummyAction and a derived Action
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Action.h"
#include "DummyAction.h"
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
	std::wstring ToString<DummyAction>(DummyAction* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<DummyAction>(const DummyAction* t) {
		RETURN_WIDE_STRING(t);
	}


	template<>
	std::wstring ToString<DummyAction>(const DummyAction& t) {
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
		TEST_CLASS(ActionTests)
	{
	public:

		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();
			RegisterType<Action>();
			RegisterType<DummyAction, Action>();
			Factory<Scope>::Add(make_unique<DummyActionFactory>());
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

		TEST_METHOD(DummyActionRTTIBasics)
		{
			{
				DummyAction a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(DummyAction::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(DummyAction::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				DummyAction* s = rtti->As<DummyAction>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const DummyAction a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(DummyAction::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(DummyAction::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const DummyAction* s = rtti->As<DummyAction>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(DummyActionRTTIOverrides)
		{
			DummyAction a("nameVal");
			RTTI* rtti = &a;

			std::string expected = "nameVal";
			Assert::AreEqual(expected, rtti->ToString());

			DummyAction e("nameVal");
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

		TEST_METHOD(CopySemantics)
		{
			DummyAction action("test action");
			{
				DummyAction copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				DummyAction copy;
				copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			DummyAction action("test action");

			{
				DummyAction movingCopy = action;
				DummyAction copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				DummyAction movingCopy = action;
				DummyAction copy;
				copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(DummyClone)
		{
			DummyAction action("Test Action"s);

			auto clone = action.Clone();
			DummyAction* clonedGameObject = clone->As<DummyAction>();
			Assert::IsNotNull(clonedGameObject);
			Assert::AreNotSame(action, *clonedGameObject);
			Assert::AreEqual(action, *clonedGameObject);

			action["name"].Set("new name"s);
			Assert::AreEqual("new name"s, action.Name());
			Assert::AreNotEqual(action, *clonedGameObject);

			auto clone2 = action.Clone();
			DummyAction* clonedGameObject2 = clone2->As<DummyAction>();
			Assert::IsNotNull(clonedGameObject2);
			Assert::AreNotSame(action, *clonedGameObject2);
			Assert::AreEqual(action, *clonedGameObject2);
			Assert::AreEqual("new name"s, clonedGameObject2->Name());

			//prevent memory leaks
			delete clone;
			delete clone2;
		}

		TEST_METHOD(Update)
		{
			const string inputString = R"delim(
			{
				"TestAction" :
				{
					"type": "Table",
					"class" : "DummyAction",
					"value":
					{
						"name":
						{
							"type": "String",
							"value": "test action"
						}
					}
				}
				
			})delim"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);

			Assert::AreEqual(size_t(1), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("TestAction").Type());
			Assert::AreEqual(size_t(1), parentScope.At("TestAction").Size());

			Assert::IsTrue(parentScope.At("TestAction").GetAsTable().Is(DummyAction::TypeIdClass()));
			DummyAction* action = parentScope.At("TestAction").GetAsTable().As<DummyAction>();
			Assert::AreEqual(size_t(3), action->Size()); //this, name, updateNumber

			Assert::AreEqual(Datum::DatumTypes::String, action->At("name").Type());
			Assert::AreEqual(size_t(1), action->At("name").Size());
			Assert::AreEqual("test action"s, action->At("name").FrontAsString());

			GameTime gameTime;

			Assert::AreEqual(0, action->updateNumber);
			action->Update(gameTime);
			Assert::AreEqual(1, action->updateNumber);

			action->Update(gameTime);
			action->Update(gameTime);
			action->Update(gameTime);
			Assert::AreEqual(4, action->updateNumber);
		}

		TEST_METHOD(GetSetName)
		{
			{
				DummyAction actionDummy;
				Assert::AreEqual(""s, actionDummy.Name());
				actionDummy.SetName("test"s);
				Assert::AreEqual("test"s, actionDummy.Name());
			}
			{
				const DummyAction actionDummy("test"s);
				Assert::AreEqual("test"s, actionDummy.Name());
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}