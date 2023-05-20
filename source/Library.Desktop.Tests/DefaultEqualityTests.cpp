#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DefaultEquality.h"
#include "Foo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace std::string_literals;

namespace  LibraryDesktopTests 
{
	TEST_CLASS(DeafultEqualityTests)
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

			DefaultEquality<Foo> eq;
			
			Assert::IsTrue(eq(a, b));
			Assert::IsTrue(eq(a, a));
			Assert::IsFalse(eq(a, c));
			Assert::IsFalse(eq(b, c));
		}

		TEST_METHOD(CharPointerSpecializations)
		{
			const char* a = "owo";
			const char* b = "uwu";
			char c[4];
			strncpy_s(c, a, strlen(a));

			{
				DefaultEquality<char*> eq;
				Assert::IsTrue(eq(a, c));
				Assert::IsTrue(eq(a, a));
				Assert::IsFalse(eq(a, b));
				Assert::IsFalse(eq(c, b));
			}
			{
				DefaultEquality<const char*> eq;
				Assert::IsTrue(eq(a, c));
				Assert::IsTrue(eq(a, a));
				Assert::IsFalse(eq(a, b));
				Assert::IsFalse(eq(c, b));
			}
			{
				DefaultEquality<char* const> eq;
				Assert::IsTrue(eq(a, c));
				Assert::IsTrue(eq(a, a));
				Assert::IsFalse(eq(a, b));
				Assert::IsFalse(eq(c, b));
			}
			{
				DefaultEquality<const char* const> eq;
				Assert::IsTrue(eq(a, c));
				Assert::IsTrue(eq(a, a));
				Assert::IsFalse(eq(a, b));
				Assert::IsFalse(eq(c, b));
			}
		}


	private:
		inline static _CrtMemState _startMemState;
	};
}