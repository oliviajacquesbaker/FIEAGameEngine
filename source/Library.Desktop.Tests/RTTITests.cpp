#include "pch.h"
#include "CppUnitTest.h"
#include "EmptyRTTI.h"
#include "ToStringSpecializations.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace FIEAGameEngine;
using namespace UnitTests;
using namespace std;
using namespace std::string_literals;

namespace UnitTestLibraryDesktop
{

	TEST_CLASS(RTTITests)
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

		TEST_METHOD(DefaultToString)
		{
			EmptyRTTI test;
			Assert::AreEqual("RTTI"s, test.ToString());
		}

		TEST_METHOD(DefaultEquals)
		{
			EmptyRTTI test;
			EmptyRTTI test2;
			Assert::IsFalse(test.Equals(&test2));
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}