/// <summary>
/// Unit tests exercising the functionality of Event (and the EventPublisher ABC)
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Event.h"
#include "EventQueue.h"
#include "JsonParseCoordinator.h"
#include "JsonTableParseHelper.h"
#include "Factory.h"
#include "GameState.h"
#include <windows.h> 
#include <iostream>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace glm;
using namespace std::string_literals;
using namespace std::placeholders;

namespace LibraryDesktopTests
{
	struct EventFooArgs : public EventArgs {
	public:
		EventFooArgs(std::int32_t data = 0) :
			foo{ data }
		{};

		bool operator==(const EventFooArgs& rhs) const {
			return (foo == rhs.foo);
		}

		Foo foo;
	};
	struct DummyFooSubscriber {
	public:
		void MemberDeliverFunction(EventPublisher& eventPublisher) {
			Event<EventFooArgs>* event = eventPublisher.As<Event<EventFooArgs>>();
			assert(event);

			data += event->Message().foo.Data();
		}

		void SubscribeToEventFoo() {
			Event<EventFooArgs>::Subscribe(delegate);
		}

		int Data() const {
			return data;
		}

	private:
		int data = 0;
		Delegate delegate = { std::bind(&DummyFooSubscriber::MemberDeliverFunction, this, _1) };
	};

	class IncrementingGameObject : public GameObject {
		RTTI_DECLARATIONS(IncrementingGameObject, GameObject);
	public:
		IncrementingGameObject(const std::string& name = "") :
			GameObject(IncrementingGameObject::TypeIdClass(), name)
		{}

		[[nodiscard]] gsl::owner<GameObject*> Clone() const override {
			return new IncrementingGameObject(*this);
		}

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "updateNumber"s, Datum::DatumTypes::Integer, 1, offsetof(IncrementingGameObject, updateNumber) }
			};
		}

		virtual void Update(const GameTime& gameTime) {
			GameObject::Update(gameTime);
			++updateNumber;
		}

		int updateNumber = 0;
	};
	RTTI_DEFINITIONS(IncrementingGameObject);
	CONCRETE_FACTORY(IncrementingGameObject, FIEAGameEngine::Scope)

	TEST_CLASS(GameStateTests)
	{
	public:
		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();

			RegisterType<GameObject>();
			Factory<Scope>::Add(make_unique<GameObjectFactory>());
			RegisterType<IncrementingGameObject, GameObject>();
			Factory<Scope>::Add(make_unique<IncrementingGameObjectFactory>());

			RegisterType<Action>();
		}

		TEST_CLASS_CLEANUP(TestClassCleanup)
		{
			TypeManager::Clear();
			Factory<Scope>::Clear();
		}

		TEST_METHOD_INITIALIZE(Initialize)
		{
			//start on clean slate
			Event<EventFooArgs>::RequestClearSubscribers();
			Event<int>::RequestClearSubscribers();
			GameState::RequestCleanup();

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

		TEST_METHOD(UpdateAllManualTimeControl)
		{
			GameTime& gameTime = GameState::GetGameTime();
			EventQueue& eventQueueTest = GameState::GetEventQueue();
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> aEvent(eventFoo);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime, 1ms);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime, 3ms);

			Assert::AreEqual(5, aEvent.Message().foo.Data());

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction = [&testingInt](EventPublisher& eventPublisher) {
				Event<EventFooArgs>* event = eventPublisher.As<Event<EventFooArgs>>();
				Assert::IsNotNull(event);
				testingInt += event->Message().foo.Data();
			};
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);

			IncrementingGameObject root;
			GameState::SetRootObject(root);
			GameState::SetGameClockEnabled(false);

			Assert::AreEqual(0, root.updateNumber);
			Assert::AreEqual(size_t(3), eventQueueTest.Size());
			GameState::Update();
			Assert::AreEqual(1, root.updateNumber);
			Assert::AreEqual(size_t(2), eventQueueTest.Size());
			Assert::AreEqual(5, testingInt);

			GameState::Update();
			Assert::AreEqual(2, root.updateNumber);
			Assert::AreEqual(size_t(2), eventQueueTest.Size());
			Assert::AreEqual(5, testingInt);

			gameTime.SetCurrentTime(gameTime.CurrentTime() + 1ms);
			GameState::Update();
			Assert::AreEqual(3, root.updateNumber);
			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			Assert::AreEqual(10, testingInt);

			gameTime.SetCurrentTime(gameTime.CurrentTime() + 5ms);
			GameState::Update();
			Assert::AreEqual(4, root.updateNumber);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(15, testingInt);

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
			GameState::RequestCleanup();
		}

		TEST_METHOD(UpdateAllAutoTimeControl)
		{
			GameTime& gameTime = GameState::GetGameTime();
			EventQueue& eventQueueTest = GameState::GetEventQueue();
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> aEvent(eventFoo);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime, 100ms);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime, 500ms);

			Assert::AreEqual(5, aEvent.Message().foo.Data());

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction = [&testingInt](EventPublisher& eventPublisher) {
				Event<EventFooArgs>* event = eventPublisher.As<Event<EventFooArgs>>();
				Assert::IsNotNull(event);
				testingInt += event->Message().foo.Data();
			};
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);

			IncrementingGameObject root;
			GameState::SetRootObject(root);
			GameState::SetGameClockEnabled(true);

			Assert::AreEqual(0, root.updateNumber);
			Assert::AreEqual(size_t(3), eventQueueTest.Size());
			auto currentTime = GameState::GetGameTime().ElapsedGameTime();
			Sleep(1000);
			GameState::Update();
			auto nowTime = GameState::GetGameTime().ElapsedGameTime();
			auto timeDiffBetweenExpectation = abs((GameState::GetGameTime().ElapsedGameTime() - currentTime) - 1000ms);
			Assert::IsTrue( timeDiffBetweenExpectation <= 100ms);
			Assert::AreEqual(1, root.updateNumber);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(15, testingInt);

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
			GameState::RequestCleanup();
		}

		TEST_METHOD(GetSetRootObject)
		{
			GameObject root("root"s);
			GameObject differentRoot("different"s);

			Assert::IsNull(GameState::GetRootObject());
			GameState::SetRootObject(root);
			Assert::AreEqual("root"s, GameState::GetRootObject()->Name());
			GameState::SetRootObject(differentRoot);
			Assert::AreEqual("different"s, GameState::GetRootObject()->Name());
		}

		TEST_METHOD(GetSetGameClockEnabled)
		{
			Assert::IsFalse(GameState::IsGameClockEnabled());
			GameState::SetGameClockEnabled(true);
			Assert::IsTrue(GameState::IsGameClockEnabled());
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}