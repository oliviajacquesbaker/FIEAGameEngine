/// <summary>
/// Unit tests exercising the functionality of HashMap and its Iterator and ConstIterator types.
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "Foo.h"
#include "Bar.h"
#include "ToStringSpecializations.h"
#include "HashMap.h"
#include "DefaultHash.h"
#include "DefaultEquality.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace std::string_literals;

namespace FIEAGameEngine {
	template<>
	struct DefaultEquality<const Foo> final {
		bool operator()(const Foo& lhs, const Foo& rhs) const {
			return lhs == rhs;
		}
	};

	template<>
	struct DefaultEquality<const Bar> final {
		bool operator()(const Bar& lhs, const Bar& rhs) const {
			return lhs.Data() == rhs.Data();
		}
	};

	template<>
	inline std::size_t DefaultHash<Foo>::operator()(const Foo& key) const {
		const std::size_t hashPrime = 11;
		return key.Data() * hashPrime;
	}

	template<>
	inline std::size_t DefaultHash<const Foo>::operator()(const Foo& key) const {
		const std::size_t hashPrime = 11;
		return key.Data() * hashPrime;
	}

	template<>
	inline std::size_t DefaultHash<Bar>::operator()(const Bar& key) const {
		const std::size_t hashPrime = 11;
		return key.Data() * hashPrime;
	}

	template<>
	inline std::size_t DefaultHash<const Bar>::operator()(const Bar& key) const {
		const std::size_t hashPrime = 11;
		return key.Data() * hashPrime;
	}
}

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<HashMap<Foo, int>::Iterator>(const HashMap<Foo, int>::Iterator& t) {
		std::wstring value;
		try {
			value = ToString((*t).first) + L", "s + ToString((*t).second);
		}
		catch (const std::exception&) {
			value = L"end()"s;
		}
		return value;
	}

	template<>
	std::wstring ToString<HashMap<Foo, int>::ConstIterator>(const HashMap<Foo, int>::ConstIterator& t) {
		std::wstring value;
		try {
			value = ToString((*t).first) + L", "s + ToString((*t).second);
		}
		catch (const std::exception&) {
			value = L"end()"s;
		}
		return value;
	}

	template<>
	std::wstring ToString<HashMap<Bar, int>::Iterator>(const HashMap<Bar, int>::Iterator& t) {
		std::wstring value;
		try {
			value = ToString((*t).first) + L", "s + ToString((*t).second);
		}
		catch (const std::exception&) {
			value = L"end()"s;
		}
		return value;
	}

	template<>
	std::wstring ToString<HashMap<Bar, int>::ConstIterator>(const HashMap<Bar, int>::ConstIterator& t) {
		std::wstring value;
		try {
			value = ToString((*t).first) + L", "s + ToString((*t).second);
		}
		catch (const std::exception&) {
			value = L"end()"s;
		}
		return value;
	}
}

namespace LibraryDesktopTests
{
	TEST_CLASS(HashMapTests)
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

