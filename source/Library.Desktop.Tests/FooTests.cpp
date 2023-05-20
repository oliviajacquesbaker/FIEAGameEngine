#include "pch.h"
#include "CppUnitTest.h"
#include "Foo.h"
#include "ToStringSpecializations.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace FIEAGameEngine;
using namespace UnitTests;
using namespace std;
using namespace std::string_literals;

namespace UnitTestLibraryDesktop
{
	TEST_CLASS(FooTests)
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

		TEST_METHOD(RTTITests) 
		{
			{
				Foo a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Foo::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(Foo::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				Foo* f = rtti->As<Foo>();
				Assert::IsNotNull(f);
				Assert::AreEqual(&a, f);

				Assert::AreEqual(string("0"), rtti->ToString());
				a = Foo(10);
				Assert::AreEqual(string("10"), rtti->ToString());

				Foo e(10);
				RTTI* eRtti = &e;
				Bar d(10);
				RTTI* dRtti = &d;
				Assert::IsTrue(rtti->Equals(eRtti));
				Assert::IsFalse(rtti->Equals(dRtti));
			}
			{
				const Foo a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Foo::TypeIdClass()));
				Assert::AreEqual(Foo::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const Foo* f = rtti->As<Foo>();
				Assert::IsNotNull(f);
				Assert::AreEqual(&a, f);

				Assert::AreEqual(string("0"), rtti->ToString());
				const Foo e(10);
				const RTTI* eRtti = &e;
				Assert::AreEqual(string("10"), eRtti->ToString());

				const Foo ff(10);
				const RTTI* fRtti = &ff;
				const Bar d(10);
				const RTTI* dRtti = &d;
				Assert::IsTrue(fRtti->Equals(eRtti));
				Assert::IsFalse(fRtti->Equals(dRtti));
			}
		}

		TEST_METHOD(Constructor)
		{
			const Foo a;
			Assert::AreEqual(0, a.Data());

			const Foo b(10);
			Assert::AreEqual(10, b.Data());
		}

		TEST_METHOD(CopySemantics)
		{
			const auto data = 10;
			const Foo a(data);
			Assert::AreEqual(data, a.Data());

			const Foo b(a);
			Assert::AreEqual(a, b);

			const Foo c = a;
			Assert::AreEqual(a, c);

			Foo d;
			d = a;
			Assert::AreEqual(c, a);
		}

		TEST_METHOD(EqualityOperators)
		{
			const Foo a(10);
			const Foo b(a);
			Assert::AreEqual(a, b);

			const Foo c;
			Assert::IsTrue(c != a); // Assert::AreNotEqual does not invoke operator!=
		}

		TEST_METHOD(SetData)
		{
			Foo a;
			Assert::AreEqual(0, a.Data());

			const int data = 10;
			a.SetData(data);
			Assert::AreEqual(data, a.Data());
		}

		TEST_METHOD(MoveSemantics)
		{
			const int data = 10;
			Foo a(data);
			Assert::AreEqual(data, a.Data());

			Foo b(std::move(a));
			Assert::AreEqual(data, b.Data());

			Foo c;
			Assert::AreEqual(0, c.Data());
			c = std::move(b);
			Assert::AreEqual(data, c.Data());
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}