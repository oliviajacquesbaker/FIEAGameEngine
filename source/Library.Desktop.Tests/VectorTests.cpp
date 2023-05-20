#include "pch.h"
#include "CppUnitTest.h"
#include "Vector.h"
#include "Foo.h"
#include "ToStringSpecializations.h"
#include <algorithm>
#include <limits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<Vector<Foo>::Iterator>(const Vector<Foo>::Iterator& t) {
		std::wstring value;
		try {
			value = ToString(*t);
		}
		catch (const std::exception&) {
			value = L"end()"s;
		}
		return value;
	}

	template<>
	std::wstring ToString<Vector<Foo>::ConstIterator>(const Vector<Foo>::ConstIterator& t) {
		std::wstring value;
		try {
			value = ToString(*t);
		}
		catch (const std::exception&) {
			value = L"end()"s;
		}
		return value;
	}
}

namespace  LibraryDesktopTests
{
	TEST_CLASS(VectorTests)
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


		TEST_METHOD(DefaultConstructor)
		{
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(0), vector.Capacity());
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& front = vector.Front(); UNREFERENCED_LOCAL(front); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& back= vector.Back(); UNREFERENCED_LOCAL(back); });
		}

		TEST_METHOD(InitializerListConstructor)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);

			Vector<Foo> vector{ a, b, c };
			Assert::AreEqual(size_t(3), vector.Size());
			Assert::AreEqual(a, vector.Front());
			Assert::AreNotSame(a, vector.Front());
			Assert::AreEqual(c, vector.Back());
			Assert::AreNotSame(c, vector.Back());
			Assert::AreEqual(b, vector[1]);
			Assert::AreNotSame(b, vector[1]);
		}

		TEST_METHOD(CopySemantics)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			Vector<Foo> vector;

			vector.PushBack(a);
			vector.PushBack(b);
			vector.PushBack(c);
			Assert::AreEqual(size_t(3), vector.Size());
			Assert::AreEqual(a, vector.Front());
			Assert::AreNotSame(a, vector.Front());
			Assert::AreEqual(c, vector.Back());
			Assert::AreNotSame(c, vector.Back());

			{
				Vector<Foo> copyOfList{ vector };
				Assert::AreNotSame(vector.Front(), copyOfList.Front());
				Assert::AreNotSame(vector.Back(), copyOfList.Back());
				//testing std::equal with both ConstIterator and Iterator here to exercise both classes
				Assert::IsTrue(std::equal(vector.cbegin(), vector.cend(), copyOfList.cbegin()));
				Assert::IsTrue(std::equal(vector.begin(), vector.end(), copyOfList.begin()));
			}
			{
				Vector<Foo> copyOfList = vector;
				Assert::AreNotSame(vector.Front(), copyOfList.Front());
				Assert::AreNotSame(vector.Back(), copyOfList.Back());
				Assert::IsTrue(std::equal(vector.cbegin(), vector.cend(), copyOfList.cbegin()));
			}
			{
				Vector<Foo> copyOfList;
				copyOfList = vector;
				Assert::AreNotSame(vector.Front(), copyOfList.Front());
				Assert::AreNotSame(vector.Back(), copyOfList.Back());
				Assert::IsTrue(std::equal(vector.cbegin(), vector.cend(), copyOfList.cbegin()));
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			Vector<Foo> vector{ a, b, c };

			Vector<Foo> anotherVector{ std::move(vector) };
			Assert::AreEqual(std::size_t(3), anotherVector.Size());
			Assert::AreEqual(a, anotherVector[0]);
			Assert::AreEqual(b, anotherVector[1]);
			Assert::AreEqual(c, anotherVector[2]);

#pragma warning(push)
#pragma warning(disable:26800)
			Assert::IsTrue(vector.IsEmpty());
#pragma warning(pop)

			Vector<Foo> yetAnotherList{ c, b, a };
			yetAnotherList = std::move(anotherVector);
			Assert::AreEqual(std::size_t(3), yetAnotherList.Size());
			Assert::AreEqual(a, yetAnotherList[0]);
			Assert::AreEqual(b, yetAnotherList[1]);
			Assert::AreEqual(c, yetAnotherList[2]);

#pragma warning(push)
#pragma warning(disable:26800)
			Assert::IsTrue(anotherVector.IsEmpty());
#pragma warning(pop)
		}

		TEST_METHOD(BracketAccessOperator)
		{
			const Foo a(10);
			const Foo b(10);
			const Foo c(20);

			Vector<Foo> vector{ a, b, c };
			Assert::AreEqual(size_t(3), vector.Size());
			Foo& front = vector[0];
			Assert::AreEqual(a, front);
			Assert::AreNotSame(a, vector[0]);
			Assert::AreEqual(b, vector[1]);
			Assert::AreNotSame(b, vector[1]);
			Assert::AreEqual(c, vector[2]);
			Assert::AreNotSame(c, vector[2]);

			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector[3]; UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector[10]; UNREFERENCED_LOCAL(value); });

			vector.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector[3]; UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector[10]; UNREFERENCED_LOCAL(value); });

			vector.PushBack(a);
			Assert::AreEqual(a, vector[3]);
			Assert::AreNotSame(a, vector[3]);
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector[4]; UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(BracketAccessOperatorConst)
		{
			const Foo a(10);
			const Foo b(10);
			const Foo c(20);

			const Vector<Foo> vector{ a, b, c };
			Assert::AreEqual(size_t(3), vector.Size());
			const Foo& front = vector[0];
			Assert::AreEqual(a, front);
			Assert::AreNotSame(a, vector[0]);
			Assert::AreEqual(b, vector[1]);
			Assert::AreNotSame(b, vector[1]);
			Assert::AreEqual(c, vector[2]);
			Assert::AreNotSame(c, vector[2]);

			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector[3]; UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector[10]; UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(At)
		{
			const Foo a(10);
			const Foo b(10);
			const Foo c(20);

			Vector<Foo> vector{ a, b, c };
			Assert::AreEqual(size_t(3), vector.Size());
			Foo& front = vector.At(0);
			Assert::AreEqual(a, front);
			Assert::AreNotSame(a, vector.At(0));
			Assert::AreEqual(b, vector.At(1));
			Assert::AreNotSame(b, vector.At(1));
			Assert::AreEqual(c, vector.At(2));
			Assert::AreNotSame(c, vector.At(2));

			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector.At(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector.At(10); UNREFERENCED_LOCAL(value); });

			vector.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector.At(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector.At(10); UNREFERENCED_LOCAL(value); });

			vector.PushBack(a);
			Assert::AreEqual(a, vector.At(3));
			Assert::AreNotSame(a, vector.At(3));
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector.At(4); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(AtConst)
		{
			const Foo a(10);
			const Foo b(10);
			const Foo c(20);

			const Vector<Foo> vector{ a, b, c };
			Assert::AreEqual(size_t(3), vector.Size());
			const Foo& front = vector.At(0);
			Assert::AreEqual(a, front);
			Assert::AreNotSame(a, vector.At(0));
			Assert::AreEqual(b, vector.At(1));
			Assert::AreNotSame(b, vector.At(1));
			Assert::AreEqual(c, vector.At(2));
			Assert::AreNotSame(c, vector.At(2));

			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector.At(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& value = vector.At(10); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetSize)
		{
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());

			const Foo a{ 10 };
			vector.PushBack(a);
			Assert::AreEqual(size_t(1), vector.Size());

			vector.PushBack(a);
			Assert::AreEqual(size_t(2), vector.Size());

			vector.PopBack();
			Assert::AreEqual(size_t(1), vector.Size());

			vector.PopBack();
			Assert::AreEqual(size_t(0), vector.Size());

			const Vector<Foo> anotherVector{ a };
			Assert::AreEqual(size_t(1), anotherVector.Size());
		}

		TEST_METHOD(GetCapacity)
		{
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Capacity());

			const Foo a{ 10 };
			vector.PushBack(a);
			Assert::AreEqual(size_t(1), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(2), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(3), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(4), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(6), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(6), vector.Capacity());

			vector.Reserve(size_t(10));
			Assert::AreEqual(size_t(10), vector.Capacity());
		}

		TEST_METHOD(GetIsEmpty)
		{
			Vector<Foo> vector;
			Assert::IsTrue(vector.IsEmpty());

			const Foo a{ 10 };
			vector.PushBack(a);
			Assert::IsFalse(vector.IsEmpty());

			vector.PopBack();
			Assert::IsTrue(vector.IsEmpty());
		}

		TEST_METHOD(Reserve)
		{
			const Foo a{ 10 };
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(0), vector.Capacity());

			vector.Reserve(1);
			vector.PushBack(a);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());
			Assert::AreEqual(a, vector.Front());

			vector.Reserve(5);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(5), vector.Capacity());
			Assert::AreEqual(a, vector.Front());

			vector.Reserve(0);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(5), vector.Capacity());
			Assert::AreEqual(a, vector.Front());

			vector.PopBack();
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(5), vector.Capacity());
		}

		TEST_METHOD(ShrinkToFit)
		{
			const Foo a{ 10 };
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(0), vector.Capacity());

			vector.Reserve(size_t(10));
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(10), vector.Capacity());
			vector.ShrinkToFit();
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(0), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());
			vector.ShrinkToFit();
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());

			vector.PopBack();
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());
			vector.ShrinkToFit();
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(0), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());
			Assert::AreEqual(a, vector.Front());
			vector.Reserve(10);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(10), vector.Capacity());
			Assert::AreEqual(a, vector.Front());
			vector.ShrinkToFit();
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());
			Assert::AreEqual(a, vector.Front());
		}

		TEST_METHOD(Clear)
		{
			const Foo a{ 10 };
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(0), vector.Capacity());

			vector.Clear();
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(0), vector.Capacity());

			vector.PushBack(a);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());
			Assert::AreEqual(a, vector.Front());
			vector.Clear();
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(size_t(1), vector.Capacity());

			Vector<Foo> anotherVector{ a, a, a, a, a };
			Assert::AreEqual(size_t(5), anotherVector.Size());
			size_t originalCapacity = anotherVector.Capacity();
			anotherVector.Clear();
			Assert::AreEqual(size_t(0), anotherVector.Size());
			Assert::AreEqual(originalCapacity, anotherVector.Capacity());
		}

		TEST_METHOD(Begin)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			Vector<Foo> vector{ a, b, c };

			{
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(vector.end(), iter);
			}
			{
				Vector<Foo>::ConstIterator iter = vector.cbegin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(vector.cend(), iter);
			}
		}

		TEST_METHOD(BeginConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			const Vector<Foo> anotherVector{ a, b, c };

			{
				Vector<Foo>::ConstIterator iter = anotherVector.begin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(iter, anotherVector.end());
			}
			{
				Vector<Foo>::ConstIterator iter = anotherVector.cbegin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(iter, anotherVector.cend());
			}
		}

		TEST_METHOD(End)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			Vector<Foo> vector{ a, b, c };

			{
				Vector<Foo>::Iterator iter = vector.end();
				Assert::AreNotEqual(iter, vector.begin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::ConstIterator iter = vector.cend();
				Assert::AreNotEqual(iter, vector.cbegin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(EndConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			const Vector<Foo> vector{ a, b, c };

			{
				Vector<Foo>::ConstIterator iter = vector.end();
				Assert::AreNotEqual(iter, vector.begin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::ConstIterator iter = vector.cend();
				Assert::AreNotEqual(iter, vector.cbegin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(GetFront)
		{
			Vector<Foo> vector;
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& front = vector.Front(); UNREFERENCED_LOCAL(front); });

			const Foo a{ 10 };
			vector.PushBack(a);
			Foo& b = vector.Front();
			Assert::AreEqual(a, b);
			Assert::AreNotSame(a, b);

			const Foo c{ 20 };
			vector.PushBack(c);
			Foo& d = vector.Front();
			Assert::AreEqual(a, d);
			Assert::AreNotSame(a, d);
		}

		TEST_METHOD(GetFrontConst)
		{
			const Vector<Foo> vector;
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& front = vector.Front(); UNREFERENCED_LOCAL(front); });

			Vector<Foo> anotherVector;
			const Foo a{ 10 };
			anotherVector.PushBack(a);
			{
				const Vector<Foo> yetAnotherList{ anotherVector };
				const Foo& b = yetAnotherList.Front();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);
			}

			const Foo c{ 20 };
			anotherVector.PushBack(c);
			{
				const Vector<Foo> yetAnotherList{ anotherVector };
				const Foo& b = yetAnotherList.Front();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);
			}
		}

		TEST_METHOD(GetBack)
		{
			Vector<Foo> vector;
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& back = vector.Back(); UNREFERENCED_LOCAL(back); });

			const Foo a{ 10 };
			vector.PushBack(a);
			Foo& b = vector.Back();
			Assert::AreEqual(a, b);
			Assert::AreNotSame(a, b);

			const Foo c{ 20 };
			vector.PushBack(c);
			Foo& d = vector.Back();
			Assert::AreEqual(c, d);
			Assert::AreNotSame(c, d);
		}

		TEST_METHOD(GetBackConst)
		{
			const Vector<Foo> vector;
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& back = vector.Back(); UNREFERENCED_LOCAL(back); });

			Vector<Foo> anotherVector;
			const Foo a{ 10 };
			anotherVector.PushBack(a);
			{
				const Vector<Foo> yetAnotherList{ anotherVector };
				const Foo& b = yetAnotherList.Back();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);
			}

			const Foo c{ 20 };
			anotherVector.PushBack(c);
			{
				const Vector<Foo> yetAnotherList{ anotherVector };
				const Foo& b = yetAnotherList.Back();
				Assert::AreEqual(c, b);
				Assert::AreNotSame(c, b);
			}
		}

		TEST_METHOD(Find)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };

				Vector<Foo>::Iterator iter = vector.Find(a);
				Assert::AreEqual(a, *iter);
				Assert::AreEqual(vector.begin(), iter);

				iter = vector.Find(c);
				Assert::AreEqual(c, *iter);
				Assert::AreNotEqual(vector.begin(), iter);
				Assert::AreNotEqual(vector.end(), iter);

				iter = vector.Find(Foo(69));
				Assert::AreEqual(vector.end(), iter);
			}
			{
				Vector<Foo> vector;

				Vector<Foo>::Iterator iter = vector.Find(a);
				Assert::AreEqual(vector.end(), iter);
			}
		}

		TEST_METHOD(FindConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c };

				Vector<Foo>::ConstIterator iter = vector.Find(a);
				Assert::AreEqual(a, *iter);
				Assert::AreEqual(vector.begin(), iter);

				iter = vector.Find(c);
				Assert::AreEqual(c, *iter);
				Assert::AreNotEqual(vector.begin(), iter);
				Assert::AreNotEqual(vector.end(), iter);

				iter = vector.Find(Foo(69));
				Assert::AreEqual(vector.end(), iter);
			}
			{
				const Vector<Foo> vector;

				Vector<Foo>::ConstIterator iter = vector.Find(a);
				Assert::AreEqual(vector.end(), iter);
			}
		}

		TEST_METHOD(PushBack)
		{
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };

			Vector<Foo>::Iterator iter = vector.PushBack(a);
			Assert::AreNotEqual(vector.end(), iter);
			Assert::AreEqual(vector.begin(), iter);
			Assert::AreEqual(a, *iter);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(a, vector.Front());
			Assert::AreNotSame(a, vector.Front());
			Assert::AreEqual(a, vector.Back());
			Assert::AreNotSame(a, vector.Back());

			iter = vector.PushBack(b);
			Assert::AreNotEqual(vector.begin(), iter);
			Assert::AreNotEqual(vector.end(), iter);
			Assert::AreEqual(b, *iter);
			Assert::AreEqual(size_t(2), vector.Size());
			Assert::AreEqual(a, vector.Front());
			Assert::AreNotSame(a, vector.Front());
			Assert::AreEqual(b, vector.Back());
			Assert::AreNotSame(b, vector.Back());

			iter = vector.PushBack(c);
			Assert::AreNotEqual(vector.begin(), iter);
			Assert::AreNotEqual(vector.end(), iter);
			Assert::AreEqual(c, *iter);
			Assert::AreEqual(size_t(3), vector.Size());
			Assert::AreEqual(a, vector.Front());
			Assert::AreNotSame(a, vector.Front());
			Assert::AreEqual(c, vector.Back());
			Assert::AreNotSame(c, vector.Back());

			vector.Clear();
			vector.ShrinkToFit();
			vector.PushBack(a);
		}

		TEST_METHOD(PushBackMoveSemantics)
		{
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());

			Foo a{ 10 };
			Foo aDup{ 10 };
			Foo b{ 20 };
			Foo c{ 30 };
			Foo cDup{ 30 };

			Vector<Foo>::Iterator iter = vector.PushBack(std::move(a));
			Assert::AreNotEqual(vector.end(), iter);
			Assert::AreEqual(vector.begin(), iter);
			Assert::AreEqual(aDup, *iter);
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(aDup, vector.Front());
			Assert::AreNotSame(aDup, vector.Front());
			Assert::AreEqual(aDup, vector.Back());
			Assert::AreNotSame(aDup, vector.Back());

			iter = vector.PushBack(Foo(20));
			Assert::AreNotEqual(vector.begin(), iter);
			Assert::AreNotEqual(vector.end(), iter);
			Assert::AreEqual(b, *iter);
			Assert::AreEqual(size_t(2), vector.Size());
			Assert::AreEqual(aDup, vector.Front());
			Assert::AreNotSame(aDup, vector.Front());
			Assert::AreEqual(b, vector.Back());
			Assert::AreNotSame(b, vector.Back());

			iter = vector.PushBack(std::move(c));
			Assert::AreNotEqual(vector.begin(), iter);
			Assert::AreNotEqual(vector.end(), iter);
			Assert::AreEqual(cDup, *iter);
			Assert::AreEqual(size_t(3), vector.Size());
			Assert::AreEqual(aDup, vector.Front());
			Assert::AreNotSame(aDup, vector.Front());
			Assert::AreEqual(cDup, vector.Back());
			Assert::AreNotSame(cDup, vector.Back());
		}

		TEST_METHOD(RemoveByValue)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(b, vector.Front());
				Assert::AreEqual(c, vector.Back());
				Vector<Foo>::Iterator iter = vector.Find(a);
				Assert::AreEqual(vector.end(), iter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(a, vector.Front());
				Assert::AreEqual(c, vector.Back());
				Vector<Foo>::Iterator iter = vector.Find(b);
				Assert::AreEqual(vector.end(), iter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(a, vector.Front());
				Assert::AreEqual(b, vector.Back());
				Vector<Foo>::Iterator foundIter = vector.Find(c);
				Assert::AreEqual(vector.end(), foundIter);
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(vector.end(), vector.begin());
				Vector<Foo>::Iterator iter = vector.Find(a);
				Assert::AreEqual(vector.end(), iter);

				vector.PushBack(b);
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreNotEqual(vector.end(), vector.begin());
				Assert::AreEqual(b, vector.Front());
			}
			{
				Vector<Foo> vector;
				Assert::AreEqual(size_t(0), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(b);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::IsFalse(removed);
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(b);
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::IsFalse(removed);
			}
		}

		TEST_METHOD(RemoveByPosition)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(b, vector.Front());
				Assert::AreEqual(c, vector.Back());
				Vector<Foo>::Iterator iter = vector.Find(a);
				Assert::AreEqual(vector.end(), iter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());

				Vector<Foo>::Iterator iter = vector.begin();
				std::size_t originalCapacity = vector.Capacity();
				++iter;
				bool removed = vector.Remove(iter);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(a, vector.Front());
				Assert::AreEqual(c, vector.Back());
				Vector<Foo>::Iterator foundIter = vector.Find(b);
				Assert::AreEqual(vector.end(), foundIter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter = vector.begin();
				++iter;
				++iter;
				bool removed = vector.Remove(iter);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(a, vector.Front());
				Assert::AreEqual(b, vector.Back());
				Vector<Foo>::Iterator foundIter = vector.Find(c);
				Assert::AreEqual(vector.end(), foundIter);
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(vector.end(), vector.begin());
				Vector<Foo>::Iterator foundIter = vector.Find(a);
				Assert::AreEqual(vector.end(), foundIter);

				vector.PushBack(b);
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreNotEqual(vector.end(), vector.begin());
				Assert::AreEqual(b, vector.Front());

				vector.PushBack(c);
				Vector<Foo> anotherVector{ b, c };
				Assert::IsTrue(std::equal(vector.cbegin(), vector.cend(), anotherVector.cbegin()));
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(vector.end(), vector.begin());
				Vector<Foo>::Iterator foundIter = vector.Find(a);
				Assert::AreEqual(vector.end(), foundIter);
			}
			{
				Vector<Foo> vector;
				Assert::AreEqual(size_t(0), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin());
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter;
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::ExpectException<std::runtime_error>([&iter, &vector]() {vector.Remove(iter); });
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo> anotherVector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter = anotherVector.begin();
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::ExpectException<std::runtime_error>([&iter, &vector]() {vector.Remove(iter); });
			}
			{
				Vector<Foo> vector{ a, b };
				Assert::AreEqual(size_t(2), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter = vector.begin();
				bool removed = vector.Remove(iter);
				Assert::IsTrue(removed);
				Assert::AreEqual(b, vector.Front());
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(iter);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(iter);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter = vector.end();
				bool removed = vector.Remove(iter);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
		}

		TEST_METHOD(RemoveByRange)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin(), vector.begin() + 1);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(b, vector.Front());
				Assert::AreEqual(c, vector.Back());
				Vector<Foo>::Iterator iter = vector.Find(a);
				Assert::AreEqual(vector.end(), iter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.end() - 1, vector.end());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(a, vector.Front());
				Assert::AreEqual(b, vector.Back());
				Vector<Foo>::Iterator iter = vector.Find(c);
				Assert::AreEqual(vector.end(), iter);
			}
			{
				Vector<Foo> vector{ a, b, c, b };
				Assert::AreEqual(size_t(4), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin() + 1, vector.end() - 1);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(a, vector.Front());
				Assert::AreEqual(b, vector.Back());
				Vector<Foo>::Iterator foundIter = vector.Find(c);
				Assert::AreEqual(vector.end(), foundIter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin(), vector.end());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				Vector<Foo>::Iterator foundIter = vector.Find(a);
				Assert::AreEqual(vector.end(), foundIter);
				foundIter = vector.Find(b);
				Assert::AreEqual(vector.end(), foundIter);
				foundIter = vector.Find(c);
				Assert::AreEqual(vector.end(), foundIter);
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin(), vector.end());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreEqual(vector.end(), vector.begin());
				Vector<Foo>::Iterator foundIter = vector.Find(a);
				Assert::AreEqual(vector.end(), foundIter);

				vector.PushBack(b);
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
				Assert::AreNotEqual(vector.end(), vector.begin());
				Assert::AreEqual(b, vector.Front());

				vector.PushBack(c);
				Vector<Foo> anotherVector{ b, c };
				Assert::IsTrue(std::equal(vector.cbegin(), vector.cend(), anotherVector.cbegin()));
			}
			{
				Vector<Foo> vector;
				Assert::AreEqual(size_t(0), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.begin(), vector.end());
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.end(), vector.begin());
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(3), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(vector.begin(), vector.begin());
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(3), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(vector.begin(), vector.begin() - 1);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(3), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(vector.begin() - 1, vector.end());
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(3), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(vector.begin(), vector.end() + 1);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(3), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter, &vector]() {vector.Remove(vector.begin(), iter); });
				Assert::ExpectException<std::runtime_error>([&iter, &vector]() {vector.Remove(iter, vector.end()); });
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo> anotherVector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter = anotherVector.begin();
				Assert::ExpectException<std::runtime_error>([&anotherVector, &vector]() {vector.Remove(anotherVector.begin(), anotherVector.end()); });
				Assert::ExpectException<std::runtime_error>([&anotherVector, &vector]() {vector.Remove(vector.begin(), anotherVector.end()); });
				Assert::ExpectException<std::runtime_error>([&anotherVector, &vector]() {vector.Remove(anotherVector.begin(), vector.end()); });
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
			{
				Vector<Foo> vector{ a, b, c };
				Assert::AreEqual(size_t(3), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				Vector<Foo>::Iterator iter = vector.begin();
				bool removed = vector.Remove(iter, iter + 1);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(iter, iter + 1);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(iter, iter + 1);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());

				removed = vector.Remove(iter, iter + 1);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
			{
				Vector<Foo> vector{ a };
				Assert::AreEqual(size_t(1), vector.Size());
				std::size_t originalCapacity = vector.Capacity();

				bool removed = vector.Remove(vector.end(), vector.end() + 1);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(1), vector.Size());
				Assert::AreEqual(originalCapacity, vector.Capacity());
			}
		}

		TEST_METHOD(PopBack)
		{
			Vector<Foo> vector;
			Assert::AreEqual(size_t(0), vector.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			vector.PushBack(c);
			vector.PushBack(b);
			vector.PushBack(a);
			Assert::AreEqual(size_t(3), vector.Size());

			std::size_t originalCapacity = vector.Capacity();

			vector.PopBack();
			Assert::AreEqual(size_t(2), vector.Size());
			Assert::AreEqual(originalCapacity, vector.Capacity());
			Assert::AreEqual(c, vector.Front());
			Assert::AreNotSame(c, vector.Front());
			Assert::AreEqual(b, vector.Back());
			Assert::AreNotSame(b, vector.Back());

			vector.PopBack();
			Assert::AreEqual(size_t(1), vector.Size());
			Assert::AreEqual(originalCapacity, vector.Capacity());
			Assert::AreEqual(c, vector.Front());
			Assert::AreNotSame(c, vector.Front());
			Assert::AreEqual(c, vector.Back());
			Assert::AreNotSame(c, vector.Back());

			vector.PopBack();
			Assert::AreEqual(size_t(0), vector.Size());
			Assert::AreEqual(originalCapacity, vector.Capacity());
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& front = vector.Front(); UNREFERENCED_LOCAL(front); });
			Assert::ExpectException<std::runtime_error>([&vector]() {auto& back = vector.Back(); UNREFERENCED_LOCAL(back); });

			Assert::ExpectException<std::runtime_error>([&vector]() {vector.PopBack(); });
		}

		TEST_METHOD(IteratorConstructors)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector;
				Assert::AreEqual(vector.end(), vector.begin());

				vector.PushBack(a);
				Assert::AreNotEqual(vector.end(), vector.begin());
			}
			{
				Vector<Foo> vector{ a, b, c };
				size_t count = 0;
				for (auto it = vector.begin(); it != vector.end(); ++it) {
					++count;
				}
				Assert::AreEqual(vector.Size(), count);
			}
			{
				Vector<Foo> vector{ a, b, c };
				size_t count = 0;
				count = 0;
				for (const Foo& value : vector) {
					UNREFERENCED_LOCAL(value);
					++count;
				}
				Assert::AreEqual(vector.Size(), count);
			}
		}

		TEST_METHOD(IteratorConstructorsConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector;
				Assert::AreEqual(vector.cend(), vector.cbegin());

				vector.PushBack(a);
				Assert::AreNotEqual(vector.cend(), vector.cbegin());
			}
			{
				const Vector<Foo> vector{ a, b, c };
				size_t count = 0;
				for (auto it = vector.cbegin(); it != vector.cend(); ++it) {
					++count;
				}
				Assert::AreEqual(vector.Size(), count);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				size_t count = 0;
				count = 0;
				for (const Foo& value : vector) {
					UNREFERENCED_LOCAL(value);
					++count;
				}
				Assert::AreEqual(vector.Size(), count);
			}
			{
				Vector<Foo> vector{ a, b };
				Vector<Foo>::Iterator iter = vector.begin();
				Vector<Foo>::ConstIterator constIter(iter);

				Assert::AreEqual(*iter, *constIter);

				++iter;
				++constIter;
				Assert::AreEqual(*iter, *constIter);
			}
		}

		TEST_METHOD(IteratorDereference)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);

				++iter;
				Assert::AreEqual(*iter, c);

				iter = vector.end();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin() + 5;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(IteratorDereferenceConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);

				++iter;
				Assert::AreEqual(*iter, c);

				iter = vector.end();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin() + 5;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(IteratorIncrement)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);
				++iter;
				Assert::AreEqual(*iter, c);
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				Vector<Foo>::Iterator anotherIter = ++iter;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, b);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				++iter;
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {++iter; });
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter++;
				Assert::AreEqual(*iter, b);
				iter++;
				Assert::AreEqual(*iter, c);
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				Vector<Foo>::Iterator anotherIter = iter++;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter++;
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter++; });
			}
		}

		TEST_METHOD(IteratorIncrementConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);
				++iter;
				Assert::AreEqual(*iter, c);
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				Vector<Foo>::ConstIterator anotherIter = ++iter;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, b);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				++iter;
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {++iter; });
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter++;
				Assert::AreEqual(*iter, b);
				iter++;
				Assert::AreEqual(*iter, c);
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				Vector<Foo>::ConstIterator anotherIter = iter++;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter++;
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter++; });
			}
		}

		TEST_METHOD(IteratorDecrement)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.end();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				--iter;
				Assert::AreEqual(*iter, c);
				--iter;
				Assert::AreEqual(*iter, b);
				--iter;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, c);

				Vector<Foo>::Iterator anotherIter = --iter;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, b);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();
				--iter;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {--iter; });
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.end();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter--;
				Assert::AreEqual(*iter, c);
				iter--;
				Assert::AreEqual(*iter, b);
				iter--;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, c);

				Vector<Foo>::Iterator anotherIter = iter--;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, c);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();
				--iter;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter--; });
			}
		}

		TEST_METHOD(IteratorDecrementConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.end();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				--iter;
				Assert::AreEqual(*iter, c);
				--iter;
				Assert::AreEqual(*iter, b);
				--iter;
				Assert::AreEqual(*iter, a);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, c);

				Vector<Foo>::ConstIterator anotherIter = --iter;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, b);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				--iter;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {--iter; });
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.end();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter--;
				Assert::AreEqual(*iter, c);
				iter--;
				Assert::AreEqual(*iter, b);
				iter--;
				Assert::AreEqual(*iter, a);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, c);

				Vector<Foo>::ConstIterator anotherIter = iter--;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, c);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				--iter;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter--; });
			}
		}

		TEST_METHOD(IteratorDifference)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Vector<Foo>::Iterator anotherIter = vector.begin();

				Assert::AreEqual(ptrdiff_t(0), anotherIter - iter);
				Assert::AreEqual(ptrdiff_t(0), iter - anotherIter);

				++anotherIter;
				Assert::AreEqual(ptrdiff_t(1), anotherIter - iter);
				Assert::AreEqual(ptrdiff_t(-1), iter - anotherIter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Vector<Foo>::Iterator anotherIter = vector.end();

				Assert::AreEqual(ptrdiff_t(3), anotherIter - iter);
				Assert::AreEqual(ptrdiff_t(-3), iter - anotherIter);
			}
		}

		TEST_METHOD(IteratorDifferenceConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Vector<Foo>::ConstIterator anotherIter = vector.begin();

				Assert::AreEqual(ptrdiff_t(0), anotherIter - iter);
				Assert::AreEqual(ptrdiff_t(0), iter - anotherIter);

				++anotherIter;
				Assert::AreEqual(ptrdiff_t(1), anotherIter - iter);
				Assert::AreEqual(ptrdiff_t(-1), iter - anotherIter);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Vector<Foo>::ConstIterator anotherIter = vector.end();

				Assert::AreEqual(ptrdiff_t(3), anotherIter - iter);
				Assert::AreEqual(ptrdiff_t(-3), iter - anotherIter);
			}
		}

		TEST_METHOD(IteratorAdditionAssignment)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter += 2;
				Assert::AreEqual(*iter, c);
				iter += 1;
				Assert::AreEqual(*iter, a);
				iter += 1;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter += 3;
				Assert::AreEqual(*iter, b);
				iter += -1;
				Assert::AreEqual(*iter, c);
				iter += -20;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += 2;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter += 1; });
			}
		}

		TEST_METHOD(IteratorAdditionAssignmentConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter += 2;
				Assert::AreEqual(*iter, c);
				iter += 1;
				Assert::AreEqual(*iter, a);
				iter += 1;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter += 3;
				Assert::AreEqual(*iter, b);
				iter += -1;
				Assert::AreEqual(*iter, c);
				iter += -20;
				Assert::AreEqual(*iter, a);
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::AreEqual(*iter, a);

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });

				iter += 2;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter += 1; });
			}
		}

		TEST_METHOD(IteratorSubtractionAssignment)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::Iterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, a);

				iter -= 2;
				Assert::AreEqual(*iter, b);
				iter -= 1;
				Assert::AreEqual(*iter, a);
				iter -= 1;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::Iterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, b);

				iter -= 3;
				Assert::AreEqual(*iter, a);
				iter -= -1;
				Assert::AreEqual(*iter, b);
				iter -= -20;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();
				iter -= 10;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter -= 1; });
			}
		}

		TEST_METHOD(IteratorSubtractionAssignmentConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::ConstIterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, a);

				iter -= 2;
				Assert::AreEqual(*iter, b);
				iter -= 1;
				Assert::AreEqual(*iter, a);
				iter -= 1;
				Assert::AreEqual(*iter, a);
			}
			{
				const Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::ConstIterator iter = vector.end() - 1;
				Assert::AreEqual(*iter, b);

				iter -= 3;
				Assert::AreEqual(*iter, a);
				iter -= -1;
				Assert::AreEqual(*iter, b);
				iter -= -20;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				iter -= 10;
				Assert::AreEqual(*iter, a);
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter -= 1; });
			}
		}

		TEST_METHOD(IteratorBracketOperator)
		{
			const Foo a(10);
			const Foo b(10);
			const Foo c(20);

			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();

				Foo& front = iter[0];
				Assert::AreEqual(a, front);
				Assert::AreNotSame(a, iter[0]);
				Assert::AreEqual(b, iter[1]);
				Assert::AreNotSame(b, iter[1]);
				Assert::AreEqual(c, iter[2]);
				Assert::AreNotSame(c, iter[2]);

				--iter;
				Assert::AreEqual(b, *iter);
				Assert::AreNotSame(b, *iter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();

				Assert::ExpectException<std::runtime_error>([&iter]() {iter[3]; });
				Assert::ExpectException<std::runtime_error>([&iter]() {iter[10]; });

				vector.Reserve(50);
				Assert::ExpectException<std::runtime_error>([&iter]() {iter[3]; });
				Assert::ExpectException<std::runtime_error>([&iter]() {iter[10]; });

				vector.PushBack(a);
				Assert::AreEqual(a, iter[3]);
				Assert::AreNotSame(a, iter[3]);
				Assert::ExpectException<std::runtime_error>([&iter]() {iter[4]; });
			}
			{
				Vector<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter[0]; });
			}
		}

		TEST_METHOD(IteratorBracketOperatorConst)
		{
			const Foo a(10);
			const Foo b(10);
			const Foo c(20);

			{
				const Vector<Foo> vector{a, b, c};
				Vector<Foo>::ConstIterator iter = vector.begin();

				const Foo& front = iter[0];
				Assert::AreEqual(a, front);
				Assert::AreNotSame(a, iter[0]);
				Assert::AreEqual(b, iter[1]);
				Assert::AreNotSame(b, iter[1]);
				Assert::AreEqual(c, iter[2]);
				Assert::AreNotSame(c, iter[2]);

				--iter;
				Assert::AreEqual(b, *iter);
				Assert::AreNotSame(b, *iter);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();

				Assert::ExpectException<std::runtime_error>([&iter]() {iter[3]; });
				Assert::ExpectException<std::runtime_error>([&iter]() {iter[10]; });
			}
			{
				Vector<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter[0]; });
			}
		}

		TEST_METHOD(IteratorFriendAddition)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::Iterator iter = vector.begin();
				Vector<Foo>::Iterator anotherIter;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter + 2;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, c);
				iter += 2;
				anotherIter = iter + 1;
				Assert::AreEqual(*iter, c);
				Assert::AreEqual(*anotherIter, a);
				iter += 1;
				anotherIter = iter + 1;
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::Iterator iter = vector.begin();
				Vector<Foo>::Iterator anotherIter;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter + 3;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, b);
				iter += 3;
				anotherIter = iter + -1;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, c);
				anotherIter = iter + -20;
				Assert::AreEqual(*iter, b);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();
				Vector<Foo>::Iterator anotherIter;
				Assert::AreEqual(*iter, a);

				anotherIter = iter + std::numeric_limits<std::ptrdiff_t>::max();
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				anotherIter = iter + std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				anotherIter = iter + 2;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				Assert::AreEqual(*anotherIter, a);
			}
			{
				Vector<Foo>::Iterator iter;
				Vector<Foo>::Iterator anotherIter;
				Assert::ExpectException<std::runtime_error>([&iter, &anotherIter]() {anotherIter = iter + 1; });
			}
		}

		TEST_METHOD(IteratorFriendAdditionConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Vector<Foo>::ConstIterator anotherIter;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter + 2;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, c);
				iter += 2;
				anotherIter = iter + 1;
				Assert::AreEqual(*iter, c);
				Assert::AreEqual(*anotherIter, a);
				iter += 1;
				anotherIter = iter + 1;
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Vector<Foo>::ConstIterator anotherIter;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter + 3;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, b);
				iter += 3;
				anotherIter = iter + -1;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, c);
				anotherIter = iter + -20;
				Assert::AreEqual(*iter, b);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Vector<Foo>::ConstIterator anotherIter;
				Assert::AreEqual(*iter, a);

				anotherIter = iter + std::numeric_limits<std::ptrdiff_t>::max();
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				anotherIter = iter + std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				iter += std::numeric_limits<std::ptrdiff_t>::max();
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				anotherIter = iter + 2;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
				Assert::AreEqual(*anotherIter, a);
			}
			{
				Vector<Foo>::ConstIterator iter;
				Vector<Foo>::ConstIterator anotherIter;
				Assert::ExpectException<std::runtime_error>([&iter, &anotherIter]() {anotherIter = iter + 1; });
			}
		}

		TEST_METHOD(IteratorFriendSubtraction)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::Iterator iter = vector.end() - 1;
				Vector<Foo>::Iterator anotherIter;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter - 2;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, b);
				iter -= 2;
				Assert::AreEqual(*iter, b);

				anotherIter = iter - 1;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				iter -= 1;
				Assert::AreEqual(*iter, a);

				anotherIter = iter - 1;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::Iterator iter = vector.end() - 1;
				Vector<Foo>::Iterator anotherIter;
				Assert::AreEqual(*iter, b);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter - 3;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				iter -= 3;
				Assert::AreEqual(*iter, a);

				anotherIter = iter - -1;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, b);
				iter -= -1;
				Assert::AreEqual(*iter, b);

				anotherIter = iter - -20;
				Assert::AreEqual(*iter, b);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo> vector{ a };
				Vector<Foo>::Iterator iter = vector.begin();
				Vector<Foo>::Iterator anotherIter = iter;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, a);

				anotherIter = iter - -10;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::Iterator iter;
				Vector<Foo>::Iterator anotherIter;
				Assert::ExpectException<std::runtime_error>([&iter, &anotherIter]() {anotherIter = iter - 1; });
			}
		}

		TEST_METHOD(IteratorFriendSubtractionConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector{ a, b, c, a };
				Vector<Foo>::ConstIterator iter = vector.end() - 1;
				Vector<Foo>::ConstIterator anotherIter;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter - 2;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, b);
				iter -= 2;
				Assert::AreEqual(*iter, b);

				anotherIter = iter - 1;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				iter -= 1;
				Assert::AreEqual(*iter, a);

				anotherIter = iter - 1;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a, b, c, b };
				Vector<Foo>::ConstIterator iter = vector.end() - 1;
				Vector<Foo>::ConstIterator anotherIter;
				Assert::AreEqual(*iter, b);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });

				anotherIter = iter - 3;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				iter -= 3;
				Assert::AreEqual(*iter, a);

				anotherIter = iter - -1;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, b);
				iter -= -1;
				Assert::AreEqual(*iter, b);

				anotherIter = iter - -20;
				Assert::AreEqual(*iter, b);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				const Vector<Foo> vector{ a };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Vector<Foo>::ConstIterator anotherIter = iter;
				Assert::AreEqual(*iter, a);
				Assert::AreEqual(*anotherIter, a);

				anotherIter = iter - -10;
				Assert::AreEqual(*iter, a);
				Assert::ExpectException<std::runtime_error>([&anotherIter]() {auto& value = *anotherIter; UNREFERENCED_LOCAL(value); });
			}
			{
				Vector<Foo>::ConstIterator iter;
				Vector<Foo>::ConstIterator anotherIter;
				Assert::ExpectException<std::runtime_error>([&iter, &anotherIter]() {anotherIter = iter - 1; });
			}
		}

		TEST_METHOD(IteratorEquality)
		{
			//uses IsTrue to test != in order to evoke that operator, as AreNotEqual fails to do so.
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector;
				Assert::AreEqual(vector.begin(), vector.begin());
				Assert::AreEqual(vector.end(), vector.end());
				Assert::AreEqual(vector.begin(), vector.end());
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::AreEqual(vector.begin(), iter);
				iter = vector.end();
				Assert::AreEqual(vector.end(), iter);
				Assert::IsTrue(vector.begin() != vector.end());
				Assert::IsTrue(vector.begin() != iter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo> anotherVector{ a, b, c };
				Assert::IsTrue(vector.begin() != anotherVector.begin());
				Assert::IsTrue(vector.end() != anotherVector.end());
			}
			{
				Vector<Foo>::Iterator iter;
				Vector<Foo>::Iterator anotherIter;
				Assert::AreEqual(iter, anotherIter);

				Vector<Foo> vector{ a, b, c };
				anotherIter = vector.begin();
				Assert::IsTrue(iter != anotherIter);
			}
		}

		TEST_METHOD(IteratorEqualityConst)
		{
			//uses IsTrue to test != in order to evoke that operator, as AreNotEqual fails to do so.
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector;
				Assert::AreEqual(vector.cbegin(), vector.cbegin());
				Assert::AreEqual(vector.cend(), vector.cend());
				Assert::AreEqual(vector.cbegin(), vector.cend());
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.cbegin();
				Assert::AreEqual(vector.cbegin(), iter);
				iter = vector.cend();
				Assert::AreEqual(vector.cend(), iter);
				Assert::IsTrue(vector.cbegin() != vector.cend());
				Assert::IsTrue(vector.cbegin() != iter);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				const Vector<Foo> anotherVector{ a, b, c };
				Assert::IsTrue(vector.cbegin() != anotherVector.cbegin());
				Assert::IsTrue(vector.cend() != anotherVector.cend());
			}
			{
				Vector<Foo>::ConstIterator iter;
				Vector<Foo>::ConstIterator anotherIter;
				Assert::AreEqual(iter, anotherIter);

				const Vector<Foo> vector{ a, b, c };
				anotherIter = vector.cbegin();
				Assert::IsTrue(iter != anotherIter);
			}
		}

		TEST_METHOD(IteratorCompareAndEqual)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector;
				Assert::IsTrue(vector.begin() >= vector.begin());
				Assert::IsTrue(vector.begin() <= vector.begin());
				Assert::IsTrue(vector.end() >= vector.end());
				Assert::IsTrue(vector.end() <= vector.end());
				Assert::IsTrue(vector.begin() <= vector.end());
				Assert::IsTrue(vector.begin() >= vector.end());
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::IsTrue(vector.begin() >= iter);
				Assert::IsTrue(vector.begin() <= iter);

				Vector<Foo>::Iterator anotherIter = vector.end();
				Assert::IsTrue(vector.end() >= anotherIter);
				Assert::IsTrue(vector.end() <= anotherIter);

				Assert::IsTrue(iter <= anotherIter);
				Assert::IsFalse(iter >= anotherIter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo> anotherVector{ a, b, c };
				Assert::IsFalse(vector.begin() >= anotherVector.begin());
				Assert::IsFalse(vector.begin() <= anotherVector.begin());
				Assert::IsFalse(vector.end() <= anotherVector.end());
				Assert::IsFalse(vector.end() >= anotherVector.end());
			}
			{
				Vector<Foo>::Iterator iter;
				Vector<Foo>::Iterator anotherIter;
				Assert::IsTrue(iter >= anotherIter);
				Assert::IsTrue(iter <= anotherIter);

				Vector<Foo> vector{ a, b, c };
				anotherIter = vector.begin();
				Assert::IsFalse(iter >= anotherIter);
				Assert::IsFalse(iter <= anotherIter);
			}
		}

		TEST_METHOD(IteratorCompareAndEqualConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector;
				Assert::IsTrue(vector.begin() >= vector.begin());
				Assert::IsTrue(vector.begin() <= vector.begin());
				Assert::IsTrue(vector.end() >= vector.end());
				Assert::IsTrue(vector.end() <= vector.end());
				Assert::IsTrue(vector.begin() <= vector.end());
				Assert::IsTrue(vector.begin() >= vector.end());
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::IsTrue(vector.begin() >= iter);
				Assert::IsTrue(vector.begin() <= iter);

				Vector<Foo>::ConstIterator anotherIter = vector.end();
				Assert::IsTrue(vector.end() >= anotherIter);
				Assert::IsTrue(vector.end() <= anotherIter);

				Assert::IsTrue(iter <= anotherIter);
				Assert::IsFalse(iter >= anotherIter);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				const Vector<Foo> anotherVector{ a, b, c };
				Assert::IsFalse(vector.begin() >= anotherVector.begin());
				Assert::IsFalse(vector.begin() <= anotherVector.begin());
				Assert::IsFalse(vector.end() <= anotherVector.end());
				Assert::IsFalse(vector.end() >= anotherVector.end());
			}
			{
				Vector<Foo>::ConstIterator iter;
				Vector<Foo>::ConstIterator anotherIter;
				Assert::IsTrue(iter >= anotherIter);
				Assert::IsTrue(iter <= anotherIter);

				const Vector<Foo> vector{ a, b, c };
				anotherIter = vector.begin();
				Assert::IsFalse(iter >= anotherIter);
				Assert::IsFalse(iter <= anotherIter);
			}
		}

		TEST_METHOD(IteratorCompare)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				Vector<Foo> vector;
				Assert::IsFalse(vector.begin() > vector.begin());
				Assert::IsFalse(vector.begin() < vector.begin());
				Assert::IsFalse(vector.end() > vector.end());
				Assert::IsFalse(vector.end() < vector.end());
				Assert::IsFalse(vector.begin() < vector.end());
				Assert::IsFalse(vector.begin() > vector.end());
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo>::Iterator iter = vector.begin();
				Assert::IsFalse(vector.begin() > iter);
				Assert::IsFalse(vector.begin() < iter);

				Vector<Foo>::Iterator anotherIter = vector.end() - 1;
				Assert::IsFalse(vector.end() - 1 > anotherIter);
				Assert::IsFalse(vector.end() - 1 < anotherIter);

				Assert::IsTrue(iter < anotherIter);
				Assert::IsFalse(iter > anotherIter);

				iter++;
				Assert::IsTrue(iter < anotherIter);
				Assert::IsFalse(iter > anotherIter);
				anotherIter--;
				Assert::IsFalse(iter < anotherIter);
				Assert::IsFalse(iter > anotherIter);
			}
			{
				Vector<Foo> vector{ a, b, c };
				Vector<Foo> anotherVector{ a, b, c };
				Assert::IsFalse(vector.begin() > anotherVector.begin());
				Assert::IsFalse(vector.begin() < anotherVector.begin());
				Assert::IsFalse(vector.end() < anotherVector.end());
				Assert::IsFalse(vector.end() > anotherVector.end());
			}
			{
				Vector<Foo>::Iterator iter;
				Vector<Foo>::Iterator anotherIter;
				Assert::IsFalse(iter > anotherIter);
				Assert::IsFalse(iter < anotherIter);

				Vector<Foo> vector{ a, b, c };
				anotherIter = vector.begin();
				Assert::IsFalse(iter > anotherIter);
				Assert::IsFalse(iter < anotherIter);
			}
		}

		TEST_METHOD(IteratorCompareConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const Vector<Foo> vector;
				Assert::IsFalse(vector.begin() > vector.begin());
				Assert::IsFalse(vector.begin() < vector.begin());
				Assert::IsFalse(vector.end() > vector.end());
				Assert::IsFalse(vector.end() < vector.end());
				Assert::IsFalse(vector.begin() < vector.end());
				Assert::IsFalse(vector.begin() > vector.end());
			}
			{
				const Vector<Foo> vector{ a, b, c };
				Vector<Foo>::ConstIterator iter = vector.begin();
				Assert::IsFalse(vector.begin() > iter);
				Assert::IsFalse(vector.begin() < iter);

				Vector<Foo>::ConstIterator anotherIter = vector.end() - 1;
				Assert::IsFalse(vector.end() - 1 > anotherIter);
				Assert::IsFalse(vector.end() - 1 < anotherIter);

				Assert::IsTrue(iter < anotherIter);
				Assert::IsFalse(iter > anotherIter);

				iter++;
				Assert::IsTrue(iter < anotherIter);
				Assert::IsFalse(iter > anotherIter);
				anotherIter--;
				Assert::IsFalse(iter < anotherIter);
				Assert::IsFalse(iter > anotherIter);
			}
			{
				const Vector<Foo> vector{ a, b, c };
				const Vector<Foo> anotherVector{ a, b, c };
				Assert::IsFalse(vector.begin() > anotherVector.begin());
				Assert::IsFalse(vector.begin() < anotherVector.begin());
				Assert::IsFalse(vector.end() < anotherVector.end());
				Assert::IsFalse(vector.end() > anotherVector.end());
			}
			{
				Vector<Foo>::ConstIterator iter;
				Vector<Foo>::ConstIterator anotherIter;
				Assert::IsFalse(iter > anotherIter);
				Assert::IsFalse(iter < anotherIter);

				const Vector<Foo> vector{ a, b, c };
				anotherIter = vector.begin();
				Assert::IsFalse(iter > anotherIter);
				Assert::IsFalse(iter < anotherIter);
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}