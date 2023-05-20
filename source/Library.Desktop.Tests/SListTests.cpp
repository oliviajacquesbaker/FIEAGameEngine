/// <summary>
/// A suits of unit tests to exercise SList's capabilities and correctness, including its enveloped classes Iterator and ConstIterator. Checks for memory leaks as well. 
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "SList.h"
#include "Foo.h"
#include "ToStringSpecializations.h"
#include <algorithm>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace FIEAGameEngine;
using namespace UnitTests;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<SList<Foo>::Iterator>(const SList<Foo>::Iterator& t) {
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
	std::wstring ToString<SList<Foo>::ConstIterator>(const SList<Foo>::ConstIterator& t) {
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

namespace LibraryDesktopTests
{
	TEST_CLASS(SListTests)
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

		TEST_METHOD(Constructor)
		{
			const SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());
			Assert::ExpectException<std::runtime_error>([&list]() {auto& front = list.Front(); UNREFERENCED_LOCAL(front); });
			Assert::ExpectException<std::runtime_error>([&list]() {auto& back = list.Back(); UNREFERENCED_LOCAL(back); });
		}

		TEST_METHOD(InitializerListConstructor)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			SList<Foo> list{ a, b, c };

			Assert::AreEqual(size_t(3), list.Size());
			Assert::AreEqual(a, list.Front());
			Assert::AreNotSame(a, list.Front());
			Assert::AreEqual(c, list.Back());
			Assert::AreNotSame(c, list.Back());
			SList<Foo>::Iterator iter = list.Find(b);
			Assert::AreNotEqual(iter, list.end());
		}

		TEST_METHOD(CopySemantics)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			SList<Foo> list;

			list.PushBack(a);
			list.PushBack(b);
			list.PushBack(c);
			Assert::AreEqual(size_t(3), list.Size());
			Assert::AreEqual(a, list.Front());
			Assert::AreNotSame(a, list.Front());
			Assert::AreEqual(c, list.Back());
			Assert::AreNotSame(c, list.Back());

			{
				SList<Foo> copyOfList{ list };
				Assert::AreNotSame(list.Front(), copyOfList.Front());
				Assert::AreNotSame(list.Back(), copyOfList.Back());
				//testing std::equal with both ConstIterator and Iterator here to exercise both classes
				Assert::IsTrue(std::equal(list.cbegin(), list.cend(), copyOfList.cbegin()));
				Assert::IsTrue(std::equal(list.begin(), list.end(), copyOfList.begin()));
			}
			{
				SList<Foo> copyOfList = list;
				Assert::AreNotSame(list.Front(), copyOfList.Front());
				Assert::AreNotSame(list.Back(), copyOfList.Back());
				Assert::IsTrue(std::equal(list.cbegin(), list.cend(), copyOfList.cbegin()));
			}
			{
				SList<Foo> copyOfList;
				copyOfList = list;
				Assert::AreNotSame(list.Front(), copyOfList.Front());
				Assert::AreNotSame(list.Back(), copyOfList.Back());
				Assert::IsTrue(std::equal(list.cbegin(), list.cend(), copyOfList.cbegin()));
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			SList<Foo> list{ a, b, c };

			SList<Foo> anotherList{ std::move(list) };
			Assert::AreEqual(std::size_t(3), anotherList.Size());
			Assert::AreEqual(a, anotherList.Front());
			Assert::AreEqual(c, anotherList.Back());

#pragma warning(push)
#pragma warning(disable:26800)
			Assert::IsTrue(list.IsEmpty());
#pragma warning(pop)

			SList<Foo> yetAnotherList{ c, b, a };
			yetAnotherList = std::move(anotherList);
			Assert::AreEqual(std::size_t(3), yetAnotherList.Size());
			Assert::AreEqual(a, yetAnotherList.Front());
			Assert::AreEqual(c, yetAnotherList.Back());

#pragma warning(push)
#pragma warning(disable:26800)
			Assert::IsTrue(anotherList.IsEmpty());
#pragma warning(pop)
		}

		TEST_METHOD(GetFront)
		{
			SList<Foo> list;
			Assert::ExpectException<std::runtime_error>([&list]() {auto& front = list.Front(); UNREFERENCED_LOCAL(front); });

			const Foo a{ 10 };
			list.PushBack(a);			
			const Foo& b = list.Front();
			Assert::AreEqual(a, b);
			Assert::AreNotSame(a, b);

			const Foo c{ 20 };
			list.PushBack(c);
			const Foo& d = list.Front();
			Assert::AreEqual(a, d);
			Assert::AreNotSame(a, d);
		}

		TEST_METHOD(GetFrontConst)
		{
			const SList<Foo> list;
			Assert::ExpectException<std::runtime_error>([&list]() {auto& front = list.Front(); UNREFERENCED_LOCAL(front); });

			SList<Foo> anotherList;
			const Foo a{ 10 };
			anotherList.PushBack(a);
			{
				const SList<Foo> yetAnotherList{ anotherList };
				const Foo& b = yetAnotherList.Front();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);
			}

			const Foo c{ 20 };
			anotherList.PushBack(c);
			{
				const SList<Foo> yetAnotherList{ anotherList };
				const Foo& b = yetAnotherList.Front();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);
			}
		}

		TEST_METHOD(GetBack)
		{
			SList<Foo> list;
			Assert::ExpectException<std::runtime_error>([&list]() {auto& back = list.Back(); UNREFERENCED_LOCAL(back); });

			const Foo a{ 10 };
			list.PushBack(a);
			const Foo& b = list.Back();
			Assert::AreEqual(a, b);
			Assert::AreNotSame(a, b);

			const Foo c{ 20 };
			list.PushBack(c);
			const Foo& d = list.Back();
			Assert::AreEqual(c, d);
			Assert::AreNotSame(c, d);
		}

		TEST_METHOD(GetBackConst)
		{
			const SList<Foo> list;
			Assert::ExpectException<std::runtime_error>([&list]() {auto& back = list.Back(); UNREFERENCED_LOCAL(back); });

			SList<Foo> anotherList;
			const Foo a{ 10 };
			anotherList.PushBack(a);
			{
				const SList<Foo> yetAnotherList{ anotherList };
				const Foo& b = yetAnotherList.Back();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);
			}

			const Foo c{ 20 };
			anotherList.PushBack(c);
			{
				const SList<Foo> yetAnotherList{ anotherList };
				const Foo& b = yetAnotherList.Back();
				Assert::AreEqual(c, b);
				Assert::AreNotSame(c, b);
			}
		}

		TEST_METHOD(GetSize)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			const Foo a{ 10 };
			list.PushBack(a);
			Assert::AreEqual(size_t(1), list.Size());

			list.PushBack(a);
			Assert::AreEqual(size_t(2), list.Size());

			list.PopFront();
			Assert::AreEqual(size_t(1), list.Size());

			list.PopBack();
			Assert::AreEqual(size_t(0), list.Size());
		}

		TEST_METHOD(GetIsEmpty)
		{
			SList<Foo> list;
			Assert::IsTrue(list.IsEmpty());

			const Foo a{ 10 };
			list.PushBack(a);
			Assert::IsFalse(list.IsEmpty());

			list.PopFront();
			Assert::IsTrue(list.IsEmpty());
		}

		TEST_METHOD(PushFront)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };

			SList<Foo>::Iterator iter = list.PushFront(a);		
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(a, *iter);
			Assert::AreEqual(size_t(1), list.Size());
			Assert::AreEqual(a, list.Front());
			Assert::AreNotSame(a, list.Front());
			Assert::AreEqual(a, list.Back());
			Assert::AreNotSame(a, list.Back());

			iter = list.PushFront(b);
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(b, *iter);
			Assert::AreEqual(size_t(2), list.Size());
			Assert::AreEqual(b, list.Front());
			Assert::AreNotSame(b, list.Front());
			Assert::AreEqual(a, list.Back());
			Assert::AreNotSame(a, list.Back());

			iter = list.PushFront(c);
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(c, *iter);
			Assert::AreEqual(size_t(3), list.Size());
			Assert::AreEqual(c, list.Front());
			Assert::AreNotSame(c, list.Front());
			Assert::AreEqual(a, list.Back());
			Assert::AreNotSame(a, list.Back());
		}

		TEST_METHOD(PushFrontMoveSemantics)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			Foo a{ 10 };
			Foo aDup{ 10 };
			Foo b{ 20 };
			Foo c{ 30 };
			Foo cDup{ 30 };

			SList<Foo>::Iterator iter = list.PushFront(std::move(a));
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(aDup, *iter);
			Assert::AreEqual(size_t(1), list.Size());
			Assert::AreEqual(aDup, list.Front());
			Assert::AreNotSame(aDup, list.Front());
			Assert::AreEqual(aDup, list.Back());
			Assert::AreNotSame(aDup, list.Back());

			iter = list.PushFront(Foo(20));
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(b, *iter);
			Assert::AreEqual(size_t(2), list.Size());
			Assert::AreEqual(b, list.Front());
			Assert::AreNotSame(b, list.Front());
			Assert::AreEqual(aDup, list.Back());
			Assert::AreNotSame(aDup, list.Back());

			iter = list.PushFront(std::move(c));
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(cDup, *iter);
			Assert::AreEqual(size_t(3), list.Size());
			Assert::AreEqual(cDup, list.Front());
			Assert::AreNotSame(cDup, list.Front());
			Assert::AreEqual(aDup, list.Back());
			Assert::AreNotSame(aDup, list.Back());
		}

		TEST_METHOD(PushBack)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };

			SList<Foo>::Iterator iter = list.PushBack(a);
			Assert::AreNotEqual(list.end(), iter);
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(a, *iter);
			Assert::AreEqual(size_t(1), list.Size());
			Assert::AreEqual(a, list.Front());
			Assert::AreNotSame(a, list.Front());
			Assert::AreEqual(a, list.Back());
			Assert::AreNotSame(a, list.Back());

			iter = list.PushBack(b);
			Assert::AreNotEqual(list.begin(), iter);
			Assert::AreNotEqual(list.end(), iter);
			Assert::AreEqual(b, *iter);
			Assert::AreEqual(size_t(2), list.Size());
			Assert::AreEqual(a, list.Front());
			Assert::AreNotSame(a, list.Front());
			Assert::AreEqual(b, list.Back());
			Assert::AreNotSame(b, list.Back());

			iter = list.PushBack(c);
			Assert::AreNotEqual(list.begin(), iter);
			Assert::AreNotEqual(list.end(), iter);
			Assert::AreEqual(c, *iter);
			Assert::AreEqual(size_t(3), list.Size());
			Assert::AreEqual(a, list.Front());
			Assert::AreNotSame(a, list.Front());
			Assert::AreEqual(c, list.Back());
			Assert::AreNotSame(c, list.Back());
		}

		TEST_METHOD(PushBackMoveSemantics)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			Foo a{ 10 };
			Foo aDup{ 10 };
			Foo b{ 20 };
			Foo c{ 30 };
			Foo cDup{ 30 };

			SList<Foo>::Iterator iter = list.PushBack(std::move(a));
			Assert::AreNotEqual(list.end(), iter);
			Assert::AreEqual(list.begin(), iter);
			Assert::AreEqual(aDup, *iter);
			Assert::AreEqual(size_t(1), list.Size());
			Assert::AreEqual(aDup, list.Front());
			Assert::AreNotSame(aDup, list.Front());
			Assert::AreEqual(aDup, list.Back());
			Assert::AreNotSame(aDup, list.Back());

			iter = list.PushBack(Foo(20));
			Assert::AreNotEqual(list.begin(), iter);
			Assert::AreNotEqual(list.end(), iter);
			Assert::AreEqual(b, *iter);
			Assert::AreEqual(size_t(2), list.Size());
			Assert::AreEqual(aDup, list.Front());
			Assert::AreNotSame(aDup, list.Front());
			Assert::AreEqual(b, list.Back());
			Assert::AreNotSame(b, list.Back());

			iter = list.PushBack(std::move(c));
			Assert::AreNotEqual(list.begin(), iter);
			Assert::AreNotEqual(list.end(), iter);
			Assert::AreEqual(cDup, *iter);
			Assert::AreEqual(size_t(3), list.Size());
			Assert::AreEqual(aDup, list.Front());
			Assert::AreNotSame(aDup, list.Front());
			Assert::AreEqual(cDup, list.Back());
			Assert::AreNotSame(cDup, list.Back());
		}

		TEST_METHOD(PopFront)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			list.PushFront(a);
			list.PushFront(b);
			list.PushFront(c);
			Assert::AreEqual(size_t(3), list.Size());

			list.PopFront();
			Assert::AreEqual(size_t(2), list.Size());
			Assert::AreEqual(b, list.Front());
			Assert::AreNotSame(b, list.Front());
			Assert::AreEqual(a, list.Back());
			Assert::AreNotSame(a, list.Back());

			list.PopFront();
			Assert::AreEqual(size_t(1), list.Size());
			Assert::AreEqual(a, list.Front());
			Assert::AreNotSame(a, list.Front());
			Assert::AreEqual(a, list.Back());
			Assert::AreNotSame(a, list.Back());

			list.PopFront();
			Assert::AreEqual(size_t(0), list.Size());

			Assert::ExpectException<std::runtime_error>([&list]() {auto& front = list.Front(); UNREFERENCED_LOCAL(front); });
			Assert::ExpectException<std::runtime_error>([&list]() {auto& back = list.Back(); UNREFERENCED_LOCAL(back); });

			Assert::ExpectException<std::runtime_error>([&list]() {list.PopFront();});
		}

		TEST_METHOD(PopBack)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			list.PushFront(a);
			list.PushFront(b);
			list.PushFront(c);
			Assert::AreEqual(size_t(3), list.Size());

			list.PopBack();
			Assert::AreEqual(size_t(2), list.Size());
			Assert::AreEqual(c, list.Front());
			Assert::AreNotSame(c, list.Front());
			Assert::AreEqual(b, list.Back());
			Assert::AreNotSame(b, list.Back());

			list.PopBack();
			Assert::AreEqual(size_t(1), list.Size());
			Assert::AreEqual(c, list.Front());
			Assert::AreNotSame(c, list.Front());
			Assert::AreEqual(c, list.Back());
			Assert::AreNotSame(c, list.Back());

			list.PopBack();
			Assert::AreEqual(size_t(0), list.Size());
			Assert::ExpectException<std::runtime_error>([&list]() {auto& front = list.Front(); UNREFERENCED_LOCAL(front); });
			Assert::ExpectException<std::runtime_error>([&list]() {auto& back = list.Back(); UNREFERENCED_LOCAL(back); });

			Assert::ExpectException<std::runtime_error>([&list]() {list.PopBack(); });
		}

		TEST_METHOD(Clear)
		{
			SList<Foo> list;
			Assert::AreEqual(size_t(0), list.Size());

			list.Clear();
			Assert::AreEqual(size_t(0), list.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			list.PushFront(a);
			list.PushFront(b);
			list.PushFront(c);
			Assert::AreEqual(size_t(3), list.Size());

			list.Clear();
			Assert::AreEqual(size_t(0), list.Size());

			list.PushFront(a);
			list.Clear();
			Assert::AreEqual(size_t(0), list.Size());
		}

		TEST_METHOD(Begin)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			SList<Foo> list{ a, b, c };

			{
				SList<Foo>::Iterator iter = list.begin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(list.end(), iter);
			}
			{
				SList<Foo>::ConstIterator iter = list.cbegin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(list.cend(), iter);
			}
		}

		TEST_METHOD(BeginConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			const SList<Foo> anotherList{ a, b, c };

			{
				SList<Foo>::ConstIterator iter = anotherList.begin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(iter, anotherList.end());
			}
			{
				SList<Foo>::ConstIterator iter = anotherList.cbegin();
				Assert::AreEqual(a, *iter);
				Assert::AreNotEqual(iter, anotherList.cend());
			}
		}

		TEST_METHOD(End)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			SList<Foo> list{ a, b, c };

			{
				SList<Foo>::Iterator iter = list.end();
				Assert::AreNotEqual(iter, list.begin());
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
			{
				SList<Foo>::ConstIterator iter = list.cend();
				Assert::AreNotEqual(iter, list.cbegin());
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
		}

		TEST_METHOD(EndConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			const SList<Foo> list{ a, b, c };

			{
				SList<Foo>::ConstIterator iter = list.end();
				Assert::AreNotEqual(iter, list.begin());
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
			{
				SList<Foo>::ConstIterator iter = list.cend();
				Assert::AreNotEqual(iter, list.cbegin());
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
		}

		TEST_METHOD(InsertAfter)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list{ a, c };
				Assert::AreEqual(size_t(2), list.Size());

				SList<Foo>::Iterator iter = list.InsertAfter(b, list.begin());
				Assert::AreEqual(size_t(3), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(c, list.Back());
				SList<Foo>::Iterator foundIter = list.Find(b);
				Assert::AreNotEqual(foundIter, list.end());

				list.InsertAfter(a, iter);
				Assert::AreEqual(size_t(4), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(c, list.Back());

				list.InsertAfter(a, list.end());
				Assert::AreEqual(size_t(5), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(a, list.Back());
			}
			{
				SList<Foo> list;
				Assert::AreEqual(size_t(0), list.Size());

				list.InsertAfter(a, list.begin());
				Assert::AreEqual(size_t(1), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(a, list.Back());

				list.InsertAfter(b, list.end());
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(b, list.Back());
			}
			{
				SList<Foo> list;
				Assert::AreEqual(size_t(0), list.Size());

				list.InsertAfter(a, list.end());
				Assert::AreEqual(size_t(1), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(a, list.Back());

				list.InsertAfter(b, list.begin());
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(b, list.Back());
			}
			{
				SList<Foo> list;
				SList<Foo>::Iterator iter;				
				Assert::ExpectException<std::runtime_error>([&iter, &list, &a]() {list.InsertAfter(a, iter); });
			}
			{
				SList<Foo> list;
				SList<Foo> anotherList;
				Assert::ExpectException<std::runtime_error>([&anotherList, &list, &a]() {list.InsertAfter(a, anotherList.begin()); });
			}
		}

		TEST_METHOD(Find)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list{ a, b, c };

				SList<Foo>::Iterator iter = list.Find(a);
				Assert::AreEqual(a, *iter);
				Assert::AreEqual(list.begin(), iter);

				iter = list.Find(Foo(69));
				Assert::AreEqual(list.end(), iter);
			}
			{
				SList<Foo> list;

				SList<Foo>::Iterator iter = list.Find(a);
				Assert::AreEqual(list.end(), iter);
			}
		}

		TEST_METHOD(FindConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const SList<Foo> list{ a, b, c };

				SList<Foo>::ConstIterator iter = list.Find(a);
				Assert::AreEqual(a, *iter);
				Assert::AreEqual(list.begin(), iter);

				iter = list.Find(Foo(69));
				Assert::AreEqual(list.end(), iter);
			}
			{
				const SList<Foo> list;

				SList<Foo>::ConstIterator iter = list.Find(a);
				Assert::AreEqual(list.end(), iter);
			}
		}

		TEST_METHOD(RemoveByValue)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list{ a, b, c };
				Assert::AreEqual(size_t(3), list.Size());

				bool removed = list.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(b, list.Front());
				Assert::AreEqual(c, list.Back());
				SList<Foo>::Iterator iter = list.Find(a);
				Assert::AreEqual(list.end(), iter);
			}
			{
				SList<Foo> list{ a, b, c };
				Assert::AreEqual(size_t(3), list.Size());

				bool removed = list.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(c, list.Back());
				SList<Foo>::Iterator iter = list.Find(b);
				Assert::AreEqual(list.end(), iter);
			}
			{
				SList<Foo> list{ a, b, c };
				Assert::AreEqual(size_t(3), list.Size());

				bool removed = list.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(b, list.Back());
				SList<Foo>::Iterator foundIter = list.Find(c);
				Assert::AreEqual(list.end(), foundIter);
			}
			{
				SList<Foo> list{ a };
				Assert::AreEqual(size_t(1), list.Size());

				bool removed = list.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), list.Size());
				Assert::AreEqual(list.end(), list.begin());
				SList<Foo>::Iterator iter = list.Find(a);
				Assert::AreEqual(list.end(), iter);

				list.PushBack(b);
				Assert::AreEqual(size_t(1), list.Size());
				Assert::AreNotEqual(list.end(), list.begin());
				Assert::AreEqual(b, list.Front());
			}
			{
				SList<Foo> list;
				Assert::AreEqual(size_t(0), list.Size());

				bool removed = list.Remove(b);
				Assert::AreEqual(size_t(0), list.Size());
				Assert::IsFalse(removed);
			}
			{
				SList<Foo> list{ a };
				Assert::AreEqual(size_t(1), list.Size());

				bool removed = list.Remove(b);
				Assert::AreEqual(size_t(1), list.Size());
				Assert::IsFalse(removed);
			}
		}

		TEST_METHOD(RemoveByPosition)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list{ a, b, c };
				Assert::AreEqual(size_t(3), list.Size());

				bool removed = list.Remove(list.begin());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(b, list.Front());
				Assert::AreEqual(c, list.Back());
				SList<Foo>::Iterator iter = list.Find(a);
				Assert::AreEqual(list.end(), iter);
			}
			{
				SList<Foo> list{ a, b, c };
				Assert::AreEqual(size_t(3), list.Size());

				SList<Foo>::Iterator iter = list.begin();
				++iter;
				bool removed = list.Remove(iter);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(c, list.Back());
				SList<Foo>::Iterator foundIter = list.Find(b);
				Assert::AreEqual(list.end(), foundIter);
			}
			{
				SList<Foo> list{ a, b, c };
				Assert::AreEqual(size_t(3), list.Size());

				SList<Foo>::Iterator iter = list.begin();
				++iter;
				++iter;
				bool removed = list.Remove(iter);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), list.Size());
				Assert::AreEqual(a, list.Front());
				Assert::AreEqual(b, list.Back());
				SList<Foo>::Iterator foundIter = list.Find(c);
				Assert::AreEqual(list.end(), foundIter);
			}
			{
				SList<Foo> list{ a };
				Assert::AreEqual(size_t(1), list.Size());

				bool removed = list.Remove(list.begin());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), list.Size());
				Assert::AreEqual(list.end(), list.begin());
				SList<Foo>::Iterator foundIter = list.Find(a);
				Assert::AreEqual(list.end(), foundIter);

				list.PushBack(b);
				Assert::AreEqual(size_t(1), list.Size());
				Assert::AreNotEqual(list.end(), list.begin());
				Assert::AreEqual(b, list.Front());

				list.PushBack(c);
				SList<Foo> anotherList{ b, c };
				Assert::IsTrue(std::equal(list.cbegin(), list.cend(), anotherList.cbegin()));
			}
			{
				SList<Foo> list{ a };
				Assert::AreEqual(size_t(1), list.Size());

				bool removed = list.Remove(list.begin());
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), list.Size());
				Assert::AreEqual(list.end(), list.begin());
				SList<Foo>::Iterator foundIter = list.Find(a);
				Assert::AreEqual(list.end(), foundIter);
			}
			{
				SList<Foo> list;
				Assert::AreEqual(size_t(0), list.Size());

				bool removed = list.Remove(list.begin());
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), list.Size());
			}
			{
				SList<Foo> list{ a };
				Assert::AreEqual(size_t(1), list.Size());

				SList<Foo>::Iterator iter;
				Assert::AreEqual(size_t(1), list.Size());
				Assert::ExpectException<std::runtime_error>([&iter, &list]() {list.Remove(iter); });
			}
			{
				SList<Foo> list{ a };
				SList<Foo> anotherList{ a };
				Assert::AreEqual(size_t(1), list.Size());

				SList<Foo>::Iterator iter = anotherList.begin();
				Assert::AreEqual(size_t(1), list.Size());
				Assert::ExpectException<std::runtime_error>([&iter, &list]() {list.Remove(iter); });
			}
			{
				SList<Foo> list{ a };
				Assert::AreEqual(size_t(1), list.Size());

				SList<Foo>::Iterator iter = list.begin();
				bool removed = list.Remove(iter);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), list.Size());
				removed = list.Remove(iter);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), list.Size());
			}
			{
				SList<Foo> list{ a };
				Assert::AreEqual(size_t(1), list.Size());

				SList<Foo>::Iterator iter = list.end();
				bool removed = list.Remove(iter);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(1), list.Size());
			}
		}

		TEST_METHOD(IteratorConstructors)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list;
				Assert::AreEqual(list.end(), list.begin());

				list.PushBack(a);
				Assert::AreNotEqual(list.end(), list.begin());
			}
			{
				SList<Foo> list{ a, b, c };
				size_t count = 0;
				for (auto it = list.begin(); it != list.end(); ++it) {
					++count;
				}
				Assert::AreEqual(list.Size(), count);
			}
			{
				SList<Foo> list{ a, b, c };
				size_t count = 0;
				count = 0;
				for (const Foo& value : list) {
					UNREFERENCED_LOCAL(value);
					++count;
				}
				Assert::AreEqual(list.Size(), count);
			}
		}

		TEST_METHOD(IteratorConstructorsConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list;
				Assert::AreEqual(list.cend(), list.cbegin());

				list.PushBack(a);
				Assert::AreNotEqual(list.cend(), list.cbegin());
			}
			{
				const SList<Foo> list{ a, b, c };
				size_t count = 0;
				for (auto it = list.cbegin(); it != list.cend(); ++it) {
					++count;
				}
				Assert::AreEqual(list.Size(), count);
			}
			{
				const SList<Foo> list{ a, b, c };
				size_t count = 0;
				count = 0;
				for (const Foo& value : list) {
					UNREFERENCED_LOCAL(value);
					++count;
				}
				Assert::AreEqual(list.Size(), count);
			}
			{
				SList<Foo> list{ a, b };
				SList<Foo>::Iterator iter = list.begin();
				SList<Foo>::ConstIterator constIter(iter);

				Assert::AreEqual(*iter, *constIter);

				++iter;
				++constIter;
				Assert::AreEqual(*iter, *constIter);
			}
		}


		TEST_METHOD(IteratorEquality)
		{
			//uses IsTrue to test != in order to evoke that operator, as AreNotEqual fails to do so.
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list;
				Assert::AreEqual(list.begin(), list.begin());
				Assert::AreEqual(list.end(), list.end());
				Assert::AreEqual(list.begin(), list.end());
			}
			{
				SList<Foo> list{ a, b, c };
				SList<Foo>::Iterator iter = list.begin();
				Assert::AreEqual(list.begin(), iter);
				iter = list.end();
				Assert::AreEqual(list.end(), iter);
				Assert::IsTrue(list.begin() != list.end());
				Assert::IsTrue(list.begin() != iter);
			}
			{
				SList<Foo> list{ a, b, c };
				SList<Foo> anotherList{ a, b, c };
				Assert::IsTrue(list.begin() != anotherList.begin());
				Assert::IsTrue(list.end() != anotherList.end());
			}
			{
				SList<Foo>::Iterator iter;
				SList<Foo>::Iterator anotherIter;
				Assert::AreEqual(iter, anotherIter);

				SList<Foo> list{ a, b, c };
				anotherIter = list.begin();
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
				SList<Foo> list;
				Assert::AreEqual(list.cbegin(), list.cbegin());
				Assert::AreEqual(list.cend(), list.cend());
				Assert::AreEqual(list.cbegin(), list.cend());
			}
			{
				SList<Foo> list{ a, b, c };
				SList<Foo>::ConstIterator iter = list.cbegin();
				Assert::AreEqual(list.cbegin(), iter);
				iter = list.cend();
				Assert::AreEqual(list.cend(), iter);
				Assert::IsTrue(list.cbegin() != list.cend());
				Assert::IsTrue(list.cbegin() != iter);
			}
			{
				SList<Foo> list{ a, b, c };
				SList<Foo> anotherList{ a, b, c };
				Assert::IsTrue(list.cbegin() != anotherList.cbegin());
				Assert::IsTrue(list.cend() != anotherList.cend());
			}
			{
				SList<Foo>::ConstIterator iter;
				SList<Foo>::ConstIterator anotherIter;
				Assert::AreEqual(iter, anotherIter);

				SList<Foo> list{ a, b, c };
				anotherIter = list.cbegin();
				Assert::IsTrue(iter != anotherIter);
			}
		}

		TEST_METHOD(IteratorDereference)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list{ a, b, c };
				SList<Foo>::Iterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);

				++iter;
				Assert::AreEqual(*iter, c);
			}
			{
				SList<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
		}

		TEST_METHOD(IteratorDereferenceConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const SList<Foo> list{ a, b, c };
				SList<Foo>::ConstIterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);

				++iter;
				Assert::AreEqual(*iter, c);
			}
			{
				SList<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
		}

		TEST_METHOD(IteratorIncrement)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				SList<Foo> list{ a, b, c };
				SList<Foo>::Iterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);
				++iter;
				Assert::AreEqual(*iter, c);
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
			{
				SList<Foo> list{ a, b, c };
				SList<Foo>::Iterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				SList<Foo>::Iterator anotherIter = ++iter;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, b);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				SList<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {++iter; });
			}
			{
				SList<Foo> list{ a, b, c };
				SList<Foo>::Iterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				iter++;
				Assert::AreEqual(*iter, b);
				iter++;
				Assert::AreEqual(*iter, c);
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
			{
				SList<Foo> list{ a, b, c };
				SList<Foo>::Iterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				SList<Foo>::Iterator anotherIter = iter++;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				SList<Foo>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter++; });
			}
		}

		TEST_METHOD(IteratorIncrementConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const SList<Foo> list{ a, b, c };
				SList<Foo>::ConstIterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				++iter;
				Assert::AreEqual(*iter, b);
				++iter;
				Assert::AreEqual(*iter, c);
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
			{
				const SList<Foo> list{ a, b, c };
				SList<Foo>::ConstIterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				SList<Foo>::ConstIterator anotherIter = ++iter;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, b);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				SList<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {++iter; });
			}
			{
				const SList<Foo> list{ a, b, c };
				SList<Foo>::ConstIterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				iter++;
				Assert::AreEqual(*iter, b);
				iter++;
				Assert::AreEqual(*iter, c);
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {*iter; });
			}
			{
				const SList<Foo> list{ a, b, c };
				SList<Foo>::ConstIterator iter = list.begin();
				Assert::AreEqual(*iter, a);

				SList<Foo>::ConstIterator anotherIter = iter++;
				Assert::AreEqual(*iter, b);
				Assert::AreEqual(*anotherIter, a);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				SList<Foo>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter++; });
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}