		TEST_METHOD(FooHash)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(a);
			{
				DefaultHash<Foo> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
			}
			{
				DefaultHash<const Foo> hashFunc;

				Assert::AreEqual(hashFunc(a), hashFunc(a));
				Assert::AreNotEqual(hashFunc(a), hashFunc(b));
				Assert::AreNotEqual(hashFunc(b), hashFunc(c));
				Assert::AreEqual(hashFunc(a), hashFunc(c));
				Assert::AreNotSame(a, c);
			}
		}

		TEST_METHOD(DefaultConstructor)
		{
			HashMap<Foo, int> hashMap;
			Assert::AreEqual(size_t(0), hashMap.Size());
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());
			Assert::AreEqual(hashMap.begin(), hashMap.end());
			Assert::ExpectException<std::runtime_error>([&hashMap]() {auto& value = *(hashMap.begin()); UNREFERENCED_LOCAL(value);  });

			HashMap<Foo, int> anotherHashMap(size_t(2));
			Assert::AreEqual(size_t(0), anotherHashMap.Size());
			Assert::AreEqual(0.0, anotherHashMap.GetLoadFactor());
			Assert::AreEqual(anotherHashMap.begin(), anotherHashMap.end());
			Assert::ExpectException<std::runtime_error>([&anotherHashMap]() {auto& value = *(anotherHashMap.begin()); UNREFERENCED_LOCAL(value);});
		}

		TEST_METHOD(InitializerListConstructor)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);

			HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
			Assert::AreEqual(size_t(3), hashMap.Size());
			Assert::AreEqual(a.Data(), hashMap.At(a));
			Assert::AreNotSame(a.Data(), hashMap.At(a));
			Assert::AreEqual(b.Data(), hashMap.At(b));
			Assert::AreNotSame(b.Data(), hashMap.At(b));
			Assert::AreEqual(c.Data(), hashMap.At(c));
			Assert::AreNotSame(c.Data(), hashMap.At(c));

			HashMap<Foo, int> anotherHashMap{ {{a, a.Data()}, {b, b.Data()}, {c, c.Data()}}, 2 };
			Assert::AreEqual(size_t(3), anotherHashMap.Size());
			Assert::AreEqual(a.Data(), anotherHashMap.At(a));
			Assert::AreNotSame(a.Data(), anotherHashMap.At(a));
			Assert::AreEqual(b.Data(), anotherHashMap.At(b));
			Assert::AreNotSame(b.Data(), anotherHashMap.At(b));
			Assert::AreEqual(c.Data(), anotherHashMap.At(c));
			Assert::AreNotSame(c.Data(), anotherHashMap.At(c));
		}

		TEST_METHOD(CopySemantics)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			HashMap<Foo, int> hashMap;

			hashMap.Insert({ a, a.Data() });
			hashMap.Insert({ b, b.Data() });
			hashMap.Insert({ c, c.Data() });
			Assert::AreEqual(size_t(3), hashMap.Size());
			Assert::IsTrue(hashMap.ContainsKey(a));
			Assert::IsTrue(hashMap.ContainsKey(b));
			Assert::IsTrue(hashMap.ContainsKey(c));

			{
				HashMap<Foo, int> copyOfHashMap{ hashMap };
				Assert::AreNotSame(hashMap.begin()->first, copyOfHashMap.begin()->first);
				Assert::AreNotSame(hashMap.begin()->second, copyOfHashMap.begin()->second);
				//testing std::equal with both ConstIterator and Iterator here to exercise both classes
				Assert::IsTrue(std::equal(hashMap.cbegin(), hashMap.cend(), copyOfHashMap.cbegin()));
				Assert::IsTrue(std::equal(hashMap.begin(), hashMap.end(), copyOfHashMap.begin()));
			}
			{
				HashMap<Foo, int> copyOfHashMap = hashMap;
				Assert::AreNotSame(hashMap.begin()->first, copyOfHashMap.begin()->first);
				Assert::AreNotSame(hashMap.begin()->second, copyOfHashMap.begin()->second);
				Assert::IsTrue(std::equal(hashMap.cbegin(), hashMap.cend(), copyOfHashMap.cbegin()));
			}
			{
				HashMap<Foo, int> copyOfHashMap;
				copyOfHashMap = hashMap;
				Assert::AreNotSame(hashMap.begin()->first, copyOfHashMap.begin()->first);
				Assert::AreNotSame(hashMap.begin()->second, copyOfHashMap.begin()->second);
				Assert::IsTrue(std::equal(hashMap.cbegin(), hashMap.cend(), copyOfHashMap.cbegin()));
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };

			HashMap<Foo, int> anotherHashMap{ std::move(hashMap) };
			Assert::AreEqual(std::size_t(3), anotherHashMap.Size());
			Assert::AreEqual(a.Data(), anotherHashMap[a]);
			Assert::AreEqual(b.Data(), anotherHashMap[b]);
			Assert::AreEqual(c.Data(), anotherHashMap[c]);

			HashMap<Foo, int> yetAnotherHashMap{ {c, c.Data()}, {b, b.Data()}, {a, a.Data()} };
			yetAnotherHashMap = std::move(anotherHashMap);
			Assert::AreEqual(std::size_t(3), yetAnotherHashMap.Size());
			Assert::AreEqual(a.Data(), yetAnotherHashMap[a]);
			Assert::AreEqual(b.Data(), yetAnotherHashMap[b]);
			Assert::AreEqual(c.Data(), yetAnotherHashMap[c]);
		}

		TEST_METHOD(GetSize)
		{
			HashMap<Foo, int> hashMap;
			Assert::AreEqual(size_t(0), hashMap.Size());

			const Foo a{ 10 };
			const Foo b{ 20 };
			hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
			Assert::AreEqual(size_t(1), hashMap.Size());

			hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
			Assert::AreEqual(size_t(1), hashMap.Size());

			hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
			Assert::AreEqual(size_t(2), hashMap.Size());

			hashMap.Remove(a);
			Assert::AreEqual(size_t(1), hashMap.Size());

			hashMap.Remove(a);
			Assert::AreEqual(size_t(1), hashMap.Size());

			hashMap.Remove(b);
			Assert::AreEqual(size_t(0), hashMap.Size());

			const HashMap<Foo, int> anotherHashMap{ {a, a.Data()} };
			Assert::AreEqual(size_t(1), anotherHashMap.Size());
		}

		TEST_METHOD(GetLoadFactor)
		{
			HashMap<Foo, int> hashMap(10);
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());

			const Foo a{ 10 };
			const Foo b{ 20 };
			hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
			Assert::AreEqual(0.1, hashMap.GetLoadFactor());

			hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
			Assert::AreEqual(0.1, hashMap.GetLoadFactor());

			hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
			Assert::IsTrue(hashMap.GetLoadFactor() >= 0.1);

			hashMap.Remove(a);
			Assert::AreEqual(0.1, hashMap.GetLoadFactor());

			hashMap.Remove(a);
			Assert::AreEqual(0.1, hashMap.GetLoadFactor());

			hashMap.Remove(b);
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());

			const HashMap<Foo, int> anotherHashMap{ {{a, a.Data()}}, 10 };
			Assert::AreEqual(0.1, anotherHashMap.GetLoadFactor());
		}

		TEST_METHOD(ContainsKey)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()} };

				Assert::IsTrue(hashMap.ContainsKey(a));
				Assert::IsTrue(hashMap.ContainsKey(b));
				Assert::IsFalse(hashMap.ContainsKey(c));

				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				Assert::IsTrue(hashMap.ContainsKey(c));
			}
			{
				HashMap<Foo, int> hashMap;
				Assert::IsFalse(hashMap.ContainsKey(a));
			}
		}

		TEST_METHOD(Find)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };

				HashMap<Foo, int>::Iterator iter = hashMap.Find(a);
				Assert::AreEqual(a, iter->first);
				Assert::AreEqual(a.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(c);
				Assert::AreEqual(c, iter->first);
				Assert::AreEqual(c.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(Foo(69));
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				HashMap<Foo, int> hashMap;

				HashMap<Foo, int>::Iterator iter = hashMap.Find(a);
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });

				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				iter = hashMap.Find(a);
				Assert::AreEqual(a, iter->first);
				Assert::AreEqual(a.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);
			}
		}

		TEST_METHOD(FindConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };

				HashMap<Foo, int>::ConstIterator iter = hashMap.Find(a);
				Assert::AreEqual(a, iter->first);
				Assert::AreEqual(a.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(c);
				Assert::AreEqual(c, iter->first);
				Assert::AreEqual(c.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(Foo(69));
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				const HashMap<Foo, int> hashMap;

				HashMap<Foo, int>::ConstIterator iter = hashMap.Find(a);
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(FindNoEqualOperator)
		{
			const Bar a(10);
			const Bar b(20);
			const Bar c(30);
			DefaultEquality<const Bar> equality;
			{
				HashMap<Bar, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };

				HashMap<Bar, int>::Iterator iter = hashMap.Find(a);
				Assert::AreEqual(a.Data(), iter->first.Data());
				Assert::IsTrue(equality(a, iter->first));
				Assert::AreEqual(a.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(c);
				Assert::AreEqual(c.Data(), iter->first.Data());
				Assert::IsTrue(equality(c, iter->first));
				Assert::AreEqual(c.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(Bar(69));
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				HashMap<Bar, int> hashMap;

				HashMap<Bar, int>::Iterator iter = hashMap.Find(a);
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });

				hashMap.Insert(std::pair<Bar, int>(a, a.Data()));
				iter = hashMap.Find(a);
				Assert::AreEqual(a.Data(), iter->first.Data());
				Assert::IsTrue(equality(a, iter->first));
				Assert::AreEqual(a.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);
			}
		}


		TEST_METHOD(FindNoEqualOperatorConst)
		{
			const Bar a(10);
			const Bar b(20);
			const Bar c(30);
			DefaultEquality<const Bar> equality;
			{
				const HashMap<Bar, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };

				HashMap<Bar, int>::ConstIterator iter = hashMap.Find(a);
				Assert::AreEqual(a.Data(), iter->first.Data());
				Assert::IsTrue(equality(a, iter->first));
				Assert::AreEqual(a.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(c);
				Assert::AreEqual(c.Data(), iter->first.Data());
				Assert::IsTrue(equality(c, iter->first));
				Assert::AreEqual(c.Data(), iter->second);
				Assert::AreNotEqual(hashMap.end(), iter);

				iter = hashMap.Find(Bar(69));
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				const HashMap<Bar, int> hashMap;

				HashMap<Bar, int>::ConstIterator iter = hashMap.Find(a);
				Assert::AreEqual(hashMap.end(), iter);
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(At)
		{
			const Foo a(10);
			const Foo b(15);
			const Foo c(20);
			const Foo d(50);
			const Foo e(11);

			HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
			Assert::AreEqual(size_t(3), hashMap.Size());
			int& item = hashMap.At(a);
			Assert::AreEqual(a.Data(), item);
			Assert::AreNotSame(a.Data(), hashMap.At(a));
			Assert::AreEqual(b.Data(), hashMap.At(b));
			Assert::AreNotSame(b.Data(), hashMap.At(b));
			Assert::AreEqual(c.Data(), hashMap.At(c));
			Assert::AreNotSame(c.Data(), hashMap.At(c));

			Assert::ExpectException<std::runtime_error>([&hashMap, &d]() {auto& value = hashMap.At(d); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&hashMap, &e]() {auto& value = hashMap.At(e); UNREFERENCED_LOCAL(value); });

			hashMap.Insert({ a, b.Data() });
			Assert::AreEqual(a.Data(), hashMap.At(a));

			hashMap.Insert({ d, d.Data() });
			Assert::AreEqual(d.Data(), hashMap.At(d));
			Assert::AreNotSame(d.Data(), hashMap.At(d));

			hashMap.At(a) = e.Data();
			Assert::AreEqual(e.Data(), hashMap.At(a));
			Assert::AreNotSame(e.Data(), hashMap.At(a));
			Assert::ExpectException<std::runtime_error>([&hashMap, &e]() {hashMap.At(e) = e.Data(); });
		}

		TEST_METHOD(AtConst)
		{
			const Foo a(10);
			const Foo b(15);
			const Foo c(20);
			const Foo d(50);
			const Foo e(11);

			const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
			Assert::AreEqual(size_t(3), hashMap.Size());
			const int& item = hashMap.At(a);
			Assert::AreEqual(a.Data(), item);
			Assert::AreNotSame(a.Data(), hashMap.At(a));
			Assert::AreEqual(b.Data(), hashMap.At(b));
			Assert::AreNotSame(b.Data(), hashMap.At(b));
			Assert::AreEqual(c.Data(), hashMap.At(c));
			Assert::AreNotSame(c.Data(), hashMap.At(c));

			Assert::ExpectException<std::runtime_error>([&hashMap, &d]() {auto& value = hashMap.At(d); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&hashMap, &e]() {auto& value = hashMap.At(e); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(BracketAccessOperator)
		{
			const Foo a(10);
			const Foo b(15);
			const Foo c(20);
			const Foo d(50);
			const Foo e(11);

			HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
			Assert::AreEqual(size_t(3), hashMap.Size());
			int& item = hashMap[a];
			Assert::AreEqual(a.Data(), item);
			Assert::AreNotSame(a.Data(), hashMap[a]);
			Assert::AreEqual(b.Data(), hashMap[b]);
			Assert::AreNotSame(b.Data(), hashMap[b]);
			Assert::AreEqual(c.Data(), hashMap[c]);
			Assert::AreNotSame(c.Data(), hashMap[c]);

			int& value = hashMap[d];
			Assert::AreEqual(int(), value);
			Assert::AreNotSame(int(), value);

			Assert::AreEqual(hashMap.end(), hashMap.Find(e));
			hashMap[e] = e.Data();
			Assert::AreEqual(e.Data(), hashMap[e]);
			Assert::AreNotSame(e.Data(), hashMap[e]);

			hashMap.Insert({ a, b.Data() });
			Assert::AreEqual(a.Data(), hashMap[a]);

			Assert::AreNotEqual(hashMap.end(), hashMap.Find(e));
			hashMap.Insert({ d, d.Data() });
			Assert::AreEqual(int(), hashMap[d]);
			Assert::AreNotSame(int(), hashMap[d]);

			Assert::AreNotEqual(hashMap.end(), hashMap.Find(a));
			Assert::AreEqual(a.Data(), hashMap[a]);
			hashMap[a] = e.Data();
			Assert::AreEqual(e.Data(), hashMap[a]);
			Assert::AreNotSame(e.Data(), hashMap[a]);
		}

		TEST_METHOD(Insert)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			{
				HashMap<Foo, int> hashMap;
				Assert::AreEqual(size_t(0), hashMap.Size());

				std::pair<HashMap<Foo, int>::Iterator, bool> insertResult = hashMap.Insert({ a, a.Data() });
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(hashMap.begin(), insertResult.first);
				Assert::AreEqual(a, insertResult.first->first);
				Assert::AreEqual(a.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(1), hashMap.Size());
				Assert::AreEqual(1/13.0, hashMap.GetLoadFactor());

				insertResult = hashMap.Insert({ b, b.Data() });
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsTrue(hashMap.GetLoadFactor() >= 1/13.0);
				double previousPopulation = hashMap.GetLoadFactor();

				insertResult = hashMap.Insert({ b, c.Data() });
				Assert::IsFalse(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::AreEqual(previousPopulation, hashMap.GetLoadFactor());
			}
			{
				HashMap<Foo, int> hashMap(2);
				Assert::AreEqual(size_t(0), hashMap.Size());

				std::pair<HashMap<Foo, int>::Iterator, bool> insertResult = hashMap.Insert({ a, a.Data() });
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(hashMap.begin(), insertResult.first);
				Assert::AreEqual(a, insertResult.first->first);
				Assert::AreEqual(a.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(1), hashMap.Size());
				Assert::AreEqual(0.5, hashMap.GetLoadFactor());

				insertResult = hashMap.Insert({ b, b.Data() });
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsTrue(hashMap.GetLoadFactor() >= 0.5);
				double previousPopulation = hashMap.GetLoadFactor();

				insertResult = hashMap.Insert({ b, c.Data() });
				Assert::IsFalse(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::AreEqual(previousPopulation, hashMap.GetLoadFactor());
			}
		}

		TEST_METHOD(InsertMoveSemantics)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			{
				std::pair<Foo, int> aPair{ a, a.Data() };
				std::pair<Foo, int> bPair{ b, c.Data() };
				HashMap<Foo, int> hashMap;
				Assert::AreEqual(size_t(0), hashMap.Size());

				std::pair<HashMap<Foo, int>::Iterator, bool> insertResult = hashMap.Insert(std::move(aPair));
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(hashMap.begin(), insertResult.first);
				Assert::AreEqual(a, insertResult.first->first);
				Assert::AreEqual(a.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(1), hashMap.Size());
				Assert::AreEqual(1 / 13.0, hashMap.GetLoadFactor());

				insertResult = hashMap.Insert({ b, b.Data() });
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsTrue(hashMap.GetLoadFactor() >= 1 / 13.0);
				double previousPopulation = hashMap.GetLoadFactor();

				insertResult = hashMap.Insert(std::move(bPair));
				Assert::IsFalse(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::AreEqual(previousPopulation, hashMap.GetLoadFactor());
			}
			{
				std::pair<Foo, int> aPair{ a, a.Data() };
				std::pair<Foo, int> bPair{ b, c.Data() };
				HashMap<Foo, int> hashMap(2);
				Assert::AreEqual(size_t(0), hashMap.Size());

				std::pair<HashMap<Foo, int>::Iterator, bool> insertResult = hashMap.Insert(std::move(aPair));
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(hashMap.begin(), insertResult.first);
				Assert::AreEqual(a, insertResult.first->first);
				Assert::AreEqual(a.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(1), hashMap.Size());
				Assert::AreEqual(0.5, hashMap.GetLoadFactor());

				insertResult = hashMap.Insert({ b, b.Data() });
				Assert::IsTrue(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsTrue(hashMap.GetLoadFactor() >= 0.5);
				double previousPopulation = hashMap.GetLoadFactor();

				insertResult = hashMap.Insert(std::move(bPair));
				Assert::IsFalse(insertResult.second);
				Assert::AreNotEqual(hashMap.end(), insertResult.first);
				Assert::AreEqual(b, insertResult.first->first);
				Assert::AreEqual(b.Data(), insertResult.first->second);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::AreEqual(previousPopulation, hashMap.GetLoadFactor());
			}
		}

		TEST_METHOD(RemoveByKey)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				Assert::AreEqual(size_t(3), hashMap.Size());

				Assert::IsTrue(hashMap.ContainsKey(a));
				hashMap.Remove(a);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsFalse(hashMap.ContainsKey(a));
				Assert::IsTrue(hashMap.ContainsKey(b));
				Assert::IsTrue(hashMap.ContainsKey(c));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				Assert::AreEqual(size_t(3), hashMap.Size());

				Assert::IsTrue(hashMap.ContainsKey(b));
				hashMap.Remove(b);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsFalse(hashMap.ContainsKey(b));
				Assert::IsTrue(hashMap.ContainsKey(a));
				Assert::IsTrue(hashMap.ContainsKey(c));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				Assert::AreEqual(size_t(3), hashMap.Size());

				Assert::IsTrue(hashMap.ContainsKey(c));
				hashMap.Remove(c);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsFalse(hashMap.ContainsKey(c));
				Assert::IsTrue(hashMap.ContainsKey(a));
				Assert::IsTrue(hashMap.ContainsKey(b));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()} };
				Assert::AreEqual(size_t(1), hashMap.Size());

				Assert::IsTrue(hashMap.ContainsKey(a));
				hashMap.Remove(a);
				Assert::AreEqual(size_t(0), hashMap.Size());
				Assert::AreEqual(hashMap.end(), hashMap.begin());
				Assert::IsFalse(hashMap.ContainsKey(a));

				hashMap.Insert({ b, b.Data() });
				Assert::AreEqual(size_t(1), hashMap.Size());
				Assert::AreNotEqual(hashMap.end(), hashMap.begin());
				Assert::AreEqual(b, hashMap.begin()->first);
				Assert::AreEqual(b.Data(), hashMap.begin()->second);
			}
			{
				HashMap<Foo, int> hashMap;
				Assert::AreEqual(size_t(0), hashMap.Size());

				Assert::IsFalse(hashMap.ContainsKey(b));
				hashMap.Remove(b);
				Assert::AreEqual(size_t(0), hashMap.Size());
				Assert::IsFalse(hashMap.ContainsKey(b));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()} };
				Assert::AreEqual(size_t(1), hashMap.Size());

				Assert::IsTrue(hashMap.ContainsKey(a));
				Assert::IsFalse(hashMap.ContainsKey(b));

				hashMap.Remove(b);
				Assert::AreEqual(size_t(1), hashMap.Size());
				Assert::IsTrue(hashMap.ContainsKey(a));
				Assert::IsFalse(hashMap.ContainsKey(b));
			}
		}

		TEST_METHOD(RemoveByPosition)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				Assert::AreEqual(size_t(3), hashMap.Size());

				hashMap.Remove(hashMap.begin());
				Assert::AreEqual(size_t(2), hashMap.Size());
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				Assert::AreEqual(size_t(3), hashMap.Size());

				HashMap<Foo, int>::Iterator iter = hashMap.Find(a);
				hashMap.Remove(iter);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsFalse(hashMap.ContainsKey(a));
				Assert::IsTrue(hashMap.ContainsKey(b));
				Assert::IsTrue(hashMap.ContainsKey(c));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				Assert::AreEqual(size_t(3), hashMap.Size());

				HashMap<Foo, int>::Iterator iter = hashMap.Find(b);
				hashMap.Remove(iter);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsFalse(hashMap.ContainsKey(b));
				Assert::IsTrue(hashMap.ContainsKey(a));
				Assert::IsTrue(hashMap.ContainsKey(c));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				Assert::AreEqual(size_t(3), hashMap.Size());

				HashMap<Foo, int>::Iterator iter = hashMap.Find(c);
				hashMap.Remove(iter);
				Assert::AreEqual(size_t(2), hashMap.Size());
				Assert::IsFalse(hashMap.ContainsKey(c));
				Assert::IsTrue(hashMap.ContainsKey(a));
				Assert::IsTrue(hashMap.ContainsKey(b));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()} };
				Assert::AreEqual(size_t(1), hashMap.Size());

				hashMap.Remove(hashMap.begin());
				Assert::AreEqual(size_t(0), hashMap.Size());
				Assert::AreEqual(hashMap.end(), hashMap.begin());
				HashMap<Foo, int>::Iterator foundIter = hashMap.Find(a);
				Assert::AreEqual(hashMap.end(), foundIter);

				hashMap.Insert({ b, b.Data() });
				Assert::AreEqual(size_t(1), hashMap.Size());
				Assert::AreNotEqual(hashMap.end(), hashMap.begin());
				Assert::AreEqual(b, hashMap.begin()->first);
				Assert::AreEqual(b.Data(), hashMap.begin()->second);

				hashMap.Insert({ c, c.Data() });
				HashMap<Foo, int> anotherHashMap{ {b, b.Data()}, {c, c.Data()} };
				Assert::IsTrue(std::equal(hashMap.cbegin(), hashMap.cend(), anotherHashMap.cbegin()));
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()} };
				Assert::AreEqual(size_t(1), hashMap.Size());

				hashMap.Remove(hashMap.begin());
				Assert::AreEqual(size_t(0), hashMap.Size());
				Assert::AreEqual(hashMap.end(), hashMap.begin());
				HashMap<Foo, int>::Iterator foundIter = hashMap.Find(a);
				Assert::AreEqual(hashMap.end(), foundIter);
			}
			{
				HashMap<Foo, int> hashMap;
				Assert::AreEqual(size_t(0), hashMap.Size());

				hashMap.Remove(hashMap.begin());
				Assert::AreEqual(size_t(0), hashMap.Size());
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()} };
				Assert::AreEqual(size_t(1), hashMap.Size());

				HashMap<Foo, int>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter, &hashMap]() {hashMap.Remove(iter); });
				Assert::AreEqual(size_t(1), hashMap.Size());
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()} };
				HashMap<Foo, int> anotherHashMap{ {a, a.Data()} };
				Assert::AreEqual(size_t(1), hashMap.Size());

				HashMap<Foo, int>::Iterator iter = anotherHashMap.begin();
				Assert::ExpectException<std::runtime_error>([&iter, &hashMap]() {hashMap.Remove(iter); });
				Assert::AreEqual(size_t(1), hashMap.Size());
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()} };
				Assert::AreEqual(size_t(2), hashMap.Size());

				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				hashMap.Remove(iter);
				Assert::AreEqual(size_t(1), hashMap.Size());
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()} };
				Assert::AreEqual(size_t(1), hashMap.Size());

				HashMap<Foo, int>::Iterator iter = hashMap.end();
				hashMap.Remove(iter);
				Assert::AreEqual(size_t(1), hashMap.Size());
			}
		}

		TEST_METHOD(Clear)
		{
			const Foo a{ 10 };
			const Foo b{ 20 };
			const Foo c{ 30 };
			HashMap<Foo, int> hashMap;
			Assert::AreEqual(size_t(0), hashMap.Size());
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());

			hashMap.Clear();
			Assert::AreEqual(size_t(0), hashMap.Size());
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());

			hashMap.Insert({ a, a.Data() });
			Assert::AreEqual(size_t(1), hashMap.Size());
			Assert::AreEqual(1/13.0, hashMap.GetLoadFactor());
			Assert::AreEqual(a, hashMap.begin()->first);
			Assert::AreEqual(a.Data(), hashMap.begin()->second);
			hashMap.Clear();
			Assert::AreEqual(size_t(0), hashMap.Size());
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());

			HashMap<Foo, int> anotherHashMap{ {a, a.Data()}, {a, a.Data()}, {a, a.Data()}, {a, a.Data()}, {a, a.Data()} };
			Assert::IsTrue(anotherHashMap.ContainsKey(a));
			Assert::AreEqual(size_t(1), anotherHashMap.Size());
			Assert::AreEqual(1/13.0, anotherHashMap.GetLoadFactor());
			anotherHashMap.Clear();
			Assert::IsFalse(anotherHashMap.ContainsKey(a));
			Assert::AreEqual(size_t(0), anotherHashMap.Size());
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());

			HashMap<Foo, int> yetAnotherHashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
			Assert::IsTrue(yetAnotherHashMap.ContainsKey(a));
			Assert::IsTrue(yetAnotherHashMap.ContainsKey(b));
			Assert::IsTrue(yetAnotherHashMap.ContainsKey(c));
			Assert::AreEqual(size_t(3), yetAnotherHashMap.Size());
			Assert::IsTrue(yetAnotherHashMap.GetLoadFactor() >= 1/13.0);
			yetAnotherHashMap.Clear();
			Assert::IsFalse(yetAnotherHashMap.ContainsKey(a));
			Assert::IsFalse(yetAnotherHashMap.ContainsKey(b));
			Assert::IsFalse(yetAnotherHashMap.ContainsKey(c));
			Assert::AreEqual(size_t(0), yetAnotherHashMap.Size());
			Assert::AreEqual(0.0, hashMap.GetLoadFactor());
		}

		TEST_METHOD(Begin)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);

			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				Assert::AreNotEqual(hashMap.end(), iter);
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreNotEqual(hashMap.cend(), iter);
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}};
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				Assert::AreNotEqual(hashMap.end(), iter);
				Assert::AreEqual(a, iter->first);
				Assert::AreEqual(a.Data(), iter->second);
			}
			{
				HashMap<Foo, int> hashMap{ {a, a.Data()}};
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreNotEqual(hashMap.cend(), iter);
				Assert::AreEqual(a, iter->first);
				Assert::AreEqual(a.Data(), iter->second);
			}
			{
				HashMap<Foo, int> hashMap;
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				Assert::AreEqual(hashMap.end(), iter);
			}
			{
				HashMap<Foo, int> hashMap;
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreEqual(hashMap.cend(), iter);
			}
			//test with carefully chosen data values - looking for hash to fall into a chain greater than 0, in order to ensure begin will still get it
			{
				HashMap<Foo, int> hashMap{ {{Foo(8), 5}}, 10 };
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				Assert::AreNotEqual(hashMap.end(), iter);
				Assert::AreEqual(Foo(8), iter->first);
				Assert::AreEqual(5, iter->second);
			}
			{
				HashMap<Foo, int> hashMap{ {{Foo(8), 5}}, 10 };
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreNotEqual(hashMap.cend(), iter);
				Assert::AreEqual(Foo(8), iter->first);
				Assert::AreEqual(5, iter->second);
			}
		}

		TEST_METHOD(BeginConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.begin();
				Assert::AreNotEqual(hashMap.end(), iter);
			}
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreNotEqual(hashMap.cend(), iter);
			}
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.begin();
				Assert::AreNotEqual(hashMap.end(), iter);
				Assert::AreEqual(a, iter->first);
				Assert::AreEqual(a.Data(), iter->second);
			}
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreNotEqual(hashMap.cend(), iter);
				Assert::AreEqual(a, iter->first);
				Assert::AreEqual(a.Data(), iter->second);
			}
			{
				const HashMap<Foo, int> hashMap;
				HashMap<Foo, int>::ConstIterator iter = hashMap.begin();
				Assert::AreEqual(hashMap.end(), iter);
			}
			{
				const HashMap<Foo, int> hashMap;
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreEqual(hashMap.cend(), iter);
			}
			//test with carefully chosen data values - looking for hash to fall into a chain greater than 0, in order to ensure begin will still get it
			{
				const HashMap<Foo, int> hashMap{ {{Foo(8), 5}}, 10 };
				HashMap<Foo, int>::ConstIterator iter = hashMap.begin();
				Assert::AreNotEqual(hashMap.end(), iter);
				Assert::AreEqual(Foo(8), iter->first);
				Assert::AreEqual(5, iter->second);
			}
			{
				const HashMap<Foo, int> hashMap{ {{Foo(8), 5}}, 10 };
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreNotEqual(hashMap.cend(), iter);
				Assert::AreEqual(Foo(8), iter->first);
				Assert::AreEqual(5, iter->second);
			}
		}

		TEST_METHOD(End)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };

			{
				HashMap<Foo, int>::Iterator iter = hashMap.end();
				Assert::AreNotEqual(iter, hashMap.begin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				HashMap<Foo, int>::ConstIterator iter = hashMap.cend();
				Assert::AreNotEqual(iter, hashMap.cbegin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(EndConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };

			{
				HashMap<Foo, int>::ConstIterator iter = hashMap.end();
				Assert::AreNotEqual(iter, hashMap.begin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
			{
				HashMap<Foo, int>::ConstIterator iter = hashMap.cend();
				Assert::AreNotEqual(iter, hashMap.cbegin());
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *iter; UNREFERENCED_LOCAL(value); });
			}
		}

#pragma region IteratorTests
		TEST_METHOD(IteratorConstructors)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap;
				Assert::AreEqual(hashMap.end(), hashMap.begin());

				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				Assert::AreNotEqual(hashMap.end(), hashMap.begin());
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				size_t count = 0;
				for (auto it = hashMap.begin(); it != hashMap.end(); ++it) {
					++count;
				}
				Assert::AreEqual(size_t(3), count);
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				size_t count = 0;
				for (const std::pair<Foo, int>& value : hashMap) {
					UNREFERENCED_LOCAL(value);
					++count;
				}
				Assert::AreEqual(size_t(3), count);
			}
		}

		TEST_METHOD(IteratorConstructorsConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap;
				Assert::AreEqual(hashMap.cend(), hashMap.cbegin());

				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				Assert::AreNotEqual(hashMap.cend(), hashMap.cbegin());
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				size_t count = 0;
				for (auto it = hashMap.cbegin(); it != hashMap.cend(); ++it) {
					++count;
				}
				Assert::AreEqual(size_t(3), count);
			}
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				size_t count = 0;
				for (const std::pair<Foo, int>& value : hashMap) {
					UNREFERENCED_LOCAL(value);
					++count;
				}
				Assert::AreEqual(size_t(3), count);
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));

				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				HashMap<Foo, int>::ConstIterator constIter(iter);

				Assert::AreEqual(iter->first, constIter->first);
				Assert::AreEqual(iter->second, constIter->second);

				++iter;
				++constIter;
				Assert::AreEqual(iter->first, constIter->first);
				Assert::AreEqual(iter->second, constIter->second);
			}
		}

		TEST_METHOD(IteratorEquality)
		{
			//uses IsTrue to test != in order to evoke that operator, as AreNotEqual fails to do so.
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap;
				Assert::AreEqual(hashMap.begin(), hashMap.begin());
				Assert::AreEqual(hashMap.end(), hashMap.end());
				Assert::AreEqual(hashMap.begin(), hashMap.end());
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
								
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				Assert::AreEqual(hashMap.begin(), iter);
				iter = hashMap.end();
				Assert::AreEqual(hashMap.end(), iter);
				Assert::IsTrue(hashMap.begin() != hashMap.end());
				Assert::IsTrue(hashMap.begin() != iter);
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));

				HashMap<Foo, int> anotherHashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				Assert::IsTrue(hashMap.begin() != anotherHashMap.begin());
				Assert::IsTrue(hashMap.end() != anotherHashMap.end());
			}
			{
				HashMap<Foo, int>::Iterator iter;
				HashMap<Foo, int>::Iterator anotherIter;
				Assert::AreEqual(iter, anotherIter);

				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				anotherIter = hashMap.begin();
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
				const HashMap<Foo, int> hashMap;
				Assert::AreEqual(hashMap.cbegin(), hashMap.cbegin());
				Assert::AreEqual(hashMap.cend(), hashMap.cend());
				Assert::AreEqual(hashMap.cbegin(), hashMap.cend());
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));

				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				Assert::AreEqual(hashMap.cbegin(), iter);
				iter = hashMap.cend();
				Assert::AreEqual(hashMap.cend(), iter);
				Assert::IsTrue(hashMap.cbegin() != hashMap.cend());
				Assert::IsTrue(hashMap.cbegin() != iter);
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));

				HashMap<Foo, int> anotherHashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				Assert::IsTrue(hashMap.cbegin() != anotherHashMap.cbegin());
				Assert::IsTrue(hashMap.cend() != anotherHashMap.cend());
			}
			{
				HashMap<Foo, int>::ConstIterator iter;
				HashMap<Foo, int>::ConstIterator anotherIter;
				Assert::AreEqual(iter, anotherIter);

				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				anotherIter = hashMap.cbegin();
				Assert::IsTrue(iter != anotherIter);
			}
		}

		TEST_METHOD(IteratorDereference)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				++iter;
				Assert::AreNotEqual(beginPair.first, (*iter).first);
				Assert::AreNotEqual(beginPair.second, (*iter).second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, (*iter).first);
				Assert::AreEqual(beginPair.second, (*iter).second);

				++iter;
				Assert::AreNotEqual(beginPair.first, (*iter).first);
				Assert::AreNotEqual(beginPair.second, (*iter).second);
			}
			{
				HashMap<Foo, int>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(IteratorDereferenceConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				std::pair<Foo, int> beginPair = *iter;

				++iter;
				Assert::AreNotEqual(beginPair.first, (*iter).first);
				Assert::AreNotEqual(beginPair.second, (*iter).second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, (*iter).first);
				Assert::AreEqual(beginPair.second, (*iter).second);

				++iter;
				Assert::AreNotEqual(beginPair.first, (*iter).first);
				Assert::AreNotEqual(beginPair.second, (*iter).second);
			}
			{
				HashMap<Foo, int>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(IteratorArrowOperator)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, iter->first);
				Assert::AreEqual(beginPair.second, iter->second);

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
			}
			{
				HashMap<Foo, int>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter->first; });
				Assert::ExpectException<std::runtime_error>([&iter]() {iter->second; });
			}
		}

		TEST_METHOD(IteratorArrowOperatorConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				std::pair<Foo, int> beginPair = *iter;

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, iter->first);
				Assert::AreEqual(beginPair.second, iter->second);

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
			}
			{
				HashMap<Foo, int>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter->first; });
				Assert::ExpectException<std::runtime_error>([&iter]() {iter->second; });
			}
		}

		TEST_METHOD(IteratorIncrement)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, iter->first);
				Assert::AreEqual(beginPair.second, iter->second);

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				HashMap<Foo, int>::Iterator anotherIter = ++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				Assert::AreNotEqual(beginPair.first, (*anotherIter).first);
				Assert::AreNotEqual(beginPair.second, (*anotherIter).second);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				HashMap<Foo, int>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {++iter; });
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				iter++;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, iter->first);
				Assert::AreEqual(beginPair.second, iter->second);

				iter++;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::Iterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				HashMap<Foo, int>::Iterator anotherIter = iter++;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				Assert::AreEqual(beginPair.first, anotherIter->first);
				Assert::AreEqual(beginPair.second, anotherIter->second);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				HashMap<Foo, int>::Iterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter++; });
			}
		}

		TEST_METHOD(IteratorIncrementConst)
		{
			const Foo a(10);
			const Foo b(20);
			const Foo c(30);
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, iter->first);
				Assert::AreEqual(beginPair.second, iter->second);

				++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				++iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				const HashMap<Foo, int> hashMap{ {a, a.Data()}, {b, b.Data()}, {c, c.Data()} };
				HashMap<Foo, int>::ConstIterator iter = hashMap.begin();
				std::pair<Foo, int> beginPair = *iter;

				HashMap<Foo, int>::ConstIterator anotherIter = ++iter;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				Assert::AreNotEqual(beginPair.first, (*anotherIter).first);
				Assert::AreNotEqual(beginPair.second, (*anotherIter).second);
				Assert::AreEqual(anotherIter, iter);
			}
			{
				HashMap<Foo, int>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {++iter; });
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				std::pair<Foo, int> beginPair = *iter;

				iter++;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				beginPair = *iter;
				Assert::AreEqual(beginPair.first, iter->first);
				Assert::AreEqual(beginPair.second, iter->second);

				iter++;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				iter++;
				Assert::ExpectException<std::runtime_error>([&iter]() {auto& value = *(iter); UNREFERENCED_LOCAL(value); });
			}
			{
				HashMap<Foo, int> hashMap;
				hashMap.Insert(std::pair<Foo, int>(a, a.Data()));
				hashMap.Insert(std::pair<Foo, int>(b, b.Data()));
				hashMap.Insert(std::pair<Foo, int>(c, c.Data()));
				HashMap<Foo, int>::ConstIterator iter = hashMap.cbegin();
				std::pair<Foo, int> beginPair = *iter;

				HashMap<Foo, int>::ConstIterator anotherIter = iter++;
				Assert::AreNotEqual(beginPair.first, iter->first);
				Assert::AreNotEqual(beginPair.second, iter->second);
				Assert::AreEqual(beginPair.first, anotherIter->first);
				Assert::AreEqual(beginPair.second, anotherIter->second);
				Assert::AreNotEqual(anotherIter, iter);
			}
			{
				HashMap<Foo, int>::ConstIterator iter;
				Assert::ExpectException<std::runtime_error>([&iter]() {iter++; });
			}
		}
#pragma endregion IteratorTests

	private:
		inline static _CrtMemState _startMemState;
	};
}