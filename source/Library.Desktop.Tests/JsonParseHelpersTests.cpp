#include "pch.h"
#include "CppUnitTest.h"
#include "JsonParseCoordinator.h"
#include "JsonTestParseHelper.h"
#include "JsonIntegerParseHelper.h"
#include "ToStringSpecializations.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace FIEAGameEngine;
using namespace UnitTests;
using namespace std;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<JsonTestParseHelper>(JsonTestParseHelper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonTestParseHelper>(const JsonTestParseHelper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonIntegerParseHelper>(JsonIntegerParseHelper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonIntegerParseHelper>(const JsonIntegerParseHelper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonTestParseHelper::Wrapper>(JsonTestParseHelper::Wrapper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonTestParseHelper::Wrapper>(const JsonTestParseHelper::Wrapper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonIntegerParseHelper::Wrapper>(JsonIntegerParseHelper::Wrapper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonIntegerParseHelper::Wrapper>(const JsonIntegerParseHelper::Wrapper* t) {
		RETURN_WIDE_STRING(t);
	}
}

namespace LibraryDesktopTests
{
	TEST_CLASS(JsonParseHelperTests)
	{
	public:
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

		TEST_METHOD(IntWrapperRTTITests)
		{
			{
				shared_ptr<JsonIntegerParseHelper::Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
				RTTI* rtti = wrapper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonIntegerParseHelper::Wrapper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonIntegerParseHelper::Wrapper::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				JsonIntegerParseHelper::Wrapper* p = rtti->As<JsonIntegerParseHelper::Wrapper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(wrapper.get(), p);

				Assert::AreEqual(string("Data:"), rtti->ToString());
				wrapper->data.PushBack(1);
				wrapper->data.PushBack(2);
				Assert::AreEqual(string("Data: 1 2"), rtti->ToString());

				JsonIntegerParseHelper::Wrapper P_wrapper;
				P_wrapper.data.PushBack(1);
				P_wrapper.data.PushBack(2);
				RTTI* pRtti = &P_wrapper;
				Assert::IsTrue(rtti->Equals(pRtti));

				RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
			{
				shared_ptr<const JsonIntegerParseHelper::Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
				const RTTI* rtti = wrapper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonIntegerParseHelper::Wrapper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonIntegerParseHelper::Wrapper::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const JsonIntegerParseHelper::Wrapper* p = rtti->As<JsonIntegerParseHelper::Wrapper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(wrapper.get(), p);

				Assert::AreEqual(string("Data:"), rtti->ToString());

				JsonIntegerParseHelper::Wrapper P_wrapper;
				const RTTI* pRtti = &P_wrapper;
				Assert::IsTrue(rtti->Equals(pRtti));

				const RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
		}

		TEST_METHOD(IntHelperRTTITests)
		{
			{
				shared_ptr<JsonIntegerParseHelper> helper = make_shared<JsonIntegerParseHelper>();
				RTTI* rtti = helper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonIntegerParseHelper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonIntegerParseHelper::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				JsonIntegerParseHelper* p = rtti->As<JsonIntegerParseHelper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(helper.get(), p);

				Assert::AreEqual(string("Integer Parse Helper. Is currently parsing: 0"), rtti->ToString());

				JsonIntegerParseHelper P_helper(*helper);
				RTTI* pRtti = &P_helper;
				Assert::IsTrue(rtti->Equals(pRtti));

				RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
			{
				shared_ptr<const JsonIntegerParseHelper> helper = make_shared<JsonIntegerParseHelper>();
				const RTTI* rtti = helper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonIntegerParseHelper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonIntegerParseHelper::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const JsonIntegerParseHelper* p = rtti->As<JsonIntegerParseHelper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(helper.get(), p);

				Assert::AreEqual(string("Integer Parse Helper. Is currently parsing: 0"), rtti->ToString());

				const JsonIntegerParseHelper P_helper(*helper);
				const RTTI* pRtti = &P_helper;
				Assert::IsTrue(rtti->Equals(pRtti));

				const RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
		}

		TEST_METHOD(IntWrapperCreate)
		{
			JsonIntegerParseHelper::Wrapper intWrapper;
			intWrapper.data.PushBack(5);
			Assert::AreEqual(size_t(1), intWrapper.data.Size());

			shared_ptr<Wrapper> wrapper = intWrapper.Create();
			Assert::AreEqual(intWrapper.TypeIdInstance(), wrapper->TypeIdInstance());

			JsonIntegerParseHelper::Wrapper* intTest = static_cast<JsonIntegerParseHelper::Wrapper*>(wrapper.get());
			Assert::AreEqual(size_t(1), intWrapper.data.Size());
			Assert::AreEqual(size_t(0), intTest->data.Size());
		}

		TEST_METHOD(IntHelperCreate)
		{
			JsonIntegerParseHelper intHelper;

			shared_ptr<IJsonParseHelper> wrapper = intHelper.Create();
			Assert::AreEqual(intHelper.TypeIdInstance(), wrapper->TypeIdInstance());

			JsonIntegerParseHelper* intTest = static_cast<JsonIntegerParseHelper*>(wrapper.get());
			Assert::IsNotNull(intTest);
		}

		TEST_METHOD(IntHelperDeserialization)
		{
			string inputString = R"({ "Integer": 100})";

			shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
			JsonParseCoordinator parseCoordinator(wrapper);
			auto helper = make_shared<JsonIntegerParseHelper>();
			parseCoordinator.AddHelper(helper);

			parseCoordinator.DeserializeObject(inputString);
			JsonIntegerParseHelper::Wrapper* rawWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(wrapper.get());
			Assert::AreEqual(size_t(1), rawWrapper->data.Size());
			Assert::AreEqual(100, rawWrapper->data.Front());

			//cover case unable to be reached by normal means
			auto badWrapper = JsonTestParseHelper::Wrapper();
			bool handled = helper->EndHandler(badWrapper, "nonsense");
			Assert::IsFalse(handled);
		}

		TEST_METHOD(TestWrapperRTTITests)
		{
			{
				shared_ptr<JsonTestParseHelper::Wrapper> wrapper = make_shared<JsonTestParseHelper::Wrapper>();
				RTTI* rtti = wrapper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTestParseHelper::Wrapper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTestParseHelper::Wrapper::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				JsonTestParseHelper::Wrapper* p = rtti->As<JsonTestParseHelper::Wrapper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(wrapper.get(), p);

				Assert::AreEqual(string("Test Parse Wrapper, with its max depth at 0"), rtti->ToString());
				wrapper->maxDepth = 5;
				Assert::AreEqual(string("Test Parse Wrapper, with its max depth at 5"), rtti->ToString());

				JsonTestParseHelper::Wrapper P_wrapper;
				P_wrapper.maxDepth = 5;
				RTTI* pRtti = &P_wrapper;
				Assert::IsTrue(rtti->Equals(pRtti));
				RTTI* bad = nullptr;
				Assert::IsFalse(rtti->Equals(bad));
			}
			{
				shared_ptr<const JsonTestParseHelper::Wrapper> wrapper = make_shared<JsonTestParseHelper::Wrapper>();
				const RTTI* rtti = wrapper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTestParseHelper::Wrapper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTestParseHelper::Wrapper::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const JsonTestParseHelper::Wrapper* p = rtti->As<JsonTestParseHelper::Wrapper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(wrapper.get(), p);

				Assert::AreEqual(string("Test Parse Wrapper, with its max depth at 0"), rtti->ToString());

				JsonTestParseHelper::Wrapper P_wrapper;
				const RTTI* pRtti = &P_wrapper;
				Assert::IsTrue(rtti->Equals(pRtti));
				const RTTI* bad = nullptr;
				Assert::IsFalse(rtti->Equals(bad));
			}
		}

		TEST_METHOD(TestHelperRTTITests)
		{
			{
				shared_ptr<JsonTestParseHelper> helper = make_shared<JsonTestParseHelper>();
				RTTI* rtti = helper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTestParseHelper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTestParseHelper::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				JsonTestParseHelper* p = rtti->As<JsonTestParseHelper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(helper.get(), p);

				Assert::AreEqual(string("Test Parse Helper, with its start handler called 0 times, end handler called 0 times."), rtti->ToString());
				helper->startHandlerCount++;
				Assert::AreEqual(string("Test Parse Helper, with its start handler called 1 times, end handler called 0 times."), rtti->ToString());

				JsonTestParseHelper P_helper(*helper);
				RTTI* pRtti = &P_helper;
				Assert::IsTrue(rtti->Equals(pRtti));
				RTTI* bad = nullptr;
				Assert::IsFalse(rtti->Equals(bad));
			}
			{
				shared_ptr<const JsonTestParseHelper> helper = make_shared<JsonTestParseHelper>();
				const RTTI* rtti = helper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTestParseHelper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTestParseHelper::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const JsonTestParseHelper* p = rtti->As<JsonTestParseHelper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(helper.get(), p);

				Assert::AreEqual(string("Test Parse Helper, with its start handler called 0 times, end handler called 0 times."), rtti->ToString());

				const JsonTestParseHelper P_helper(*helper);
				const RTTI* pRtti = &P_helper;
				Assert::IsTrue(rtti->Equals(pRtti));
				const RTTI* bad = nullptr;
				Assert::IsFalse(rtti->Equals(bad));
			}
		}

		TEST_METHOD(TestWrapperCreate)
		{
			JsonTestParseHelper::Wrapper testWrapper;
			testWrapper.maxDepth = 1;
			testWrapper.Initialize();
			testWrapper.Cleanup();
			Assert::AreEqual(size_t(1), testWrapper.maxDepth);
			Assert::IsTrue(testWrapper.initializeCalled);
			Assert::IsTrue(testWrapper.cleanupCalled);

			shared_ptr<Wrapper> wrapper = testWrapper.Create();
			Assert::AreEqual(testWrapper.TypeIdInstance(), wrapper->TypeIdInstance());

			JsonTestParseHelper::Wrapper* testRaw = static_cast<JsonTestParseHelper::Wrapper*>(wrapper.get());
			Assert::AreEqual(size_t(1), testWrapper.maxDepth);
			Assert::IsTrue(testWrapper.initializeCalled);
			Assert::IsTrue(testWrapper.cleanupCalled);
			Assert::AreEqual(size_t(0), testRaw->maxDepth);
			Assert::IsFalse(testRaw->initializeCalled);
			Assert::IsFalse(testRaw->cleanupCalled);
		}

		TEST_METHOD(TestHelperCreate)
		{
			JsonTestParseHelper testHelper;
			testHelper.startHandlerCount = 5;
			testHelper.endHandlerCount = 5;
			testHelper.Initialize();
			testHelper.Cleanup();
			Assert::AreEqual(size_t(5), testHelper.startHandlerCount);
			Assert::AreEqual(size_t(5), testHelper.startHandlerCount);
			Assert::IsTrue(testHelper.initializeCalled);
			Assert::IsTrue(testHelper.cleanupCalled);

			shared_ptr<IJsonParseHelper> wrapper = testHelper.Create();
			Assert::AreEqual(testHelper.TypeIdInstance(), wrapper->TypeIdInstance());

			JsonTestParseHelper* test = static_cast<JsonTestParseHelper*>(wrapper.get());
			Assert::IsNotNull(test);

			Assert::AreEqual(size_t(5), testHelper.startHandlerCount);
			Assert::AreEqual(size_t(5), testHelper.startHandlerCount);
			Assert::IsTrue(testHelper.initializeCalled);
			Assert::IsTrue(testHelper.cleanupCalled);

			Assert::AreEqual(size_t(0), test->startHandlerCount);
			Assert::AreEqual(size_t(0), test->startHandlerCount);
			Assert::IsFalse(test->initializeCalled);
			Assert::IsFalse(test->cleanupCalled);
		}

		TEST_METHOD(TestHelperDeserialization)
		{
			const string inputString = R"(
			{
				"Address":
				{
					"type": "table",
					"value": 
					{
						"Street":
						{
							"type": "string",
							"value": "123 Sesame St."
						}
					}
				}
			})"s;

			shared_ptr<Wrapper> wrapper = make_shared<JsonTestParseHelper::Wrapper>();
			JsonTestParseHelper::Wrapper* rawWrapper = static_cast<JsonTestParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTestParseHelper> testHelper = make_shared<JsonTestParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::IsFalse(testHelper->initializeCalled);
			Assert::IsFalse(testHelper->cleanupCalled);
			Assert::IsFalse(rawWrapper->initializeCalled);
			Assert::IsFalse(rawWrapper->cleanupCalled);
			Assert::AreEqual(size_t(0), testHelper->startHandlerCount);
			Assert::AreEqual(size_t(0), testHelper->endHandlerCount);
			Assert::AreEqual(size_t(0), wrapper->Depth());
			Assert::AreEqual(size_t(0), rawWrapper->maxDepth);

			parseCoordinator.DeserializeObject(inputString);
			Assert::IsTrue(testHelper->initializeCalled);
			Assert::IsTrue(testHelper->cleanupCalled);
			Assert::IsTrue(rawWrapper->initializeCalled);
			Assert::IsTrue(rawWrapper->cleanupCalled);
			Assert::AreEqual(size_t(6), testHelper->startHandlerCount);
			Assert::AreEqual(size_t(6), testHelper->endHandlerCount);
			Assert::AreEqual(size_t(0), wrapper->Depth());
			Assert::AreEqual(size_t(4), rawWrapper->maxDepth);

			//cover case unable to be reached by normal means
			auto badWrapper = JsonIntegerParseHelper::Wrapper();
			bool handled = testHelper->EndHandler(badWrapper, "nonsense");
			Assert::IsFalse(handled);
		}

		TEST_METHOD(TestWrapperInitializeandCleanup)
		{
			JsonTestParseHelper::Wrapper testWrapper;
			Assert::IsFalse(testWrapper.initializeCalled);
			Assert::IsFalse(testWrapper.cleanupCalled);

			testWrapper.Initialize();
			testWrapper.Cleanup();
			Assert::IsTrue(testWrapper.initializeCalled);
			Assert::IsTrue(testWrapper.cleanupCalled);

			testWrapper.Initialize();
			testWrapper.Cleanup();
			Assert::IsTrue(testWrapper.initializeCalled);
			Assert::IsTrue(testWrapper.cleanupCalled);
		}

		TEST_METHOD(TestHelperInitializeandCleanup)
		{
			JsonTestParseHelper testHelper;
			Assert::IsFalse(testHelper.initializeCalled);
			Assert::IsFalse(testHelper.cleanupCalled);

			testHelper.Initialize();
			testHelper.Cleanup();
			Assert::IsTrue(testHelper.initializeCalled);
			Assert::IsTrue(testHelper.cleanupCalled);

			testHelper.Initialize();
			testHelper.Cleanup();
			Assert::IsTrue(testHelper.initializeCalled);
			Assert::IsTrue(testHelper.cleanupCalled);
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}