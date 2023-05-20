/// <summary>
/// Unit tests exercising the functionality of DefaultHash and its specializations
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "DefaultHash.h"
#include "Foo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace std::string_literals;

namespace LibraryDesktopTests
{
	TEST_CLASS(DeafultHashTests)
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

		TEST_METHOD(IntegerHash)
		{
			const int a = 10;
			const int b = -20;
			const int c = 10;
			{
				DefaultHash<int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<const int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<short int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<const short int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<long int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<const long int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<long long int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<const long long int> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
		}

		TEST_METHOD(DoubleHash)
		{
			const double a = 10.5;
			const double b = -20.5;
			const double c = 10.5;
			const double d = 10.1;
			{
				DefaultHash<double> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreNotEqual(hashFunc(d), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
			{
				DefaultHash<const double> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreNotEqual(hashFunc(d), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
				Assert::IsTrue(hashFunc(b) >= 0);
			}
		}

		TEST_METHOD(StringHash)
		{
			const string a = "Hello"s;
			const string b = "Goodbye"s;
			const string c(a);
			{
				DefaultHash<string> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
			}	
			{
				DefaultHash<const string> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
			}
		}

		TEST_METHOD(WideStringHash)
		{
			const wstring a = L"Hello"s;
			const wstring b = L"Goodbye"s;
			const wstring c(a);
			{
				DefaultHash<wstring> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
			}
			{
				DefaultHash<const wstring> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
			}
		}

		TEST_METHOD(CStringHash)
		{
			const char* a = "Hello";
			const char* b = "Goodbye";
			char c[6];
			strcpy_s(c, a);
			{
				DefaultHash<char*> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
			}
			{
				DefaultHash<const char*> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
			}
			{
				DefaultHash<char* const> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
			}
			{
				DefaultHash<const char* const> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}