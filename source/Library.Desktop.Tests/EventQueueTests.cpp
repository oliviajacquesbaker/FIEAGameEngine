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
#include "GameTime.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace glm;
using namespace std::string_literals;
using namespace std::placeholders;

//test EventArgs struct using Foo and DummyFooSubscriber class to test class method callbacks
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
}

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<EventQueue>(const EventQueue& t) {
		RETURN_WIDE_STRING(&t);
	}
}

namespace LibraryDesktopTests
{
	TEST_CLASS(EventQueueTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Initialize)
		{
			//start on clean slate
			Event<EventFooArgs>::RequestClearSubscribers();
			Event<int>::RequestClearSubscribers();

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

		TEST_METHOD(CopySemantics)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> aEvent(eventFoo);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);
			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			{
				EventQueue copy = eventQueueTest;
				Assert::AreEqual(eventQueueTest.Size(), copy.Size());
				Assert::AreEqual(size_t(1), copy.Size());
				Assert::AreNotSame(eventQueueTest, copy);
			}
			{
				GameTime gameTime2;
				EventQueue copy;
				copy = eventQueueTest;
				Assert::AreEqual(eventQueueTest.Size(), copy.Size());
				Assert::AreEqual(size_t(1), copy.Size());
				Assert::AreNotSame(eventQueueTest, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> aEvent(eventFoo);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);
			{
				EventQueue movingCopy = eventQueueTest;
				EventQueue copy = std::move(movingCopy);
				Assert::AreEqual(eventQueueTest.Size(), copy.Size());
				Assert::AreEqual(size_t(1), copy.Size());
				Assert::AreNotSame(eventQueueTest, copy);
			}
			{
				EventQueue movingCopy = eventQueueTest;
				GameTime gameTime2;
				EventQueue copy;
				copy = std::move(movingCopy);
				Assert::AreEqual(eventQueueTest.Size(), copy.Size());
				Assert::AreEqual(size_t(1), copy.Size());
				Assert::AreNotSame(eventQueueTest, copy);
			}
		}

