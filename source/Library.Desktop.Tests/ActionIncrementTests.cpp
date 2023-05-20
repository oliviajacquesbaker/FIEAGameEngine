/// <summary>
/// Unit tests exercising the functionality of ActionIncrement
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DummyAction.h"
#include "ActionIncrement.h"
#include "Action.h"
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
	std::wstring ToString<ActionIncrement>(ActionIncrement* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<ActionIncrement>(const ActionIncrement* t) {
		RETURN_WIDE_STRING(t);
	}


	template<>
	std::wstring ToString<ActionIncrement>(const ActionIncrement& t) {
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
	class DummyIncrement final : public ActionIncrement {
		RTTI_DECLARATIONS(DummyIncrement, ActionIncrement);

	public:
		int extraInt;

		DummyIncrement(const std::string& name = "") :
			ActionIncrement(DummyIncrement::TypeIdClass(), name)
		{};

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "extraInt"s, Datum::DatumTypes::Integer, 1, offsetof(DummyIncrement, extraInt) },
			};
		}
	};
	RTTI_DEFINITIONS(DummyIncrement);
	CONCRETE_FACTORY(DummyIncrement, FIEAGameEngine::Scope)


	TEST_CLASS(ActionIncrementTests)
	{
	public:

		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();
			RegisterType<Action>();
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

		TEST_METHOD(ActionIncrementRTTIBasics)
		{
			{
				ActionIncrement a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ActionIncrement::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(ActionIncrement::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				ActionIncrement* s = rtti->As<ActionIncrement>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const ActionIncrement a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ActionIncrement::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(ActionIncrement::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const ActionIncrement* s = rtti->As<ActionIncrement>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(ActionIncrementRTTIOverrides)
		{
			ActionIncrement a("nameVal");
			RTTI* rtti = &a;

			std::string expected = "nameVal";
			Assert::AreEqual(expected, rtti->ToString());

			ActionIncrement e("nameVal");
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
			RegisterType<DummyIncrement, ActionIncrement>();
			Factory<Scope>::Add(make_unique<DummyIncrementFactory>());

			DummyIncrement a;
			RTTI* rtti = &a;
			Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
			Assert::IsTrue(rtti->Is(DummyIncrement::TypeIdClass()));
			Assert::IsTrue(rtti->Is(ActionIncrement::TypeIdClass()));
			Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
			Assert::AreEqual(DummyIncrement::TypeIdClass(), rtti->TypeIdInstance());
			Assert::AreEqual(0, a["extraInt"].GetAsInt());

			//prevent perceived memory leak
			TypeManager::Remove(DummyIncrement::TypeIdClass());
			Factory<Scope>::Remove("DummyIncrement"s);
		}

		TEST_METHOD(CopySemantics)
		{
			ActionIncrement action("test action");
			action.At("incrementAmount").Set(5);
			{
				ActionIncrement copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("incrementAmount").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ActionIncrement copy;
				copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("incrementAmount").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			ActionIncrement action("test action");
			action.At("incrementAmount").Set(5);
			{
				ActionIncrement movingCopy = action;
				ActionIncrement copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("incrementAmount").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ActionIncrement movingCopy = action;
				ActionIncrement copy;
				copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(5, copy.At("incrementAmount").GetAsInt());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(ActionIncrementClone)
		{
			ActionIncrement action("Test Action"s);

			auto clone = action.Clone();
			ActionIncrement* clonedGameObject = clone->As<ActionIncrement>();
			Assert::IsNotNull(clonedGameObject);
			Assert::AreNotSame(action, *clonedGameObject);
			Assert::AreEqual(action, *clonedGameObject);

			action["name"].Set("new name"s);
			Assert::AreEqual("new name"s, action.Name());
			Assert::AreNotEqual(action, *clonedGameObject);

			action["target"].Set("some variable");

			auto clone2 = action.Clone();
			ActionIncrement* clonedGameObject2 = clone2->As<ActionIncrement>();
			Assert::IsNotNull(clonedGameObject2);
			Assert::AreNotSame(action, *clonedGameObject2);
			Assert::AreEqual(action, *clonedGameObject2);
			Assert::AreEqual("new name"s, clonedGameObject2->Name());
			Assert::AreEqual("some variable"s, clonedGameObject2->Target());

			//prevent memory leaks
			delete clone;
			delete clone2;
		}

		TEST_METHOD(ActionIncrementUpdateDefault)
		{
			const string inputString = R"delim(
			{
				"A" :
				{
					"type": "Integer",
					"value": 3
				},
				"TestActionIncrement" :
				{
					"type": "Table",
					"class" : "ActionIncrement",
					"value":
					{
						"name":
						{
							"type": "String",
							"value": "test action increment"
						},
						"target":
						{
							"type": "String",
							"value": "A"
						},
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

			Assert::AreEqual(size_t(2), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("TestActionIncrement").Type());
			Assert::AreEqual(size_t(1), parentScope.At("TestActionIncrement").Size());

			Assert::IsTrue(parentScope.At("TestActionIncrement").GetAsTable().Is(ActionIncrement::TypeIdClass()));
			ActionIncrement* actionIncrement = parentScope.At("TestActionIncrement").GetAsTable().As<ActionIncrement>();
			Assert::AreEqual(size_t(5), actionIncrement->Size()); //this, name, target, target index, value

			Assert::AreEqual(Datum::DatumTypes::String, actionIncrement->At("name").Type());
			Assert::AreEqual(size_t(1), actionIncrement->At("name").Size());
			Assert::AreEqual("test action increment"s, actionIncrement->At("name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Integer, parentScope.At("A").Type());
			int& aInt = parentScope.At("A").GetAsInt();

			GameTime gameTime;
			
			Assert::AreEqual(3, aInt);
			actionIncrement->Update(gameTime);
			Assert::AreEqual(4, aInt);

			actionIncrement->Update(gameTime);
			actionIncrement->Update(gameTime);
			actionIncrement->Update(gameTime);
			Assert::AreEqual(7, aInt);
		}

		TEST_METHOD(ActionIncrementUpdateSpecial)
		{
			const string inputString = R"delim(
			{
				"A" :
				{
					"type": "Integer",
					"value": [3, 10]
				},
				"TestActionIncrement" :
				{
					"type": "Table",
					"class" : "ActionIncrement",
					"value":
					{
						"name":
						{
							"type": "String",
							"value": "test action increment"
						},
						"target":
						{
							"type": "String",
							"value": "A"
						},
						"incrementAmount":
						{
							"type": "Integer",
							"value": -2
						},
						"targetIndex":
						{
							"type": "Integer",
							"value": 1
						},
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

			Assert::AreEqual(size_t(2), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("TestActionIncrement").Type());
			Assert::AreEqual(size_t(1), parentScope.At("TestActionIncrement").Size());

			Assert::IsTrue(parentScope.At("TestActionIncrement").GetAsTable().Is(ActionIncrement::TypeIdClass()));
			ActionIncrement* actionIncrement = parentScope.At("TestActionIncrement").GetAsTable().As<ActionIncrement>();
			Assert::AreEqual(size_t(5), actionIncrement->Size()); //this, name, target, target index, value

			Assert::AreEqual(Datum::DatumTypes::String, actionIncrement->At("name").Type());
			Assert::AreEqual(size_t(1), actionIncrement->At("name").Size());
			Assert::AreEqual("test action increment"s, actionIncrement->At("name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Integer, parentScope.At("A").Type());
			int& aInt = parentScope.At("A").GetAsInt(1);

			GameTime gameTime;

			Assert::AreEqual(10, aInt);
			actionIncrement->Update(gameTime);
			Assert::AreEqual(8, aInt);

			actionIncrement->Update(gameTime);
			actionIncrement->Update(gameTime);
			actionIncrement->Update(gameTime);
			actionIncrement->Update(gameTime);
			Assert::AreEqual(0, aInt);

			Assert::AreEqual(3, parentScope.At("A").GetAsInt());
		}

		TEST_METHOD(ActionIncrementUpdateFailure)
		{
			const string inputString = R"delim(
			{
				"TestActionIncrement" :
				{
					"type": "Table",
					"class" : "ActionIncrement",
					"value":
					{
						"name":
						{
							"type": "String",
							"value": "test action increment"
						},
						"target":
						{
							"type": "String",
							"value": "doesntExist"
						},
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

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("TestActionIncrement").Type());
			Assert::AreEqual(size_t(1), parentScope.At("TestActionIncrement").Size());

			Assert::IsTrue(parentScope.At("TestActionIncrement").GetAsTable().Is(ActionIncrement::TypeIdClass()));
			ActionIncrement* actionIncrement = parentScope.At("TestActionIncrement").GetAsTable().As<ActionIncrement>();
			Assert::AreEqual(size_t(5), actionIncrement->Size()); //this, name, target, target index, value

			Assert::AreEqual(Datum::DatumTypes::String, actionIncrement->At("name").Type());
			Assert::AreEqual(size_t(1), actionIncrement->At("name").Size());

			GameTime gameTime;
			Assert::ExpectException<runtime_error>([&gameTime, &actionIncrement] {actionIncrement->Update(gameTime); });
		}

		TEST_METHOD(GetSetIncrementAmount)
		{
			{
				ActionIncrement actionDummy;
				Assert::AreEqual(1, actionDummy.IncrementAmount());
				actionDummy.SetIncrementAmount(2);
				Assert::AreEqual(2, actionDummy.IncrementAmount());
			}
			{
				const ActionIncrement actionDummy("test"s);
				Assert::AreEqual(1, actionDummy.IncrementAmount());
			}
		}

		TEST_METHOD(GetSetTargetIndex)
		{
			{
				ActionIncrement actionDummy;
				Assert::AreEqual(0, actionDummy.TargetIndex());
				actionDummy.SetTargetIndex(2);
				Assert::AreEqual(2, actionDummy.TargetIndex());
			}
			{
				const ActionIncrement actionDummy("test"s);
				Assert::AreEqual(0, actionDummy.TargetIndex());
			}
		}

		TEST_METHOD(GetSetTarget)
		{
			{
				ActionIncrement actionDummy;
				Assert::AreEqual(""s, actionDummy.Target());
				actionDummy.SetTarget("target test"s);
				Assert::AreEqual("target test"s, actionDummy.Target());
			}
			{
				const ActionIncrement actionDummy("test"s);
				Assert::AreEqual(""s, actionDummy.Target());
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}