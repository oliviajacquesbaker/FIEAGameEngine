#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DefaultGrowth.h"
#include "Foo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace std::string_literals;

namespace  LibraryDesktopTests
{
	TEST_CLASS(DeafultGrowthTests)
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

		TEST_METHOD(DefaultTemplate)
		{
			const Foo a(10);
			const Foo b(10);
			const Foo c(20);

			DefaultGrowth increment;

			Assert::AreEqual(std::size_t(1), increment(std::size_t(0)));
			Assert::AreEqual(std::size_t(1), increment(std::size_t(1)));
			Assert::AreEqual(std::size_t(1), increment(std::size_t(2)));
			Assert::AreEqual(std::size_t(1), increment(std::size_t(3)));
			Assert::AreEqual(std::size_t(2), increment(std::size_t(4)));
			Assert::AreEqual(std::size_t(5), increment(std::size_t(10)));
			Assert::AreEqual(std::size_t(5), increment(std::size_t(11)));
		}


	private:
		inline static _CrtMemState _startMemState;
	};
}