/// <summary>
/// Unit tests exercising the functionality of ActionList
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DummyAction.h"
#include "ActionList.h"
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
	std::wstring ToString<ActionList>(ActionList* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<ActionList>(const ActionList* t) {
		RETURN_WIDE_STRING(t);
	}


	template<>
	std::wstring ToString<ActionList>(const ActionList& t) {
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
	class DummyList final : public ActionList {
		RTTI_DECLARATIONS(DummyList, ActionList);

	public:
		int extraInt;

		DummyList(const std::string& name = "") :
			ActionList(DummyList::TypeIdClass(), name)
		{};

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "extraInt"s, Datum::DatumTypes::Integer, 1, offsetof(DummyList, extraInt) },
			};
		}
	};
	RTTI_DEFINITIONS(DummyList);
	CONCRETE_FACTORY(DummyList, FIEAGameEngine::Scope)

	TEST_CLASS(ActionListTests)
	{
	public:

		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();
			RegisterType<Action>();
			RegisterType<DummyAction, Action>();
			Factory<Scope>::Add(make_unique<DummyActionFactory>());
			RegisterType<ActionList, Action>();
			Factory<Scope>::Add(make_unique<ActionListFactory>());
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

		TEST_METHOD(ActionListRTTIBasics)
		{
			{
				ActionList a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ActionList::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(ActionList::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				ActionList* s = rtti->As<ActionList>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const ActionList a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(ActionList::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(ActionList::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const ActionList* s = rtti->As<ActionList>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(ActionListRTTIOverrides)
		{
			ActionList a("nameVal");
			RTTI* rtti = &a;

			std::string expected = "nameVal";
			Assert::AreEqual(expected, rtti->ToString());

			ActionList e("nameVal");
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
			RegisterType<DummyList, ActionList>();
			Factory<Scope>::Add(make_unique<DummyListFactory>());

			DummyList a;
			RTTI* rtti = &a;
			Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
			Assert::IsTrue(rtti->Is(DummyList::TypeIdClass()));
			Assert::IsTrue(rtti->Is(ActionList::TypeIdClass()));
			Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
			Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
			Assert::AreEqual(DummyList::TypeIdClass(), rtti->TypeIdInstance());
			Assert::AreEqual(0, a["extraInt"].GetAsInt());

			//prevent perceived memory leak
			TypeManager::Remove(DummyList::TypeIdClass());
			Factory<Scope>::Remove("DummyList"s);
		}

		TEST_METHOD(CopySemantics)
		{
			ActionList action("test action");
			{
				ActionList copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ActionList copy;
				copy = action;
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			ActionList action("test action");

			{
				ActionList movingCopy = action;
				ActionList copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
			{
				ActionList movingCopy = action;
				ActionList copy;
				copy = std::move(movingCopy);
				Assert::AreEqual(action.Size(), copy.Size());
				Assert::AreEqual("test action"s, copy.At("name").GetAsString());
				Assert::AreEqual(action, copy);
				Assert::AreNotSame(action, copy);
			}
		}

		TEST_METHOD(ActionListClone)
		{
			ActionList action("Test Action"s);

			auto clone = action.Clone();
			ActionList* clonedGameObject = clone->As<ActionList>();
			Assert::IsNotNull(clonedGameObject);
			Assert::AreNotSame(action, *clonedGameObject);
			Assert::AreEqual(action, *clonedGameObject);

			action["name"].Set("new name"s);
			Assert::AreEqual("new name"s, action.Name());
			Assert::AreNotEqual(action, *clonedGameObject);

			DummyAction* dummyAction = new DummyAction("dummy");
			action.Adopt(*dummyAction, "actions");

			auto clone2 = action.Clone();
			ActionList* clonedGameObject2 = clone2->As<ActionList>();
			Assert::IsNotNull(clonedGameObject2);
			Assert::AreNotSame(action, *clonedGameObject2);
			Assert::AreEqual(action, *clonedGameObject2);
			Assert::AreEqual("new name"s, clonedGameObject2->Name());
			Assert::AreEqual(size_t(1), clone2->At("actions").Size());

			//prevent memory leaks
			delete clone;
			delete clone2;
		}

		TEST_METHOD(ActionListUpdate)
		{
			const string inputString = R"delim(
			{
				"TestActionList" :
				{
					"type": "Table",
					"class" : "ActionList",
					"value":
					{
						"name":
						{
							"type": "String",
							"value": "test action list"
						},
						"actions":
						{
							"type": "Table",
							"value":
							[
								{
									"type": "Table",
									"class" : "DummyAction",
									"value":
									{
										"name":
										{
											"type": "String",
											"value": "dummy 1"
										}
									}
								},
								{
									"type": "Table",
									"class" : "DummyAction",
									"value":
									{
										"name":
										{
											"type": "String",
											"value": "dummy 2"
										}
									}
								},
							]
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

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("TestActionList").Type());
			Assert::AreEqual(size_t(1), parentScope.At("TestActionList").Size());

			Assert::IsTrue(parentScope.At("TestActionList").GetAsTable().Is(ActionList::TypeIdClass()));
			ActionList* actionList = parentScope.At("TestActionList").GetAsTable().As<ActionList>();
			Assert::AreEqual(size_t(3), actionList->Size()); //this, name, updateNumber

			Assert::AreEqual(Datum::DatumTypes::String, actionList->At("name").Type());
			Assert::AreEqual(size_t(1), actionList->At("name").Size());
			Assert::AreEqual("test action list"s, actionList->At("name").FrontAsString());

			Assert::IsTrue(actionList->At("actions").GetAsTable(0).Is(DummyAction::TypeIdClass()));
			DummyAction* dummy1 = actionList->At("actions").GetAsTable(0).As<DummyAction>();
			Assert::IsTrue(actionList->At("actions").GetAsTable(1).Is(DummyAction::TypeIdClass()));
			DummyAction* dummy2 = actionList->At("actions").GetAsTable(1).As<DummyAction>();

			GameTime gameTime;

			Assert::AreEqual(0, dummy1->updateNumber);
			Assert::AreEqual(0, dummy2->updateNumber);
			actionList->Update(gameTime);
			Assert::AreEqual(1, dummy1->updateNumber);
			Assert::AreEqual(1, dummy2->updateNumber);

			actionList->Update(gameTime);
			actionList->Update(gameTime);
			actionList->Update(gameTime);
			Assert::AreEqual(4, dummy1->updateNumber);
			Assert::AreEqual(4, dummy2->updateNumber);
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}