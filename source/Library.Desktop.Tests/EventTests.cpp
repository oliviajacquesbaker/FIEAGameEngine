/// <summary>
/// Unit tests exercising the functionality of Event (and the EventPublisher ABC)
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Event.h"
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
	std::wstring ToString<LibraryDesktopTests::EventFooArgs>(const LibraryDesktopTests::EventFooArgs& t) {
		std::string stringResult = t.foo.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Event<LibraryDesktopTests::EventFooArgs>>(Event<LibraryDesktopTests::EventFooArgs>* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<Event<LibraryDesktopTests::EventFooArgs>>(const Event<LibraryDesktopTests::EventFooArgs>* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<Event<LibraryDesktopTests::EventFooArgs>>(const Event<LibraryDesktopTests::EventFooArgs>& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace LibraryDesktopTests
{
	TEST_CLASS(EventTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Initialize)
		{
			//start on clean slate
			Event<EventFooArgs>::RequestClearSubscribers();

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

		TEST_METHOD(RTTIBasics)
		{
			{
				EventFooArgs eventFoo = 5;
				Event<EventFooArgs> a(eventFoo);

				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Event<EventFooArgs>::TypeIdClass()));
				Assert::IsTrue(rtti->Is(EventPublisher::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(Event<EventFooArgs>::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				Event<EventFooArgs>* s = rtti->As<Event<EventFooArgs>>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				EventPublisher* s2 = rtti->As<EventPublisher>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const EventFooArgs eventFoo = 5;
				const Event<EventFooArgs> a(eventFoo);
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Event<EventFooArgs>::TypeIdClass()));
				Assert::IsTrue(rtti->Is(EventPublisher::TypeIdClass()));
				Assert::AreEqual(Event<EventFooArgs>::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const Event<EventFooArgs>* s = rtti->As<Event<EventFooArgs>>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const EventPublisher* s2 = rtti->As<EventPublisher>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(RTTIOverrides)
		{
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> a(eventFoo);
			RTTI* rtti = &a;

			Assert::AreEqual("Event with 0 subscribers."s, rtti->ToString());

			Event<EventFooArgs> e(eventFoo);
			RTTI* eRtti = &e;
			Bar d;
			RTTI* dRtti = &d;

			//Assert::IsTrue(rtti->Equals(eRtti)); TODO: delete if we keep the default ==
			Assert::IsFalse(rtti->Equals(eRtti)); //TODO: delete if we OVERRIDE the ==
			Assert::IsFalse(rtti->Equals(dRtti));
			Assert::IsFalse(rtti->Equals(nullptr));

			std::function<void(EventPublisher&)> dummySubscriberFunction = [](EventPublisher& eventPublisher) {UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);
			Assert::AreEqual("Event with 1 subscribers."s, rtti->ToString());

			//prevent static memory leak
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(CopySemantics)
		{
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> eventTest(eventFoo);
			Event<Bar> eventTest2{Bar()};
			{
				Event<EventFooArgs> copy = eventTest;
				Assert::AreEqual(eventTest.Message(), copy.Message());
				Assert::AreEqual(Foo(5), copy.Message().foo);
				Assert::AreNotSame(eventTest, copy);
			}
			{
				Event<EventFooArgs> copy{ EventFooArgs() };
				copy = eventTest;
				Assert::AreEqual(eventTest.Message(), copy.Message());
				Assert::AreEqual(Foo(5), copy.Message().foo);
				Assert::AreNotSame(eventTest, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			EventFooArgs eventFoo = 5;
			Event<EventFooArgs> eventTest(eventFoo);
			{
				Event<EventFooArgs> movingCopy = eventTest;
				Event<EventFooArgs> copy = std::move(movingCopy);
				Assert::AreEqual(eventTest.Message(), copy.Message());
				Assert::AreEqual(Foo(5), copy.Message().foo);
				Assert::AreNotSame(eventTest, copy);
			}
			{
				Event<EventFooArgs> movingCopy = eventTest;
				Event<EventFooArgs> copy{ EventFooArgs() };
				copy = std::move(movingCopy);
				Assert::AreEqual(eventTest.Message(), copy.Message());
				Assert::AreEqual(Foo(5), copy.Message().foo);
				Assert::AreNotSame(eventTest, copy);
			}
		}

		TEST_METHOD(DeliverLambdaFunction)
		{
			GameTime gameTime;
			Event<EventFooArgs> eventTest{ EventFooArgs(5) };
			Event<EventFooArgs> eventTest2{ EventFooArgs(2) };

			Assert::AreEqual(5, eventTest.Message().foo.Data());
			Assert::AreEqual(2, eventTest2.Message().foo.Data());

			int testingInt = 3;
			std::function<void(EventPublisher&)> dummySubscriberFunction = [&testingInt](EventPublisher& eventPublisher) {
				Event<EventFooArgs>* event = eventPublisher.As<Event<EventFooArgs>>();
				Assert::IsNotNull(event);
				testingInt += event->Message().foo.Data();
			};
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);

			eventTest.Deliver();
			Assert::AreEqual(5, eventTest.Message().foo.Data());
			Assert::AreEqual(8, testingInt);
			eventTest2.Deliver();
			Assert::AreEqual(2, eventTest2.Message().foo.Data());
			Assert::AreEqual(10, testingInt);

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(DeliverMemberFunction)
		{
			GameTime gameTime;
			Event<EventFooArgs> eventTest{ EventFooArgs(5) };
			Event<EventFooArgs> eventTest2{ EventFooArgs(2) };

			Assert::AreEqual(5, eventTest.Message().foo.Data());
			Assert::AreEqual(2, eventTest2.Message().foo.Data());

			DummyFooSubscriber fooSubscriber;
			std::function<void(EventPublisher&)> dummySubscriberFunction = std::bind(&DummyFooSubscriber::MemberDeliverFunction, &fooSubscriber, _1);
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);

			Assert::AreEqual(0, fooSubscriber.Data());
			eventTest.Deliver();
			Assert::AreEqual(5, eventTest.Message().foo.Data());
			Assert::AreEqual(5, fooSubscriber.Data());
			eventTest2.Deliver();
			Assert::AreEqual(2, eventTest2.Message().foo.Data());
			Assert::AreEqual(7, fooSubscriber.Data());

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(DeliverMemberFunctionVariant)
		{
			GameTime gameTime;
			Event<EventFooArgs> eventTest{ EventFooArgs(5) };
			Event<EventFooArgs> eventTest2{ EventFooArgs(2) };

			Assert::AreEqual(5, eventTest.Message().foo.Data());
			Assert::AreEqual(2, eventTest2.Message().foo.Data());

			DummyFooSubscriber fooSubscriber;
			fooSubscriber.SubscribeToEventFoo();

			Assert::AreEqual(0, fooSubscriber.Data());
			eventTest.Deliver();
			Assert::AreEqual(5, eventTest.Message().foo.Data());
			Assert::AreEqual(5, fooSubscriber.Data());
			eventTest2.Deliver();
			Assert::AreEqual(2, eventTest2.Message().foo.Data());
			Assert::AreEqual(7, fooSubscriber.Data());

			//prevent "memory leak"
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(DeliverInterrupt) 
		{
			Event<EventFooArgs> eventTest{ EventFooArgs(0) };

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(1, testingInt);

			std::function<void(EventPublisher&)> deliverSubscriberFunction = [&eventTest](EventPublisher& eventPublisher) { eventTest.Deliver(); UNREFERENCED_LOCAL(eventPublisher); };

			Delegate del2 = { deliverSubscriberFunction };
			Event<EventFooArgs>::Subscribe(del2);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(1, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(2, testingInt);

			//prevent memory leaks
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(GetMessage)
		{
			{
				Event<EventFooArgs> eventTest{ EventFooArgs(5) };
				Assert::AreEqual(5, eventTest.Message().foo.Data());
				Event<EventFooArgs> eventTest2{ EventFooArgs() };
				Assert::AreEqual(0, eventTest2.Message().foo.Data());
			}
			{
				const Event<EventFooArgs> eventTest{ EventFooArgs(5) };
				Assert::AreEqual(5, eventTest.Message().foo.Data());
				const Event<EventFooArgs> eventTest2{ EventFooArgs() };
				Assert::AreEqual(0, eventTest2.Message().foo.Data());
			}
		}

		TEST_METHOD(SubscribeUnsubscribeBasic)
		{
			Event<EventFooArgs> eventTest{ EventFooArgs(0) };

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction1 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> dummySubscriberFunction2 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> dummySubscriberFunction3 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction1 };
			Event<EventFooArgs>::Subscribe(del1);

			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(1, testingInt);

			Delegate del2 = { dummySubscriberFunction2 };
			Event<EventFooArgs>::Subscribe(del2);
			Delegate del3 = { dummySubscriberFunction3 };
			Event<EventFooArgs>::Subscribe(del3);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(3, testingInt);

			Event<EventFooArgs>::Unsubscribe(del1);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(2, testingInt);

			Event<EventFooArgs>::Unsubscribe(del2);
			Event<EventFooArgs>::Unsubscribe(del3);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(0, testingInt);

			//prevent memory leaks
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(SubscribeUnsubscribeInterrupt)
		{
			Event<EventFooArgs> eventTest{ EventFooArgs(0) };

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction };
			Event<EventFooArgs>::Subscribe(del1);
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(1, testingInt);

			std::function<void(EventPublisher&)> subSubscriberFunction = [&del1](EventPublisher& eventPublisher) { Event<EventFooArgs>::Subscribe(del1); UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> unsubSubscriberFunction = [&del1](EventPublisher& eventPublisher) { Event<EventFooArgs>::Unsubscribe(del1); UNREFERENCED_LOCAL(eventPublisher); };

			Delegate del2 = { subSubscriberFunction };
			Event<EventFooArgs>::Subscribe(del2);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(1, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(3, testingInt);
			Event<EventFooArgs>::Unsubscribe(del2);

			Delegate del3 = { unsubSubscriberFunction };
			Event<EventFooArgs>::Subscribe(del3);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(3, testingInt);
			Event<EventFooArgs>::Unsubscribe(del3);
			eventTest.Deliver();
			Assert::AreEqual(5, testingInt);

			//prevent memory leaks
			Event<EventFooArgs>::RequestClearSubscribers();
		}

		TEST_METHOD(ClearSubscribersBasic)
		{
			Event<EventFooArgs> eventTest{ EventFooArgs(0) };

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction1 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> dummySubscriberFunction2 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> dummySubscriberFunction3 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction1 };
			Event<EventFooArgs>::Subscribe(del1);

			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(1, testingInt);
			Event<EventFooArgs>::RequestClearSubscribers();


			Delegate del2 = { dummySubscriberFunction2 };
			Event<EventFooArgs>::Subscribe(del2);
			Delegate del3 = { dummySubscriberFunction3 };
			Event<EventFooArgs>::Subscribe(del3);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(2, testingInt);

			Event<EventFooArgs>::RequestClearSubscribers();

			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(0, testingInt);
		}

		TEST_METHOD(ClearSubscribersInterrupt)
		{
			Event<EventFooArgs> eventTest{ EventFooArgs(0) };

			int testingInt = 0;
			std::function<void(EventPublisher&)> dummySubscriberFunction1 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> dummySubscriberFunction2 = [&testingInt](EventPublisher& eventPublisher) { ++testingInt; UNREFERENCED_LOCAL(eventPublisher); };
			std::function<void(EventPublisher&)> clearSubscriberFunction = [&testingInt](EventPublisher& eventPublisher) { Event<EventFooArgs>::RequestClearSubscribers(); UNREFERENCED_LOCAL(eventPublisher); };
			Delegate del1 = { dummySubscriberFunction1 };
			Event<EventFooArgs>::Subscribe(del1);
			Delegate del2 = { dummySubscriberFunction2 };
			Event<EventFooArgs>::Subscribe(del2);

			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(2, testingInt);

			Delegate del3 = { clearSubscriberFunction };
			Event<EventFooArgs>::Subscribe(del3);
			testingInt = 0;
			Assert::AreEqual(0, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(2, testingInt);
			eventTest.Deliver();
			Assert::AreEqual(2, testingInt);

			Event<EventFooArgs>::RequestClearSubscribers();
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}