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
	std::wstring ToString<JsonParseCoordinator>(JsonParseCoordinator* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonParseCoordinator>(const JsonParseCoordinator* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<shared_ptr<Wrapper>>(const shared_ptr<Wrapper>& t) {
		RETURN_WIDE_STRING(t.get());
	}
}

namespace LibraryDesktopTests
{
	TEST_CLASS(JsonParseCoordinatorTests)
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

		TEST_METHOD(ParserRTTITests)
		{
			{
				shared_ptr<Wrapper> wrapper = make_shared<JsonTestParseHelper::Wrapper>();
				JsonParseCoordinator parseCoordinator(wrapper);
				RTTI* rtti = &parseCoordinator;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonParseCoordinator::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonParseCoordinator::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				JsonParseCoordinator* p = rtti->As<JsonParseCoordinator>();
				Assert::IsNotNull(p);
				Assert::AreEqual(&parseCoordinator, p);

				Assert::AreEqual(string("Parse Coordinator with wrapper: Test Parse Wrapper, with its max depth at 0"), rtti->ToString());

				JsonParseCoordinator P_parseCoordinator(wrapper);
				RTTI* pRtti = &P_parseCoordinator;
				Assert::IsTrue(rtti->Equals(pRtti));
				P_parseCoordinator.AddHelper(make_shared<JsonTestParseHelper>());
				Assert::IsFalse(rtti->Equals(pRtti));
				parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());
				Assert::IsFalse(rtti->Equals(pRtti));
				parseCoordinator.RemoveHelper(JsonIntegerParseHelper::TypeIdClass());
				parseCoordinator.AddHelper(make_shared<JsonTestParseHelper>());
				Assert::IsTrue(rtti->Equals(pRtti));

				RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
				Foo f;
				empty = &f;
				Assert::IsFalse(rtti->Equals(empty));
			}
			{
				shared_ptr<Wrapper> wrapper = make_shared<JsonTestParseHelper::Wrapper>();
				const JsonParseCoordinator parseCoordinator(wrapper);
				const RTTI* rtti = &parseCoordinator;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonParseCoordinator::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonParseCoordinator::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const JsonParseCoordinator* p = rtti->As<JsonParseCoordinator>();
				Assert::IsNotNull(p);
				Assert::AreEqual(&parseCoordinator, p);

				Assert::AreEqual(string("Parse Coordinator with wrapper: Test Parse Wrapper, with its max depth at 0"), rtti->ToString());

				const JsonParseCoordinator P_parseCoordinator(wrapper);
				const RTTI* pRtti = &P_parseCoordinator;
				Assert::IsTrue(rtti->Equals(pRtti));
			}
		}

		TEST_METHOD(ParserConstruction)
		{
			shared_ptr<Wrapper> wrapper = make_shared<JsonTestParseHelper::Wrapper>();
			JsonParseCoordinator parseCoordinator(wrapper);
			Assert::AreEqual(size_t(0), parseCoordinator.GetHelpers().Size());
			Assert::AreEqual(wrapper, parseCoordinator.GetWrapper());

			Assert::ExpectException<runtime_error>([] {JsonParseCoordinator parser(nullptr); UNREFERENCED_LOCAL(parser); });
		}

		TEST_METHOD(ParserMoveSemantics)
		{
			shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
			JsonIntegerParseHelper::Wrapper* rawWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());
			shared_ptr<JsonTestParseHelper> testHelper = make_shared<JsonTestParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::IsFalse(testHelper->initializeCalled);
			parseCoordinator.DeserializeObject(R"({ "Integer": 100})");
			Assert::IsTrue(testHelper->initializeCalled);
			Assert::AreEqual(size_t(1), rawWrapper->data.Size());
			Assert::AreEqual(100, rawWrapper->data.Front());

			shared_ptr<JsonParseCoordinator> movedParser = make_shared<JsonParseCoordinator>(std::move(parseCoordinator));
			JsonIntegerParseHelper::Wrapper* movedWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(movedParser->GetWrapper().get());
			Assert::AreEqual(size_t(2), movedParser->GetHelpers().Size());
			Assert::AreEqual(size_t(1), movedWrapper->data.Size());
			Assert::AreEqual(100, movedWrapper->data.Front());

			JsonTestParseHelper* movedHelper = static_cast<JsonTestParseHelper*>(movedParser->GetHelpers()[1].get());
			Assert::IsNotNull(movedHelper);
			Assert::IsTrue(movedHelper->initializeCalled);
		}

		TEST_METHOD(ParserCreate)
		{
			shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
			JsonIntegerParseHelper::Wrapper* rawWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());
			shared_ptr<JsonTestParseHelper> testHelper = make_shared<JsonTestParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::IsFalse(testHelper->initializeCalled);
			parseCoordinator.DeserializeObject(R"({ "Integer": 100})");
			Assert::IsTrue(testHelper->initializeCalled);
			Assert::AreEqual(size_t(1), rawWrapper->data.Size());
			Assert::AreEqual(100, rawWrapper->data.Front());

			shared_ptr<JsonParseCoordinator> clone = parseCoordinator.Create();
			JsonIntegerParseHelper::Wrapper* cloneWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(clone->GetWrapper().get());
			Assert::AreEqual(size_t(0),cloneWrapper->data.Size());
			Assert::AreEqual(size_t(2), clone->GetHelpers().Size());

			JsonTestParseHelper* helper = static_cast<JsonTestParseHelper*>(clone->GetHelpers()[1].get());
			Assert::IsNotNull(helper);
			Assert::IsFalse(helper->initializeCalled);
			clone->DeserializeObject(R"({ "Integer": 100})");
			Assert::IsTrue(helper->initializeCalled);
			Assert::AreEqual(size_t(1), cloneWrapper->data.Size());
			Assert::AreEqual(100, cloneWrapper->data.Front());

			Assert::AreEqual(cloneWrapper->Depth(), wrapper->Depth());
			cloneWrapper->IncrementDepth();
			Assert::AreNotEqual(cloneWrapper->Depth(), wrapper->Depth());
		}

		TEST_METHOD(ParserGetHelpers)
		{
			{
				shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
				JsonParseCoordinator parseCoordinator(wrapper);
				parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());
				shared_ptr<JsonTestParseHelper> testHelper = make_shared<JsonTestParseHelper>();
				parseCoordinator.AddHelper(testHelper);
				testHelper->Initialize();

				auto& helpers = parseCoordinator.GetHelpers();
				Assert::AreEqual(size_t(2), helpers.Size());

				Assert::IsTrue(helpers[0].get()->Is(JsonIntegerParseHelper::TypeIdClass()));
				Assert::IsTrue(helpers[1].get()->Is(JsonTestParseHelper::TypeIdClass()));
				JsonTestParseHelper* helper = static_cast<JsonTestParseHelper*>(helpers[1].get());
				Assert::IsTrue(helper->initializeCalled);
			}
			{
				shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
				JsonParseCoordinator parseCoordinator(wrapper);
				parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());
				shared_ptr<JsonTestParseHelper> testHelper = make_shared<JsonTestParseHelper>();
				parseCoordinator.AddHelper(testHelper);
				testHelper->Initialize();

				const JsonParseCoordinator& constRef = parseCoordinator;
				auto& helpers = constRef.GetHelpers();
				Assert::AreEqual(size_t(2), helpers.Size());

				Assert::IsTrue(helpers[0].get()->Is(JsonIntegerParseHelper::TypeIdClass()));
				Assert::IsTrue(helpers[1].get()->Is(JsonTestParseHelper::TypeIdClass()));
				JsonTestParseHelper* helper = static_cast<JsonTestParseHelper*>(helpers[1].get());
				Assert::IsTrue(helper->initializeCalled);
			}
		}

		TEST_METHOD(ParserAddRemoveHelpers)
		{
			shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
			JsonParseCoordinator parseCoordinator(wrapper);
			Assert::AreEqual(size_t(0), parseCoordinator.GetHelpers().Size());

			auto intHelper = make_shared<JsonIntegerParseHelper>();
			bool added = parseCoordinator.AddHelper(intHelper);
			Assert::IsTrue(added);
			Assert::AreEqual(size_t(1), parseCoordinator.GetHelpers().Size());
			Assert::IsTrue(parseCoordinator.GetHelpers()[0].get()->Is(JsonIntegerParseHelper::TypeIdClass()));

			auto helper = make_shared<JsonTestParseHelper>();
			added = parseCoordinator.AddHelper(helper);
			Assert::IsTrue(added);
			Assert::AreEqual(size_t(2), parseCoordinator.GetHelpers().Size());
			Assert::IsTrue(parseCoordinator.GetHelpers()[1].get()->Is(JsonTestParseHelper::TypeIdClass()));

			added = parseCoordinator.AddHelper(helper);
			Assert::IsFalse(added);
			Assert::AreEqual(size_t(2), parseCoordinator.GetHelpers().Size());
			Assert::IsTrue(parseCoordinator.GetHelpers()[1].get()->Is(JsonTestParseHelper::TypeIdClass()));

			auto helperDup = make_shared<JsonTestParseHelper>();
			added = parseCoordinator.AddHelper(helperDup);
			Assert::IsFalse(added);
			Assert::AreEqual(size_t(2), parseCoordinator.GetHelpers().Size());
			Assert::IsTrue(parseCoordinator.GetHelpers()[1].get()->Is(JsonTestParseHelper::TypeIdClass()));

			bool removed = parseCoordinator.RemoveHelper(helper);
			Assert::IsTrue(removed);
			Assert::AreEqual(size_t(1), parseCoordinator.GetHelpers().Size());
			Assert::IsTrue(parseCoordinator.GetHelpers().end() == parseCoordinator.GetHelpers().Find(helper));

			parseCoordinator.AddHelper(helper);
			Assert::AreEqual(size_t(2), parseCoordinator.GetHelpers().Size());
			Assert::IsFalse(parseCoordinator.GetHelpers().end() == parseCoordinator.GetHelpers().Find(helper));
			Assert::IsTrue(parseCoordinator.GetHelpers()[1].get()->Is(JsonTestParseHelper::TypeIdClass()));

			removed = parseCoordinator.RemoveHelper(JsonIntegerParseHelper::TypeIdClass());
			Assert::IsTrue(removed);
			Assert::AreEqual(size_t(1), parseCoordinator.GetHelpers().Size());
			Assert::IsTrue(parseCoordinator.GetHelpers().end() == parseCoordinator.GetHelpers().Find(intHelper));

			removed = parseCoordinator.RemoveHelper(intHelper);
			Assert::IsFalse(removed);
			Assert::AreEqual(size_t(1), parseCoordinator.GetHelpers().Size());
			Assert::IsTrue(parseCoordinator.GetHelpers().end() == parseCoordinator.GetHelpers().Find(intHelper));

			removed = parseCoordinator.RemoveHelper(JsonIntegerParseHelper::TypeIdClass());
			Assert::IsFalse(removed);

			parseCoordinator.RemoveHelper(helper);
			Assert::AreEqual(size_t(0), parseCoordinator.GetHelpers().Size());
			removed = parseCoordinator.RemoveHelper(nullptr);
			Assert::IsFalse(removed);
			Assert::AreEqual(size_t(0), parseCoordinator.GetHelpers().Size());
		}

		TEST_METHOD(ParserGetSetWrapper)
		{
			shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
			JsonParseCoordinator parseCoordinator(wrapper);
			const JsonParseCoordinator& constRef = parseCoordinator;
			Assert::AreEqual(wrapper, parseCoordinator.GetWrapper());
			Assert::AreEqual(wrapper, constRef.GetWrapper());

			shared_ptr<Wrapper> testWrapper = make_shared<JsonTestParseHelper::Wrapper>();
			parseCoordinator.SetWrapper(testWrapper);
			Assert::AreEqual(testWrapper, parseCoordinator.GetWrapper());
			Assert::AreEqual(testWrapper, constRef.GetWrapper());

			Assert::ExpectException<runtime_error>([&parseCoordinator] {parseCoordinator.SetWrapper(shared_ptr<Wrapper>(nullptr)); });
		}

		TEST_METHOD(ParseFromString)
		{
			string inputString = R"(
			{
				"Health":
				{
					"type": "integer",
					"value": 100
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
			Assert::AreEqual(size_t(3), testHelper->startHandlerCount);
			Assert::AreEqual(size_t(3), testHelper->endHandlerCount);
			Assert::AreEqual(size_t(0), wrapper->Depth());
			Assert::AreEqual(size_t(2), rawWrapper->maxDepth);
		}

		TEST_METHOD(ParseFromStream)
		{
			stringstream inputStream;
			inputStream << R"(
			{
				"Health":
				{
					"type": "integer",
					"value": 100
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

			parseCoordinator.DeserializeObject(inputStream);
			Assert::IsTrue(testHelper->initializeCalled);
			Assert::IsTrue(testHelper->cleanupCalled);
			Assert::IsTrue(rawWrapper->initializeCalled);
			Assert::IsTrue(rawWrapper->cleanupCalled);
			Assert::AreEqual(size_t(3), testHelper->startHandlerCount);
			Assert::AreEqual(size_t(3), testHelper->endHandlerCount);
			Assert::AreEqual(size_t(0), wrapper->Depth());
			Assert::AreEqual(size_t(2), rawWrapper->maxDepth);

			stringstream badStream;
			badStream.setstate(1);
			Assert::ExpectException<runtime_error>([&parseCoordinator, &badStream] {parseCoordinator.DeserializeObject(badStream); }); 
		}

		TEST_METHOD(ParseFromFile)
		{
			const string fileName = "TestFile.json";
			string inputString = R"(
			{
				"Health":
				{
					"type": "integer",
					"value": 100
				}
			})"s;

			ofstream outFile(fileName);
			Assert::IsTrue(outFile.good());
			outFile << inputString;
			outFile.close();

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

			parseCoordinator.DeserializeObjectFromFile(fileName);

			Assert::IsTrue(testHelper->initializeCalled);
			Assert::IsTrue(testHelper->cleanupCalled);
			Assert::IsTrue(rawWrapper->initializeCalled);
			Assert::IsTrue(rawWrapper->cleanupCalled);
			Assert::AreEqual(size_t(3), testHelper->startHandlerCount);
			Assert::AreEqual(size_t(3), testHelper->endHandlerCount);
			Assert::AreEqual(size_t(0), wrapper->Depth());
			Assert::AreEqual(size_t(2), rawWrapper->maxDepth);
		}

		TEST_METHOD(AllHelpersFail)
		{
			const string inputString = R"(
			{
				"Test": 100
			})"s;

			{
				shared_ptr<Wrapper> wrapper = make_shared<JsonTestParseHelper::Wrapper>();
				JsonTestParseHelper::Wrapper* rawWrapper = static_cast<JsonTestParseHelper::Wrapper*>(wrapper.get());
				JsonParseCoordinator parseCoordinator(wrapper);

				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->maxDepth);
				parseCoordinator.DeserializeObject(inputString);
				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->maxDepth);

				parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());

				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->maxDepth);
				parseCoordinator.DeserializeObject(inputString);
				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->maxDepth);
			}
			{
				shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
				JsonIntegerParseHelper::Wrapper* rawWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(wrapper.get());
				JsonParseCoordinator parseCoordinator(wrapper);

				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->data.Size());
				parseCoordinator.DeserializeObject(inputString);
				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->data.Size());

				parseCoordinator.AddHelper(make_shared<JsonTestParseHelper>());

				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->data.Size());
				parseCoordinator.DeserializeObject(inputString);
				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(0), rawWrapper->data.Size());
			}
			
		}

		TEST_METHOD(IntegerParsing)
		{
			string inputString = R"({ "Integer": 100})";
			{
				//value testing
				shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
				JsonParseCoordinator parseCoordinator(wrapper);
				parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());

				parseCoordinator.DeserializeObject(inputString);
				JsonIntegerParseHelper::Wrapper* rawWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(wrapper.get());
				Assert::AreEqual(size_t(1), rawWrapper->data.Size());
				Assert::AreEqual(100, rawWrapper->data.Front());
			}
			{
				//operational testing
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
				Assert::AreEqual(size_t(1), testHelper->startHandlerCount);
				Assert::AreEqual(size_t(1), testHelper->endHandlerCount);
				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(1), rawWrapper->maxDepth);
			}
		}

		TEST_METHOD(IntegerArrayParsing)
		{
			string inputString = R"({ "Numbers" : [ 10, 20, 30 ] })";
			{	
				//value testing
				shared_ptr<Wrapper> wrapper = make_shared<JsonIntegerParseHelper::Wrapper>();
				JsonParseCoordinator parseCoordinator(wrapper);
				parseCoordinator.AddHelper(make_shared<JsonIntegerParseHelper>());

				parseCoordinator.DeserializeObject(inputString);
				JsonIntegerParseHelper::Wrapper* rawWrapper = static_cast<JsonIntegerParseHelper::Wrapper*>(wrapper.get());
				Assert::AreEqual(size_t(3), rawWrapper->data.Size());
				Assert::AreEqual(10, rawWrapper->data[0]);
				Assert::AreEqual(20, rawWrapper->data[1]);
				Assert::AreEqual(30, rawWrapper->data[2]);
			}
			{
				//operational testing
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
				Assert::AreEqual(size_t(3), testHelper->startHandlerCount);
				Assert::AreEqual(size_t(3), testHelper->endHandlerCount);
				Assert::AreEqual(size_t(0), wrapper->Depth());
				Assert::AreEqual(size_t(1), rawWrapper->maxDepth);
			}
		}

		TEST_METHOD(GeneralArrayParsing)
		{
			string inputString = R"(
			{
				"Objects":
				[
					{
						"1": 1,
						"2": 2,
						"3": 3,
						"4": 4
					},
					{ "5": 5 },
					{ "9": 9 }
				]
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
			Assert::AreEqual(size_t(2), rawWrapper->maxDepth);
		}

		TEST_METHOD(NestedObjects)
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
		}


	private:
		inline static _CrtMemState _startMemState;
	};
}