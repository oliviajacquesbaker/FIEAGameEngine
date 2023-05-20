/// <summary>
/// Unit tests exercising the functionality of GameObject and a derived GameObject
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "GameObject.h"
#include "JsonParseCoordinator.h"
#include "JsonTableParseHelper.h"
#include "Factory.h"
#include "ActionList.h"
#include "DummyAction.h"
#include "GameTime.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace glm;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<GameObject>(GameObject* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<GameObject>(const GameObject* t) {
		RETURN_WIDE_STRING(t);
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

	template<>
	std::wstring ToString<vec4>(const vec4& t) {
		std::string stringResult = glm::to_string(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<GameObject>(const GameObject& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace LibraryDesktopTests
{
	class DummyGameObject : public GameObject {
		RTTI_DECLARATIONS(DummyGameObject, GameObject);
	public:
		DummyGameObject(const std::string& name = "") :
			GameObject(DummyGameObject::TypeIdClass(), name)
		{}

		[[nodiscard]] gsl::owner<GameObject*> Clone() const override {
			return new DummyGameObject(*this);
		}

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "updateNumber"s, Datum::DatumTypes::Integer, 1, offsetof(DummyGameObject, updateNumber) }
			};
		}

		virtual void Update(const GameTime& gameTime) {
			GameObject::Update(gameTime);
			++updateNumber;
		}

		int updateNumber = 0;
	};
	RTTI_DEFINITIONS(DummyGameObject);
	CONCRETE_FACTORY(DummyGameObject, FIEAGameEngine::Scope)

	TEST_CLASS(GameObjectTests)
	{
	public:

		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();

			RegisterType<GameObject>();
			Factory<Scope>::Add(make_unique<GameObjectFactory>());
			RegisterType<DummyGameObject, GameObject>();
			Factory<Scope>::Add(make_unique<DummyGameObjectFactory>());

			RegisterType<Action>();
			RegisterType<ActionList, Action>();
			Factory<Scope>::Add(make_unique<ActionListFactory>());
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

		TEST_METHOD(GameObjectRTTIBasics)
		{
			{
				GameObject a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(GameObject::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(GameObject::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				GameObject* s = rtti->As<GameObject>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const GameObject a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(GameObject::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(GameObject::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const GameObject* s = rtti->As<GameObject>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(GameObjectRTTIOverrides)
		{
			GameObject a("nameVal");
			RTTI* rtti = &a;

			Assert::AreEqual(a.Name(), rtti->ToString());

			GameObject e("nameVal");
			RTTI* eRtti = &e;
			Bar d;
			RTTI* dRtti = &d;

			Assert::IsTrue(rtti->Equals(eRtti));
			Assert::IsFalse(rtti->Equals(dRtti));
			Assert::IsFalse(rtti->Equals(nullptr));

			a.Append("key2") = { "A"s, "B"s };
			Assert::AreEqual(a.Name(), rtti->ToString());
			Assert::IsFalse(rtti->Equals(eRtti));
		}

		TEST_METHOD(CopySemantics)
		{
			GameObject gameObject("test game object");
			gameObject.SetPosition(vec4(1));

			{
				GameObject copy = gameObject;
				Assert::AreEqual(gameObject.Size(), copy.Size());
				Assert::AreEqual("test game object"s, copy.At("name").GetAsString());
				Assert::AreEqual(gameObject, copy);
				Assert::AreNotSame(gameObject, copy);
			}
			{
				GameObject copy;
				copy = gameObject;
				Assert::AreEqual(gameObject.Size(), copy.Size());
				Assert::AreEqual("test game object"s, copy.At("name").GetAsString());
				Assert::AreEqual(gameObject, copy);
				Assert::AreNotSame(gameObject, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			GameObject gameObject("test game object");
			gameObject.SetPosition(vec4(1));

			{
				GameObject movingCopy = gameObject;
				GameObject copy = std::move(movingCopy);
				Assert::AreEqual(gameObject.Size(), copy.Size());
				Assert::AreEqual("test game object"s, copy.At("name").GetAsString());
				Assert::AreEqual(gameObject, copy);
				Assert::AreNotSame(gameObject, copy);
			}
			{
				GameObject movingCopy = gameObject;
				GameObject copy;
				copy = std::move(movingCopy);
				Assert::AreEqual(gameObject.Size(), copy.Size());
				Assert::AreEqual("test game object"s, copy.At("name").GetAsString());
				Assert::AreEqual(gameObject, copy);
				Assert::AreNotSame(gameObject, copy);
			}
		}

		TEST_METHOD(Clone)
		{
			GameObject gameObject("Test GameObject"s);

			auto clone = gameObject.Clone();
			GameObject* clonedGameObject = clone->As<GameObject>();
			Assert::IsNotNull(clonedGameObject);
			Assert::AreNotSame(gameObject, *clonedGameObject);
			Assert::AreEqual(gameObject, *clonedGameObject);

			gameObject["name"].Set("new name"s);
			Assert::AreEqual("new name"s, gameObject.Name());
			Assert::AreNotEqual(gameObject, *clonedGameObject);

			auto clone2 = gameObject.Clone();
			GameObject* clonedGameObject2 = clone2->As<GameObject>();
			Assert::IsNotNull(clonedGameObject2);
			Assert::AreNotSame(gameObject, *clonedGameObject2);
			Assert::AreEqual(gameObject, *clonedGameObject2);
			Assert::AreEqual("new name"s, clonedGameObject2->Name());

			//prevent memory leaks
			delete clone;
			delete clone2;
		}

		TEST_METHOD(Update)
		{
			const string fileName = R"(TestFiles\TestGameObject.json)"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObjectFromFile(fileName);

			Assert::AreEqual(size_t(1), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("TestGameObject").Type());
			Assert::AreEqual(size_t(1), parentScope.At("TestGameObject").Size());

			Scope& scope = parentScope.At("TestGameObject").GetAsTable();
			Assert::AreEqual(size_t(7), scope.Size()); //this, name, pos, rot, scale, children, actions

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("name").Type());
			Assert::AreEqual(size_t(1), scope.At("name").Size());
			Assert::AreEqual("test game object"s, scope.At("name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("position").Type());
			Assert::AreEqual(size_t(1), scope.At("position").Size());
			Assert::AreEqual(vec4(0), scope.At("position").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Table, scope.At("children").Type());

			Assert::IsTrue(scope.At("children").GetAsTable(0).Is(GameObject::TypeIdClass()));

			Assert::IsTrue(scope.At("children").GetAsTable(0).At("children").GetAsTable().Is(DummyGameObject::TypeIdClass()));
			DummyGameObject* dummyGameObject1 = scope.At("children").GetAsTable(0).At("children").GetAsTable().As<DummyGameObject>();
			Assert::IsTrue(scope.At("children").GetAsTable(1).Is(DummyGameObject::TypeIdClass()));
			DummyGameObject* dummyGameObject2 = scope.At("children").GetAsTable(1).As<DummyGameObject>();
			Assert::AreEqual("test dummy game object"s, dummyGameObject1->Name());
			Assert::AreEqual("test dummy game object 2"s, dummyGameObject2->Name());		

			Assert::IsTrue(scope.At("children").GetAsTable(0).At("actions").GetAsTable(1).Is(DummyAction::TypeIdClass()));
			DummyAction* dummyAction1 = scope.At("children").GetAsTable(0).At("actions").GetAsTable(1).As<DummyAction>();
			Assert::IsTrue(scope.At("children").GetAsTable(0).At("actions").GetAsTable().At("actions").GetAsTable(0).Is(DummyAction::TypeIdClass()));
			DummyAction* dummyAction2 = scope.At("children").GetAsTable(0).At("actions").GetAsTable().At("actions").GetAsTable(0).As<DummyAction>();
			Assert::IsTrue(scope.At("children").GetAsTable(0).At("actions").GetAsTable().At("actions").GetAsTable(0).Is(DummyAction::TypeIdClass()));
			DummyAction* dummyAction3 = scope.At("children").GetAsTable(0).At("actions").GetAsTable().At("actions").GetAsTable(1).As<DummyAction>();
			Assert::AreEqual("dummy out"s, dummyAction1->Name());
			Assert::AreEqual("dummy 1"s, dummyAction2->Name());
			Assert::AreEqual("dummy 2"s, dummyAction3->Name());

			GameObject* parentGameObject = scope.As<GameObject>();
			GameTime gameTime;

			Assert::AreEqual(0, dummyGameObject1->updateNumber);
			Assert::AreEqual(0, dummyGameObject2->updateNumber);
			Assert::AreEqual(0, dummyAction1->updateNumber);
			Assert::AreEqual(0, dummyAction2->updateNumber);
			Assert::AreEqual(0, dummyAction3->updateNumber);
			parentGameObject->Update(gameTime);
			Assert::AreEqual(1, dummyGameObject1->updateNumber);
			Assert::AreEqual(1, dummyGameObject2->updateNumber);
			Assert::AreEqual(1, dummyAction1->updateNumber);
			Assert::AreEqual(1, dummyAction2->updateNumber);
			Assert::AreEqual(1, dummyAction3->updateNumber);

			parentGameObject->Update(gameTime);
			parentGameObject->Update(gameTime);
			parentGameObject->Update(gameTime);
			Assert::AreEqual(4, dummyGameObject1->updateNumber);
			Assert::AreEqual(4, dummyGameObject2->updateNumber);
			Assert::AreEqual(4, dummyAction1->updateNumber);
			Assert::AreEqual(4, dummyAction2->updateNumber);
			Assert::AreEqual(4, dummyAction3->updateNumber);
		}

		TEST_METHOD(CreateChild)
		{
			GameObject gameObject;
			Assert::AreEqual(size_t(0), gameObject.At("children").Size());

			gameObject.CreateChild("DummyGameObject"s, "dummy object"s);

			Assert::AreEqual(size_t(1), gameObject.At("children").Size());
			DummyGameObject* dummy = (&gameObject.At("children").GetAsTable())->As<DummyGameObject>();
			Assert::AreEqual("dummy object"s, dummy->Name());
			Assert::AreEqual(0, dummy->updateNumber);

			Assert::ExpectException<runtime_error>([&gameObject] {gameObject.CreateChild("DoesntExist", "dummy"); });
		}

		TEST_METHOD(CreateAction)
		{
			GameObject gameObject;
			Assert::AreEqual(size_t(0), gameObject.At("actions").Size());

			gameObject.CreateAction("DummyAction"s, "dummy action"s);

			Assert::AreEqual(size_t(1), gameObject.At("actions").Size());
			DummyAction* dummy = (&gameObject.At("actions").GetAsTable())->As<DummyAction>();
			Assert::AreEqual("dummy action"s, dummy->Name());
			Assert::AreEqual(0, dummy->updateNumber);

			Assert::ExpectException<runtime_error>([&gameObject] {gameObject.CreateAction("DoesntExist", "dummy"); });
		}

		TEST_METHOD(GetActions)
		{
			GameObject gameObjectDummy;
			gameObjectDummy.CreateAction("DummyAction"s, "dummy action"s);

			const GameObject gameObject = gameObjectDummy;
			Assert::AreEqual(size_t(1), gameObject.Actions().Size());
			const DummyAction* dummy = (&gameObject.Actions().GetAsTable())->As<DummyAction>();
			Assert::AreEqual("dummy action"s, dummy->Name());
			Assert::AreEqual(0, dummy->updateNumber);
		}

		TEST_METHOD(GetSetName)
		{
			{
				GameObject gameObjectDummy;
				Assert::AreEqual(""s, gameObjectDummy.Name());
				gameObjectDummy.SetName("test"s);
				Assert::AreEqual("test"s, gameObjectDummy.Name());
			}
			{
				const GameObject gameObjectDummy("test"s);
				Assert::AreEqual("test"s, gameObjectDummy.Name());
			}
		}

		TEST_METHOD(GetSetPosition)
		{
			{
				GameObject gameObjectDummy;
				Assert::AreEqual(vec4(0), gameObjectDummy.Position());
				gameObjectDummy.SetPosition(vec4(5));
				Assert::AreEqual(vec4(5), gameObjectDummy.Position());
			}
			{
				const GameObject gameObjectDummy;
				Assert::AreEqual(vec4(0), gameObjectDummy.Position());
			}
		}

		TEST_METHOD(GetSetRotation)
		{
			{
				GameObject gameObjectDummy;
				Assert::AreEqual(vec4(0), gameObjectDummy.Rotation());
				gameObjectDummy.SetRotation(vec4(5));
				Assert::AreEqual(vec4(5), gameObjectDummy.Rotation());
			}
			{
				const GameObject gameObjectDummy;
				Assert::AreEqual(vec4(0), gameObjectDummy.Rotation());
			}
		}

		TEST_METHOD(GetSetScale)
		{
			{
				GameObject gameObjectDummy;
				Assert::AreEqual(vec4(0), gameObjectDummy.Scale());
				gameObjectDummy.SetScale(vec4(5));
				Assert::AreEqual(vec4(5), gameObjectDummy.Scale());
			}
			{
				const GameObject gameObjectDummy;
				Assert::AreEqual(vec4(0), gameObjectDummy.Scale());
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}