		TEST_METHOD(UpdateNoDelays)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> aEvent(eventFoo);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);

			Assert::AreEqual(5, aEvent.Message().foo.Data());

			int testingInt = 3;
			std::function<void(EventPublisher&)> dummySubscriberFunction = [&testingInt](EventPublisher& eventPublisher) {
				Event<EventFooArgs>* event = eventPublisher.As<Event<EventFooArgs>>();
				Assert::IsNotNull(event);
				testingInt += event->Message().foo.Data();
			};
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);

			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(5, aEvent.Message().foo.Data());
			Assert::AreEqual(8, testingInt);

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(UpdateSomeDelays)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
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

			Assert::AreEqual(size_t(3), eventQueueTest.Size());
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(2), eventQueueTest.Size());
			Assert::AreEqual(5, testingInt);

			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(2), eventQueueTest.Size());
			Assert::AreEqual(5, testingInt);

			gameTime.SetCurrentTime(gameTime.CurrentTime() + 1ms);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			Assert::AreEqual(10, testingInt);

			gameTime.SetCurrentTime(gameTime.CurrentTime() + 5ms);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(15, testingInt);

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(UpdateInterrupt)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> aEvent(eventFoo);
			eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);

			Assert::AreEqual(5, aEvent.Message().foo.Data());

			int testingInt = 3;
			std::function<void(EventPublisher&)> updateSubscriberFunction = [&eventQueueTest, &gameTime](EventPublisher& eventPublisher) { eventQueueTest.Update(gameTime); UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> dummySubscriberFunction = [&testingInt](EventPublisher& eventPublisher) {
				Event<EventFooArgs>* event = eventPublisher.As<Event<EventFooArgs>>();
				Assert::IsNotNull(event);
				testingInt += event->Message().foo.Data();
			};
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);
			Delegate del2 = { updateSubscriberFunction };
			Event<EventFooArgs>::Subscribe(del2);

			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(5, aEvent.Message().foo.Data());
			Assert::AreEqual(8, testingInt);

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(GetSize)
		{
			{
				GameTime gameTime;
				EventQueue eventQueueTest{};
				Event<EventFooArgs> aEvent{ EventFooArgs() };
				Assert::AreEqual(size_t(0), eventQueueTest.Size());
				eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);
				Assert::AreEqual(size_t(1), eventQueueTest.Size());
			}
			{
				GameTime gameTime;
				const EventQueue eventQueueTest{};
				Event<EventFooArgs> aEvent{ EventFooArgs() };
				Assert::AreEqual(size_t(0), eventQueueTest.Size());
			}
		}

		TEST_METHOD(IsEmpty)
		{
			{
				GameTime gameTime;
				EventQueue eventQueueTest{};
				Event<EventFooArgs> aEvent{ EventFooArgs() };
				Assert::IsTrue(eventQueueTest.IsEmpty());
				eventQueueTest.Enqueue(make_shared<Event<EventFooArgs>>(aEvent), gameTime);
				Assert::IsFalse(eventQueueTest.IsEmpty());
			}
			{
				GameTime gameTime;
				const EventQueue eventQueueTest{};
				Event<EventFooArgs> aEvent{ EventFooArgs() };
				Assert::IsTrue(eventQueueTest.IsEmpty());
			}
		}

		TEST_METHOD(EnqueueDequeueBasic)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			Event<EventFooArgs> aEvent(EventFooArgs(0));

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction1 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction1 };
			Event<EventFooArgs>::Subscribe(del1);

			Assert::AreEqual(0, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(0, testingInt);

			auto event1 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event1, gameTime);
			Assert::AreEqual(0, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(1, testingInt);

			eventQueueTest.Enqueue(event1, gameTime);
			auto event2 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event2, gameTime);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(2, testingInt);

			eventQueueTest.Enqueue(event1, gameTime);
			eventQueueTest.Enqueue(event2, gameTime);
			eventQueueTest.Dequeue(event2);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(1, testingInt);

			eventQueueTest.Enqueue(event2, gameTime);
			eventQueueTest.Dequeue(event2);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(0, testingInt);

			//nothing bad happens if dequeing nonexistant...
			eventQueueTest.Dequeue(event2);

			//prevent memory leaks
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(EnqueueDequeueInterrupt)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			Event<EventFooArgs> aEvent(EventFooArgs(0));

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction1 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction1 };
			Event<EventFooArgs>::Subscribe(del1);

			auto event1 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event1, gameTime);
			Assert::AreEqual(0, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(1, testingInt);

			std::function<void(EventPublisher&)> enqueueFunction = [&event1, &eventQueueTest, &gameTime](EventPublisher& eventPublisher) { eventQueueTest.Enqueue(event1, gameTime); UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> dequeueFunction = [&event1, &eventQueueTest](EventPublisher& eventPublisher) { eventQueueTest.Dequeue(event1); UNREFERENCED_LOCAL(eventPublisher); };

			Delegate del2 = { enqueueFunction };
			Event<EventFooArgs>::Subscribe(del2);
			eventQueueTest.Enqueue(event1, gameTime);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			eventQueueTest.Update(gameTime);
			Event<EventFooArgs>::Unsubscribe(del2);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(1, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(2, testingInt);
			eventQueueTest.Update(gameTime);

			Delegate del3 = { dequeueFunction };
			Event<int>::Subscribe(del3);
			Event<int> bEvent(0);
			auto event3 = make_shared<Event<int>>(bEvent);
			eventQueueTest.Enqueue(event3, gameTime);
			eventQueueTest.Enqueue(event1, gameTime, 5ms);

			testingInt = 0;
			Assert::AreEqual(size_t(2), eventQueueTest.Size());
			Assert::AreEqual(0, testingInt);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			Assert::AreEqual(0, testingInt);
			gameTime.SetCurrentTime(gameTime.CurrentTime() + 10ms);
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());
			Assert::AreEqual(0, testingInt);

			//prevent memory leaks
			Event<EventFooArgs>::RequestClearSubscribers();
			Event<int>::RequestClearSubscribers();
		}

		TEST_METHOD(ClearQueueBasic)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			Event<EventFooArgs> aEvent(EventFooArgs(0));

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction1 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction1 };
			Event<EventFooArgs>::Subscribe(del1);

			auto event1 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event1, gameTime);

			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			eventQueueTest.Clear();
			Assert::AreEqual(size_t(0), eventQueueTest.Size());

			eventQueueTest.Enqueue(event1, gameTime);
			auto event2 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event2, gameTime);
			auto event3 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event3, gameTime);

			Assert::AreEqual(size_t(3), eventQueueTest.Size());
			eventQueueTest.Clear();
			Assert::AreEqual(size_t(0), eventQueueTest.Size());

			//prevent memory leaks
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(ClearQueueInterrupt)
		{
			GameTime gameTime;
			EventQueue eventQueueTest{};
			Event<EventFooArgs> aEvent{ EventFooArgs(0) };
			Event<int> bEvent(0);

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction1 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> clearQueueFunction = [&eventQueueTest](EventPublisher& eventPublisher) { eventQueueTest.Clear(); UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction1 };
			Event<EventFooArgs>::Subscribe(del1);

			auto event1 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event1, gameTime);
			auto event2 = make_shared<Event<EventFooArgs>>(aEvent);
			eventQueueTest.Enqueue(event2, gameTime, 5ms);

			Assert::AreEqual(size_t(2), eventQueueTest.Size());
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(1), eventQueueTest.Size());

			Delegate del2 = { clearQueueFunction };
			Event<int>::Subscribe(del2);
			auto event3 = make_shared<Event<int>>(bEvent);
			eventQueueTest.Enqueue(event3, gameTime);

			Assert::AreEqual(size_t(2), eventQueueTest.Size());
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(1), eventQueueTest.Size());
			eventQueueTest.Update(gameTime);
			Assert::AreEqual(size_t(0), eventQueueTest.Size());

			Event<EventFooArgs>::RequestClearSubscribers();
			Event<int>::RequestClearSubscribers();
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}