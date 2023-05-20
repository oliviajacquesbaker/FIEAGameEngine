#include "pch.h"
#include "CppUnitTest.h"
#include "Bar.h"
#include "ToStringSpecializations.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace FIEAGameEngine;
using namespace UnitTests;
using namespace std;
using namespace std::string_literals;

namespace UnitTestLibraryDesktop
{
	TEST_CLASS(BarTests)
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
				Bar a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Foo::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(Bar::TypeIdClass(), rtti->TypeIdInstance());

				Foo* b = rtti->As<Foo>();
				Assert::IsNull(b);

				Bar* f = rtti->As<Bar>();
				Assert::IsNotNull(f);
				Assert::AreEqual(&a, f);

				Assert::AreEqual(string("0"), rtti->ToString());
				a = Bar(10);
				Assert::AreEqual(string("10"), rtti->ToString());

				Bar e(10);
				RTTI* eRtti = &e;
				Foo d(10);
				RTTI* dRtti = &d;
				Assert::IsTrue(rtti->Equals(eRtti));
				Assert::IsFalse(rtti->Equals(dRtti));
			}
			{
				const Bar a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Foo::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Bar::TypeIdClass()));
				Assert::AreEqual(Bar::TypeIdClass(), rtti->TypeIdInstance());

				const Foo* b = rtti->As<Foo>();
				Assert::IsNull(b);

				const Bar* f = rtti->As<Bar>();
				Assert::IsNotNull(f);
				Assert::AreEqual(&a, f);

				Assert::AreEqual(string("0"), rtti->ToString());
				const Bar e(10);
				const RTTI* eRtti = &e;
				Assert::AreEqual(string("10"), eRtti->ToString());

				const Bar ff(10);
				const RTTI* fRtti = &ff;
				const Foo d(10);
				const RTTI* dRtti = &d;
				Assert::IsTrue(fRtti->Equals(eRtti));
				Assert::IsFalse(fRtti->Equals(dRtti));
			}
		}

		TEST_METHOD(Constructor)
		{
			const Bar a;
			Assert::AreEqual(0, a.Data());

			const Bar b(10);
			Assert::AreEqual(10, b.Data());
		}

		TEST_METHOD(CopySemantics)
		{
			const auto data = 10;
			const Bar a(data);
			Assert::AreEqual(data, a.Data());

			const Bar b(a);
			Assert::AreEqual(a.Data(), b.Data());

			const Bar c = a;
			Assert::AreEqual(a.Data(), c.Data());

			Bar d;
			d = a;
			Assert::AreEqual(c.Data(), a.Data());
		}

		TEST_METHOD(SetData)
		{
			Bar a;
			Assert::AreEqual(0, a.Data());

			const int data = 10;
			a.SetData(data);
			Assert::AreEqual(data, a.Data());
		}

		TEST_METHOD(MoveSemantics)
		{
			const int data = 10;
			Bar a(data);
			Assert::AreEqual(data, a.Data());

			Bar b(std::move(a));
			Assert::AreEqual(data, b.Data());

			Bar c;
			Assert::AreEqual(0, c.Data());
			c = std::move(b);
			Assert::AreEqual(data, c.Data());
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}