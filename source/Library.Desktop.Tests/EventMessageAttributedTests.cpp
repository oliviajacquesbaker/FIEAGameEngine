#include "pch.h"
#include "CppUnitTest.h"
#include "EventMessageAttributed.h"
#include "Foo.h"
#include "ToStringSpecializations.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<EventMessageAttributed>(const EventMessageAttributed& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<EventMessageAttributed>(EventMessageAttributed* t) {
		std::string stringResult = t->ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<EventMessageAttributed>(const EventMessageAttributed* t) {
		std::string stringResult = t->ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace  LibraryDesktopTests
{
	TEST_CLASS(EventMessageAttributedTests)
	{
	public:
		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			RegisterType<EventMessageAttributed>();
		}

		TEST_CLASS_CLEANUP(TestClassCleanup)
		{
			TypeManager::Clear();
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
				EventMessageAttributed a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(EventMessageAttributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(EventMessageAttributed::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				EventMessageAttributed* s = rtti->As<EventMessageAttributed>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const EventMessageAttributed a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(EventMessageAttributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(EventMessageAttributed::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const EventMessageAttributed* s = rtti->As<EventMessageAttributed>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(GameObjectRTTIOverrides)
		{
			EventMessageAttributed a("subtypeVal");
			RTTI* rtti = &a;

			Assert::AreEqual("EventMessageAttributed with subtype " + a.Subtype(), rtti->ToString());

			EventMessageAttributed e("subtypeVal");
			RTTI* eRtti = &e;
			Bar d;
			RTTI* dRtti = &d;

			Assert::IsTrue(rtti->Equals(eRtti));
			Assert::IsFalse(rtti->Equals(dRtti));
			Assert::IsFalse(rtti->Equals(nullptr));

			a.Append("key2") = { "A"s, "B"s };
			Assert::AreEqual("EventMessageAttributed with subtype " + a.Subtype(), rtti->ToString());
			Assert::IsFalse(rtti->Equals(eRtti));
		}

		TEST_METHOD(ConstructionAndPopulation)
		{
			EventMessageAttributed eventMessage;
			Assert::IsFalse(eventMessage.IsEmpty());
			Assert::AreEqual(EventMessageAttributed::Signatures().Size() + 1, eventMessage.Size());
			Assert::IsTrue(eventMessage.IsAttribute("this"));
			Assert::IsTrue(eventMessage.IsPrescribedAttribute("subtype"));
			Assert::IsNull(eventMessage.GetParent());
		}

		TEST_METHOD(CopySemantics)
		{
			EventMessageAttributed eventMessage("dummy subtype");
			{
				EventMessageAttributed copy = eventMessage;
				Assert::AreEqual(eventMessage.Size(), copy.Size());
				Assert::AreEqual("dummy subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(eventMessage, copy);
				Assert::AreNotSame(eventMessage, copy);
			}
			{
				EventMessageAttributed copy("something else");
				copy = eventMessage;
				Assert::AreEqual(eventMessage.Size(), copy.Size());
				Assert::AreEqual("dummy subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(eventMessage, copy);
				Assert::AreNotSame(eventMessage, copy);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			EventMessageAttributed eventMessage("dummy subtype");
			{
				EventMessageAttributed movingCopy = eventMessage;
				EventMessageAttributed copy = std::move(movingCopy);
				Assert::AreEqual(eventMessage.Size(), copy.Size());
				Assert::AreEqual("dummy subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(eventMessage, copy);
				Assert::AreNotSame(eventMessage, copy);
			}
			{
				EventMessageAttributed movingCopy = eventMessage;
				EventMessageAttributed copy("something else");
				copy = std::move(movingCopy);
				Assert::AreEqual(eventMessage.Size(), copy.Size());
				Assert::AreEqual("dummy subtype"s, copy.At("subtype").GetAsString());
				Assert::AreEqual(eventMessage, copy);
				Assert::AreNotSame(eventMessage, copy);
			}
		}

		TEST_METHOD(Clone)
		{
			EventMessageAttributed eventMessage("dummy subtype");
			auto clone = eventMessage.Clone();

			EventMessageAttributed* clonedEventMessage = clone->As<EventMessageAttributed>();
			Assert::IsNotNull(clonedEventMessage);
			Assert::AreNotSame(eventMessage, *clonedEventMessage);
			Assert::AreEqual(eventMessage, *clonedEventMessage);

			eventMessage["subtype"].Set("new subtype"s);
			Assert::AreEqual("new subtype"s, eventMessage.Subtype());
			Assert::AreNotEqual(eventMessage, *clonedEventMessage);

			auto clone2 = eventMessage.Clone();
			EventMessageAttributed* clonedEventMessage2 = clone2->As<EventMessageAttributed>();
			Assert::IsNotNull(clonedEventMessage2);
			Assert::AreNotSame(eventMessage, *clonedEventMessage2);
			Assert::AreEqual(eventMessage, *clonedEventMessage2);
			Assert::AreEqual("new subtype"s, clonedEventMessage2->Subtype());

			//prevent memory leaks
			delete clone;
			delete clone2;
		}

		TEST_METHOD(GetSetSubtype)
		{
			{
				EventMessageAttributed actionDummy;
				Assert::AreEqual(""s, actionDummy.Subtype());
				actionDummy.SetSubtype("subtype test"s);
				Assert::AreEqual("subtype test"s, actionDummy.Subtype());
			}
			{
				const EventMessageAttributed actionDummy("test"s);
				Assert::AreEqual("test"s, actionDummy.Subtype());
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}