#include "pch.h"
#include "CppUnitTest.h"
#include "Scope.h"
#include "Foo.h"
#include "ToStringSpecializations.h"
#include <algorithm>
#include <limits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std::string_literals;
using namespace std;
using namespace glm;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<Datum::DatumTypes>(const Datum::DatumTypes& t) {
		std::string stringResult = Datum::typeToString.At(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Datum>(const Datum& t) {
		std::wstring stringResult = ToString(t.Type());
		stringResult += L", size of "s + ToString(t.Size());

		std::string datumValues = ""s;
		for (size_t i = size_t(0); i < t.Size(); ++i) {
			datumValues += " ("s + t.ToString(i) + ") "s;
		}
		stringResult += std::wstring(datumValues.begin(), datumValues.end());

		return stringResult;
	}
	
	template<>
	std::wstring ToString<Datum>(Datum* t) {
		std::wstring stringResult = ToString(t->Type());
		stringResult += L", size of "s + ToString(t->Size());

		std::string datumValues = ""s;
		for (size_t i = size_t(0); i < t->Size(); ++i) {
			datumValues += " ("s + t->ToString(i) + ") "s;
		}
		stringResult += std::wstring(datumValues.begin(), datumValues.end());

		return stringResult;
	}

	template<>
	std::wstring ToString<Scope>(Scope* t) {
		std::string stringResult = t->ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Scope>(const Scope* t) {
		std::string stringResult = t->ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Scope>(const Scope& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace  LibraryDesktopTests 
{
	TEST_CLASS(ScopeTests)
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
				Scope a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Scope::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(Scope::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				Scope* s = rtti->As<Scope>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				a.Append("testKey") = 0;
				Assert::AreEqual(std::string("testKey: (0) "), rtti->ToString());
				a.Append("key2") = { "A"s, "B"s };
				Assert::AreEqual(std::string("testKey: (0) \nkey2: (A)  (B) "), rtti->ToString());

				Scope e;
				RTTI* eRtti = &e;
				Bar d;
				RTTI* dRtti = &d;
				Assert::IsFalse(rtti->Equals(eRtti));
				Assert::IsFalse(rtti->Equals(nullptr));

				a.Clear();
				Assert::IsTrue(rtti->Equals(eRtti));
				Assert::IsFalse(rtti->Equals(dRtti));
			}
			{
				const Scope a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Scope::TypeIdClass()));
				Assert::AreEqual(Scope::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const Scope* s = rtti->As<Scope>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Assert::AreEqual(std::string(""), rtti->ToString());

				const Scope ff;
				const RTTI* fRtti = &ff;
				const Bar d;
				const RTTI* dRtti = &d;
				Assert::IsFalse(fRtti->Equals(dRtti));
				Assert::IsFalse(fRtti->Equals(nullptr));
			}
		}

		TEST_METHOD(DefaultConstructor)
		{
			const Scope scope;
			Assert::IsTrue(scope.IsEmpty());
			Assert::AreEqual(size_t(0), scope.Size());
			Assert::IsNull(scope.GetParent());

			const Scope anotherScope(5);
			Assert::IsTrue(anotherScope.IsEmpty());
			Assert::AreEqual(size_t(0), anotherScope.Size());
			Assert::IsNull(anotherScope.GetParent());
		}

		TEST_METHOD(CopySemantics)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };

			scope.Append("A"s) = aDatum;
			scope.Append("B"s) = bDatum;
			scope.Append("C"s) = cDatum;
			Scope& childScope = scope.AppendScope("child");
			childScope.AppendScope("childChild");
			Assert::AreEqual(size_t(4), scope.Size());
			Assert::AreEqual(aDatum, scope["A"]);
			Assert::AreEqual(bDatum, scope["B"]);
			Assert::AreEqual(cDatum, scope["C"]);
			Assert::AreEqual(Datum::DatumTypes::Table, scope["child"].Type());
			Assert::AreNotSame(aDatum, scope["A"]);
			Assert::AreNotSame(bDatum, scope["B"]);
			Assert::AreNotSame(cDatum, scope["C"]);

			{
				Scope copyOfScope{ scope };
				Assert::AreNotSame(scope["A"], copyOfScope["A"]);
				Assert::AreNotSame(scope["B"], copyOfScope["B"]);
				Assert::AreNotSame(scope["C"], copyOfScope["C"]);

				Assert::AreEqual(size_t(4), copyOfScope.Size());
				Assert::AreEqual(aDatum, copyOfScope["A"]);
				Assert::AreEqual(bDatum, copyOfScope["B"]);
				Assert::AreEqual(cDatum, copyOfScope["C"]);
				Assert::AreEqual(Datum::DatumTypes::Table, copyOfScope["child"].Type());
				Assert::AreEqual(scope["A"], copyOfScope["A"]);
				Assert::AreEqual(scope["B"], copyOfScope["B"]);
				Assert::AreEqual(scope["C"], copyOfScope["C"]);
			}
			{
				Scope copyOfScope = scope;
				Assert::AreNotSame(scope["A"], copyOfScope["A"]);
				Assert::AreNotSame(scope["B"], copyOfScope["B"]);
				Assert::AreNotSame(scope["C"], copyOfScope["C"]);

				Assert::AreEqual(size_t(4), copyOfScope.Size());
				Assert::AreEqual(aDatum, copyOfScope["A"]);
				Assert::AreEqual(bDatum, copyOfScope["B"]);
				Assert::AreEqual(cDatum, copyOfScope["C"]);
				Assert::AreEqual(Datum::DatumTypes::Table, copyOfScope["child"].Type());
				Assert::AreEqual(scope["A"], copyOfScope["A"]);
				Assert::AreEqual(scope["B"], copyOfScope["B"]);
				Assert::AreEqual(scope["C"], copyOfScope["C"]);
			}
			{
				Scope copyOfScope;
				copyOfScope = scope;
				Assert::AreNotSame(scope["A"], copyOfScope["A"]);
				Assert::AreNotSame(scope["B"], copyOfScope["B"]);
				Assert::AreNotSame(scope["C"], copyOfScope["C"]);

				Assert::AreEqual(size_t(4), copyOfScope.Size());
				Assert::AreEqual(aDatum, copyOfScope["A"]);
				Assert::AreEqual(bDatum, copyOfScope["B"]);
				Assert::AreEqual(cDatum, copyOfScope["C"]);
				Assert::AreEqual(Datum::DatumTypes::Table, copyOfScope["child"].Type());
				Assert::AreEqual(scope["A"], copyOfScope["A"]);
				Assert::AreEqual(scope["B"], copyOfScope["B"]);
				Assert::AreEqual(scope["C"], copyOfScope["C"]);
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };

			scope.Append("A"s) = aDatum;
			scope.Append("B"s) = bDatum;
			scope.Append("C"s) = cDatum;
			scope.AppendScope("Scope");
			Assert::AreEqual(size_t(4), scope.Size());

			Scope anotherScope{ std::move(scope) };
			Assert::AreEqual(std::size_t(4), anotherScope.Size());
			Assert::AreEqual(aDatum, anotherScope[0]);
			Assert::AreEqual(bDatum, anotherScope[1]);
			Assert::AreEqual(cDatum, anotherScope[2]);
			Assert::AreEqual(Datum::DatumTypes::Table, anotherScope[3].Type());

#pragma warning(push)
#pragma warning(disable:26800)
			Assert::IsTrue(scope.IsEmpty());
#pragma warning(pop)

			Scope yetAnotherScope;
			yetAnotherScope.Append("C"s) = aDatum;
			yetAnotherScope.Append("B"s) = aDatum;
			yetAnotherScope.Append("A"s) = aDatum;

			yetAnotherScope = std::move(anotherScope);
			Assert::AreEqual(std::size_t(4), yetAnotherScope.Size());
			Assert::AreEqual(aDatum, yetAnotherScope[0]);
			Assert::AreEqual(bDatum, yetAnotherScope[1]);
			Assert::AreEqual(cDatum, yetAnotherScope[2]);
			Assert::AreEqual(Datum::DatumTypes::Table, yetAnotherScope[3].Type());

#pragma warning(push)
#pragma warning(disable:26800)
			Assert::IsTrue(anotherScope.IsEmpty());
#pragma warning(pop)

			Scope& childScope = yetAnotherScope.AppendScope("child");
			childScope.Append("AChild") = aDatum;
			Scope& childChildScope = childScope.AppendScope("AScopeChild");
			Assert::AreEqual(std::size_t(5), yetAnotherScope.Size());
			Assert::AreEqual(Datum::DatumTypes::Table, yetAnotherScope[4].Type());
			Assert::AreEqual(childScope, yetAnotherScope[4].GetAsTable());

			Assert::IsNotNull(childScope.GetParent());
			Scope movedChildScope{ std::move(childScope) }; 
			Assert::IsNull(movedChildScope.GetParent());
			Assert::AreEqual(std::size_t(5), yetAnotherScope.Size());
			Assert::AreEqual(Datum::DatumTypes::Table, yetAnotherScope[4].Type());
			Assert::AreEqual(size_t(0), yetAnotherScope["child"].Size());
			Assert::AreEqual(std::size_t(2), movedChildScope.Size());
			Assert::AreEqual(aDatum, movedChildScope["AChild"]);
			Assert::AreEqual(childChildScope, movedChildScope["AScopeChild"].GetAsTable());

			Scope& anotherChildScope = yetAnotherScope.AppendScope("child");
			anotherChildScope.Append("AChild") = aDatum;
			Assert::AreEqual(std::size_t(5), yetAnotherScope.Size());
			Assert::AreEqual(Datum::DatumTypes::Table, yetAnotherScope[4].Type());
			Assert::AreEqual(anotherChildScope, yetAnotherScope[4].GetAsTable());

			Scope anotherMovedChildScope;
			anotherMovedChildScope.AppendScope("test");
			Assert::IsNotNull(anotherChildScope.GetParent());
			anotherMovedChildScope = std::move(anotherChildScope);
			Assert::IsNull(anotherMovedChildScope.GetParent());
			Assert::AreEqual(std::size_t(5), yetAnotherScope.Size());
			Assert::AreEqual(Datum::DatumTypes::Table, yetAnotherScope[4].Type());
			Assert::AreEqual(size_t(0), yetAnotherScope["child"].Size());
			Assert::AreEqual(std::size_t(1), anotherMovedChildScope.Size());
			Assert::AreEqual(aDatum, anotherMovedChildScope["AChild"]);

			Scope& anotherChildScope2 = yetAnotherScope.AppendScope("child");
			anotherChildScope2.Append("AChild") = aDatum;
			Assert::AreEqual(std::size_t(5), yetAnotherScope.Size());
			Assert::AreEqual(Datum::DatumTypes::Table, yetAnotherScope[4].Type());
			Assert::AreEqual(anotherChildScope2, yetAnotherScope[4].GetAsTable());

			Scope anotherMovedChildScope2;
			anotherMovedChildScope2.AppendScope("test");

			auto catchReturn = anotherChildScope2.Orphan();
			Assert::IsNull(anotherChildScope2.GetParent());
			anotherMovedChildScope2 = std::move(anotherChildScope2);
			Assert::IsNull(anotherMovedChildScope2.GetParent());
			Assert::AreEqual(std::size_t(5), yetAnotherScope.Size());
			Assert::AreEqual(Datum::DatumTypes::Table, yetAnotherScope[4].Type());
			Assert::AreEqual(size_t(0), yetAnotherScope["child"].Size());
			Assert::AreEqual(std::size_t(1), anotherMovedChildScope2.Size());
			Assert::AreEqual(aDatum, anotherMovedChildScope2["AChild"]);

			//prevent memory leak..
			delete catchReturn;
		}

		TEST_METHOD(Clone)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };

			scope.Append("A"s) = aDatum;
			scope.Append("B"s) = bDatum;
			scope.Append("C"s) = cDatum;
			Scope& childScope = scope.AppendScope("child");
			childScope.AppendScope("childChild");
			Assert::AreEqual(size_t(4), scope.Size());
			Assert::AreEqual(aDatum, scope["A"]);
			Assert::AreEqual(bDatum, scope["B"]);
			Assert::AreEqual(cDatum, scope["C"]);
			Assert::AreEqual(Datum::DatumTypes::Table, scope["child"].Type());
			Assert::AreNotSame(aDatum, scope["A"]);
			Assert::AreNotSame(bDatum, scope["B"]);
			Assert::AreNotSame(cDatum, scope["C"]);

			{
				Scope* copyOfScope = scope.Clone();
				Assert::AreNotSame(scope["A"], copyOfScope->operator[]("A"));
				Assert::AreNotSame(scope["B"], copyOfScope->operator[]("B"));
				Assert::AreNotSame(scope["C"], copyOfScope->operator[]("C"));

				Assert::AreEqual(size_t(4), copyOfScope->Size());
				Assert::AreEqual(aDatum, copyOfScope->operator[]("A"));
				Assert::AreEqual(bDatum, copyOfScope->operator[]("B"));
				Assert::AreEqual(cDatum, copyOfScope->operator[]("C"));
				Assert::AreEqual(Datum::DatumTypes::Table, copyOfScope->operator[]("child").Type());
				Assert::AreEqual(scope["A"], copyOfScope->operator[]("A"));
				Assert::AreEqual(scope["B"], copyOfScope->operator[]("B"));
				Assert::AreEqual(scope["C"], copyOfScope->operator[]("C"));

				copyOfScope->Append("test") = 12;
				copyOfScope->AppendScope("anotherChild");

				delete copyOfScope;
			}
			{
				Scope* copyOfScope = childScope.Clone();
				Assert::AreEqual(size_t(1), copyOfScope->Size());
				Assert::AreEqual(Datum::DatumTypes::Table, copyOfScope->operator[]("childChild").Type());
				Assert::IsNull(copyOfScope->GetParent());

				copyOfScope->Append("test") = 12;
				copyOfScope->AppendScope("anotherChild");

				delete copyOfScope;
			}
		}

		TEST_METHOD(Find)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };
			scope.Append("A"s) = aDatum;
			scope.Append("B"s) = bDatum;
			scope.Append("C"s) = cDatum;
			Scope& childScope = scope.AppendScope("child");
			Scope& childChildScope = childScope.AppendScope("childChild");
			childScope.Append("childA") = aDatum;
			childChildScope.Append("A"s) = cDatum;
			{
				Datum* foundDatum = scope.Find("A"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(scope[0], *foundDatum);
				Assert::AreSame(scope[0], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
			}
			{
				Datum* foundDatum = scope.Find(""s);
				Assert::IsNull(foundDatum);
			}
			{
				Datum* foundDatum = childScope.Find("childA"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(childScope[size_t(1)], *foundDatum);
				Assert::AreSame(childScope[size_t(1)], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
			}
			{
				Datum* foundDatum = childScope.Find("A"s);
				Assert::IsNull(foundDatum);
			}

			const Scope constScope = scope;
			Scope& constChildScope = scope["child"].GetAsTable();
			{
				const Datum* foundDatum = constScope.Find("A"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(constScope[0], *foundDatum);
				Assert::AreSame(constScope[0], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
			}
			{
				const Datum* foundDatum = constScope.Find(""s);
				Assert::IsNull(foundDatum);
			}
			{
				const Datum* foundDatum = constChildScope.Find("childA"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(constChildScope[size_t(1)], *foundDatum);
				Assert::AreSame(constChildScope[size_t(1)], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
			}
			{
				const Datum* foundDatum = constChildScope.Find("A"s);
				Assert::IsNull(foundDatum);
			}
		}

		TEST_METHOD(Search)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };
			scope.Append("A"s) = aDatum;
			scope.Append("B"s) = bDatum;
			scope.Append("C"s) = cDatum;
			Scope& childScope = scope.AppendScope("child");
			Scope& childChildScope = childScope.AppendScope("childChild");
			childScope.Append("childA") = aDatum;
			childChildScope.Append("A"s) = cDatum;
			{
				Datum* foundDatum = scope.Search("A"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(scope[0], *foundDatum);
				Assert::AreSame(scope[0], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
			}
			{
				Datum* foundDatum = scope.Search(""s);
				Assert::IsNull(foundDatum);
			}
			{
				Datum* foundDatum = childScope.Search("childA"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(childScope[size_t(1)], *foundDatum);
				Assert::AreSame(childScope[size_t(1)], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
			}
			{
				Datum* foundDatum = childScope.Search("A"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(scope[0], *foundDatum);
				Assert::AreSame(scope[0], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
			}
			{
				Datum* foundDatum = childChildScope.Search("A"s);
				Assert::IsNotNull(foundDatum);
				Assert::AreNotEqual(scope[0], *foundDatum);
				Assert::AreNotSame(scope[0], *foundDatum);
				Assert::AreEqual(childChildScope[0], *foundDatum);
				Assert::AreSame(childChildScope[0], *foundDatum);
				Assert::AreEqual(cDatum, *foundDatum);
			}
			{
				Datum* foundDatum = childScope.Search("Bleh"s);
				Assert::IsNull(foundDatum);
			}
		}

		TEST_METHOD(SearchWithOutputParam)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };
			scope.Append("A"s) = aDatum;
			scope.Append("B"s) = bDatum;
			scope.Append("C"s) = cDatum;
			Scope& childScope = scope.AppendScope("child");
			Scope& childChildScope = childScope.AppendScope("childChild");
			childScope.Append("childA") = aDatum;
			childChildScope.Append("A"s) = cDatum;
			{
				Scope* containingScope = nullptr;
				Datum* foundDatum = scope.Search("A"s, containingScope);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(scope[0], *foundDatum);
				Assert::AreSame(scope[0], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
				Assert::IsNotNull(containingScope);
				Assert::AreEqual(*containingScope, scope);
			}
			{
				Scope* containingScope = nullptr;
				Datum* foundDatum = scope.Search(""s, containingScope);
				Assert::IsNull(foundDatum);
				Assert::IsNull(containingScope);
			}
			{
				Scope* containingScope = nullptr;
				Datum* foundDatum = childScope.Search("childA"s, containingScope);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(childScope[size_t(1)], *foundDatum);
				Assert::AreSame(childScope[size_t(1)], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
				Assert::IsNotNull(containingScope);
				Assert::AreEqual(*containingScope, childScope);
			}
			{
				Scope* containingScope = nullptr;
				Datum* foundDatum = childScope.Search("A"s, containingScope);
				Assert::IsNotNull(foundDatum);
				Assert::AreEqual(scope[0], *foundDatum);
				Assert::AreSame(scope[0], *foundDatum);
				Assert::AreEqual(aDatum, *foundDatum);
				Assert::IsNotNull(containingScope);
				Assert::AreEqual(*containingScope, scope);
			}
			{
				Scope* containingScope = nullptr;
				Datum* foundDatum = childChildScope.Search("A"s, containingScope);
				Assert::IsNotNull(foundDatum);
				Assert::AreNotEqual(scope[0], *foundDatum);
				Assert::AreNotSame(scope[0], *foundDatum);
				Assert::AreEqual(childChildScope[0], *foundDatum);
				Assert::AreSame(childChildScope[0], *foundDatum);
				Assert::AreEqual(cDatum, *foundDatum);
				Assert::IsNotNull(containingScope);
				Assert::AreEqual(*containingScope, childChildScope);
			}
		}

		TEST_METHOD(Size)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };

			Assert::AreEqual(size_t(0), scope.Size());
			scope.Append("A"s) = aDatum;

			Assert::AreEqual(size_t(1), scope.Size());
			scope.Append("B"s) = bDatum;

			Assert::AreEqual(size_t(2), scope.Size());
			scope.Append("C"s) = cDatum;

			Assert::AreEqual(size_t(3), scope.Size());
			Scope& childScope = scope.AppendScope("child");

			Assert::AreEqual(size_t(4), scope.Size());
			Assert::AreEqual(size_t(0), childScope.Size());
			childScope.AppendScope("childChild");
			Assert::AreEqual(size_t(4), scope.Size());
			Assert::AreEqual(size_t(1), childScope.Size());

			scope.Append("A"s).PushBack(2);
			Assert::AreEqual(size_t(4), scope.Size());

			scope.Append("A"s) = bDatum;
			Assert::AreEqual(size_t(4), scope.Size());

			scope.Append("D");
			Assert::AreEqual(size_t(5), scope.Size());

			auto catchOrphanedScope = childScope.Orphan();
			Assert::AreEqual(size_t(5), scope.Size());
			Assert::AreEqual(size_t(1), childScope.Size());

			//prevent memory leak..
			delete catchOrphanedScope;
		}

		TEST_METHOD(IsEmpty)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };

			Assert::IsTrue(scope.IsEmpty());
			scope.Append("A"s) = aDatum;

			Assert::IsFalse(scope.IsEmpty());
			scope.Append("B"s) = bDatum;

			Assert::IsFalse(scope.IsEmpty());
			scope.Append("C"s) = cDatum;

			Assert::IsFalse(scope.IsEmpty());
			Scope& childScope = scope.AppendScope("child");

			Assert::IsFalse(scope.IsEmpty());
			Assert::IsTrue(childScope.IsEmpty());
			childScope.AppendScope("childChild");
			Assert::IsFalse(scope.IsEmpty());
			Assert::IsFalse(childScope.IsEmpty());

			scope.Append("A"s).PushBack(2);
			Assert::IsFalse(scope.IsEmpty());

			scope.Append("A"s) = bDatum;
			Assert::IsFalse(scope.IsEmpty());

			scope.Append("D");
			Assert::IsFalse(scope.IsEmpty());

			auto catchOrphanedScope = childScope.Orphan();
			Assert::IsFalse(scope.IsEmpty());
			Assert::IsFalse(childScope.IsEmpty());

			//prevent memory leak..
			delete catchOrphanedScope;
		}

		TEST_METHOD(Clear)
		{
			Scope scope;
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };

			Assert::IsTrue(scope.IsEmpty());
			scope.Append("A"s) = aDatum;
			scope.Append("B"s) = aDatum;
			Assert::AreEqual(size_t(2), scope.Size());
			scope.Clear();
			Assert::IsTrue(scope.IsEmpty());

			scope.Append("B"s) = bDatum;
			scope.Append("C"s) = cDatum;
			Scope& childScope = scope.AppendScope("child");
			Assert::AreEqual(size_t(3), scope.Size());
			Assert::AreEqual(size_t(0), childScope.Size());
			scope.Clear();
			Assert::IsTrue(scope.IsEmpty());

			Scope& anotherChildScope = scope.AppendScope("child");
			anotherChildScope.Append("A") = aDatum;
			anotherChildScope.AppendScope("child");
			Assert::AreEqual(size_t(1), scope.Size());
			Assert::AreEqual(size_t(2), anotherChildScope.Size());
			anotherChildScope.Clear();
			Assert::IsTrue(anotherChildScope.IsEmpty());
			Assert::AreEqual(size_t(1), scope.Size());
			scope.Clear();
			Assert::IsTrue(scope.IsEmpty());
		}

		TEST_METHOD(Append)
		{
			Scope scope;
			const std::string a = "NameA"s;
			const std::string b = "NameB"s;
			const std::string aAgain = "NameA"s;

			Datum& aDatum = scope.Append(a);
			Assert::IsNull(scope.GetParent());
			Assert::AreEqual(size_t(1), scope.Size());
			Assert::AreEqual(&aDatum, scope.Find(a));
			Assert::AreSame(aDatum, scope[0]);
			Assert::AreSame(aDatum, scope[a]);
			Assert::AreEqual(Datum::DatumTypes::Unknown, aDatum.Type());
			Assert::AreEqual(size_t(0), aDatum.Size());
			Assert::AreEqual(size_t(0), aDatum.Capacity());

			Datum& bDatum = scope.Append(b);
			Assert::IsNull(scope.GetParent());
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(&bDatum, scope.Find(b));
			Assert::AreNotEqual(&bDatum, scope.Find(a));
			Assert::AreSame(aDatum, scope[0]);
			Assert::AreSame(bDatum, scope[1]);
			Assert::AreSame(bDatum, scope[b]);
			Assert::AreEqual(Datum::DatumTypes::Unknown, bDatum.Type());
			Assert::AreEqual(size_t(0), bDatum.Size());
			Assert::AreEqual(size_t(0), bDatum.Capacity());

			Datum& cDatum = scope.Append(aAgain);
			Assert::IsNull(scope.GetParent());
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(&cDatum, scope.Find(a));
			Assert::AreEqual(&cDatum, scope.Find(aAgain));
			Assert::AreSame(cDatum, scope[0]);
			Assert::AreSame(bDatum, scope[b]);
			Assert::AreSame(cDatum, scope[a]);
			Assert::AreSame(cDatum, scope[aAgain]);
			Assert::AreEqual(Datum::DatumTypes::Unknown, aDatum.Type());
			Assert::AreEqual(size_t(0), aDatum.Size());
			Assert::AreEqual(size_t(0), aDatum.Capacity());

			aDatum.SetType(Datum::DatumTypes::Integer);
			aDatum.Reserve(size_t(5));

			cDatum = scope.Append(aAgain);
			Assert::IsNull(scope.GetParent());
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(&cDatum, scope.Find(a));
			Assert::AreEqual(&cDatum, scope.Find(aAgain));
			Assert::AreSame(cDatum, scope[0]);
			Assert::AreSame(cDatum, scope[a]);
			Assert::AreSame(cDatum, scope[aAgain]);
			Assert::AreEqual(Datum::DatumTypes::Integer, aDatum.Type());
			Assert::AreEqual(size_t(0), aDatum.Size());
			Assert::AreEqual(size_t(5), aDatum.Capacity());

			scope.Append(aAgain) = 10;
			Assert::IsNull(scope.GetParent());
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(&cDatum, scope.Find(a));
			Assert::AreEqual(&cDatum, scope.Find(aAgain));
			Assert::AreSame(cDatum, scope[0]);
			Assert::AreSame(cDatum, scope[a]);
			Assert::AreSame(cDatum, scope[aAgain]);
			Assert::AreEqual(Datum::DatumTypes::Integer, aDatum.Type());
			Assert::AreEqual(size_t(1), aDatum.Size());
			Assert::AreEqual(size_t(1), aDatum.Capacity());

			Assert::ExpectException<runtime_error>([&scope] {scope.Append(""); });
			Assert::ExpectException<runtime_error>([&scope] {auto var = scope[2]; UNREFERENCED_LOCAL(var); });
		}

		TEST_METHOD(AppendScope)
		{
			Scope scope;
			const std::string a = "NameA"s;
			const std::string b = "NameB"s;
			const std::string aAgain = "NameA"s;

			Scope& aScope = scope.AppendScope(a);
			Assert::AreEqual(size_t(0), aScope.Size());
			Assert::AreEqual(size_t(1), scope.Size());
			Assert::AreEqual(&scope, aScope.GetParent());

			Datum* aScopeDatum = scope.Find(a);
			Assert::IsNotNull(aScopeDatum);
			Assert::AreEqual(Datum::DatumTypes::Table, aScopeDatum->Type());
			Assert::AreSame(aScope, aScopeDatum->GetAsTable());
			Assert::AreEqual(aScopeDatum, &(scope[0]));

			Scope& bScope = scope.AppendScope(b);
			Assert::AreEqual(size_t(0), bScope.Size());
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(&scope, bScope.GetParent());

			Datum* bScopeDatum = scope.Find(b);
			Assert::IsNotNull(bScopeDatum);
			Assert::AreEqual(Datum::DatumTypes::Table, bScopeDatum->Type());
			Assert::AreSame(bScope, bScopeDatum->GetAsTable());
			Assert::AreEqual(bScopeDatum, &(scope[1]));

			Scope& cScope = scope.AppendScope(aAgain);
			Assert::AreEqual(size_t(0), cScope.Size());
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(&scope, cScope.GetParent());

			Datum* cScopeDatum = scope.Find(aAgain);
			Assert::IsNotNull(cScopeDatum);
			Assert::AreEqual(Datum::DatumTypes::Table, cScopeDatum->Type());
			Assert::AreSame(cScope, cScopeDatum->GetAsTable(size_t(1)));
			Assert::AreNotSame(cScope, cScopeDatum->GetAsTable());
			Assert::AreEqual(cScopeDatum, &(scope[0]));
			Assert::AreEqual(cScopeDatum, aScopeDatum); 

			Scope& childChildScope = cScope.AppendScope(aAgain);
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(size_t(1), cScope.Size());
			Assert::AreEqual(size_t(0), childChildScope.Size());
			Assert::AreEqual(&scope, cScope.GetParent());
			Assert::AreEqual(&cScope, childChildScope.GetParent());

			Datum* childScopeDatum = cScope.Find(aAgain);
			Assert::IsNotNull(childScopeDatum);
			Assert::AreEqual(Datum::DatumTypes::Table, childScopeDatum->Type());
			Assert::AreSame(childChildScope, childScopeDatum->GetAsTable());
			Assert::AreEqual(childScopeDatum, &(cScope[0]));

			scope.Append("AnotherName"s) = "owo";
			Assert::ExpectException<runtime_error>([&scope] {scope.AppendScope("AnotherName"s); });
			Assert::ExpectException<runtime_error>([&scope] {scope.AppendScope(""); });
		}

		TEST_METHOD(Adopt)
		{
			Scope scope;
			Scope& childScope = scope.AppendScope("child");
			childScope.Append("A") = 1;
			childScope.AppendScope("grandchild");
			Assert::AreEqual(size_t(1), scope.Size());
			Assert::AreEqual(size_t(1), scope[0].Size());
			Assert::AreEqual(size_t(2), childScope.Size());
			Assert::IsNotNull(childScope.GetParent());

			auto catchOrphan = childScope.Orphan();
			Assert::IsNull(childScope.GetParent());
			Assert::AreEqual(size_t(1), scope.Size());
			Assert::AreEqual(size_t(0), scope[0].Size());
			Assert::AreEqual(size_t(2), childScope.Size());

			Scope anotherScope;
			anotherScope.Adopt(*catchOrphan, "newChild");
			Assert::IsNotNull(childScope.GetParent());
			Assert::AreEqual(size_t(1), scope.Size());
			Assert::AreEqual(size_t(0), scope[0].Size());
			Assert::AreEqual(size_t(2), childScope.Size());
			Assert::AreEqual(size_t(1), anotherScope.Size());
			Assert::AreEqual(size_t(1), anotherScope[0].Size());

			Scope* anotherChildScope = new Scope;
			anotherChildScope->Append("A") = 1;
			anotherChildScope->AppendScope("grandchild");
			Assert::AreEqual(size_t(1), scope.Size());
			Assert::AreEqual(size_t(0), scope[0].Size());
			Assert::AreEqual(size_t(2), anotherChildScope->Size());
			Assert::IsNull(anotherChildScope->GetParent());

			scope.Adopt(*anotherChildScope, "newChild");
			Assert::IsNotNull(anotherChildScope->GetParent());
			Assert::AreEqual(size_t(2), anotherChildScope->Size());
			Assert::AreEqual(size_t(2), scope.Size());
			Assert::AreEqual(size_t(0), scope[0].Size());
			Assert::AreEqual(size_t(1), scope[1].Size());

			Assert::ExpectException<runtime_error>([&scope, &anotherChildScope] {anotherChildScope->Adopt(scope, "adoptAncestor"); });
			Assert::ExpectException<runtime_error>([&scope, &anotherChildScope] {scope.Adopt(scope, "adoptSelf"); });

			scope.Append("int") = 1;
			Assert::ExpectException<runtime_error>([&scope, &anotherChildScope] {scope.Adopt(*anotherChildScope, "int"); });
			Assert::ExpectException<runtime_error>([&scope, &anotherChildScope] {scope.Adopt(*anotherChildScope, ""); });
		}

		TEST_METHOD(Orphan)
		{
			{
				Scope scope;
				Scope& aScope = scope.AppendScope("A"s);
				Assert::AreEqual(&scope, aScope.GetParent());

				auto catchReturn = aScope.Orphan();
				Assert::IsNull(aScope.GetParent());

				//prevent memory leak..
				delete catchReturn;
			}
			{
				Scope scope;
				Scope& aScope = scope.AppendScope("A"s);
				Scope& aChildScope = aScope.AppendScope("AChild"s);
				Assert::AreEqual(&scope, aScope.GetParent());
				Assert::AreEqual(&aScope, aChildScope.GetParent());

				auto catchReturn = aScope.Orphan();
				Assert::IsNull(aScope.GetParent());
				Assert::AreEqual(&aScope, aChildScope.GetParent());

				//prevent memory leak..
				delete catchReturn;
			}
			{
				Scope scope;
				Assert::IsNull(scope.GetParent());

				auto catchReturn = scope.Orphan();
				Assert::IsNull(scope.GetParent());

				UNREFERENCED_LOCAL(catchReturn);
			}
		}

		TEST_METHOD(GetParent)
		{
			{
				Scope scope;
				Scope& aScope = scope.AppendScope("A"s);
				Assert::IsNull(scope.GetParent());
				Assert::AreEqual(&scope, aScope.GetParent());

				auto catchReturn = aScope.Orphan();
				Assert::IsNull(scope.GetParent());
				Assert::IsNull(aScope.GetParent());

				//prevent memory leak..
				delete catchReturn;
			}
			{
				Scope scope;
				Scope& aScope = scope.AppendScope("A"s);
				Scope& aChildScope = aScope.AppendScope("AChild"s);
				Assert::IsNull(scope.GetParent());
				Assert::AreEqual(&scope, aScope.GetParent());
				Assert::AreEqual(&aScope, aChildScope.GetParent());

				auto catchReturn = aScope.Orphan();
				Assert::IsNull(scope.GetParent());
				Assert::IsNull(aScope.GetParent());
				Assert::AreEqual(&aScope, aChildScope.GetParent());

				//prevent memory leak..
				delete catchReturn;
			}
		}

		TEST_METHOD(AncestorAndDescendentTesting)
		{
			Scope scope;
			Scope& aScope = scope.AppendScope("A"s);
			Scope& bScope = aScope.AppendScope("B"s);

			Assert::IsTrue(scope.IsAncestorOf(aScope));
			Assert::IsTrue(scope.IsAncestorOf(bScope));
			Assert::IsFalse(aScope.IsAncestorOf(scope));
			Assert::IsTrue(aScope.IsAncestorOf(bScope));
			Assert::IsFalse(bScope.IsAncestorOf(scope));
			Assert::IsFalse(bScope.IsAncestorOf(aScope));

			Assert::IsFalse(scope.IsDescendantOf(aScope));
			Assert::IsFalse(scope.IsDescendantOf(bScope));
			Assert::IsTrue(aScope.IsDescendantOf(scope));
			Assert::IsFalse(aScope.IsDescendantOf(bScope));
			Assert::IsTrue(bScope.IsDescendantOf(scope));
			Assert::IsTrue(bScope.IsDescendantOf(aScope));

			auto catchReturn = aScope.Orphan();

			Assert::IsFalse(scope.IsAncestorOf(aScope));
			Assert::IsFalse(scope.IsAncestorOf(bScope));
			Assert::IsFalse(aScope.IsAncestorOf(scope));
			Assert::IsTrue(aScope.IsAncestorOf(bScope));
			Assert::IsFalse(bScope.IsAncestorOf(scope));
			Assert::IsFalse(bScope.IsAncestorOf(aScope));

			Assert::IsFalse(scope.IsDescendantOf(aScope));
			Assert::IsFalse(scope.IsDescendantOf(bScope));
			Assert::IsFalse(aScope.IsDescendantOf(scope));
			Assert::IsFalse(aScope.IsDescendantOf(bScope));
			Assert::IsFalse(bScope.IsDescendantOf(scope));
			Assert::IsTrue(bScope.IsDescendantOf(aScope));

			//prevent memory leak..
			delete catchReturn;
		}

		TEST_METHOD(At)
		{
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };
			{
				Scope scope;
				scope.Append("A"s) = aDatum;
				scope.Append("B"s) = bDatum;
				scope.Append("C"s) = cDatum;
				Scope& childScope = scope.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope.Append("childA") = aDatum;
				childChildScope.Append("A"s) = cDatum;

				Assert::AreEqual(aDatum, scope.At("A"));
				Assert::AreEqual(bDatum, scope.At("B"));
				Assert::AreEqual(cDatum, scope.At("C"));
				Assert::AreEqual(childScope, scope.At("child").GetAsTable());
				Assert::AreEqual(childChildScope, scope.At("child").GetAsTable().At("childChild").GetAsTable());
				Assert::AreEqual(aDatum, childScope.At("childA"));
				Assert::AreEqual(cDatum, childChildScope.At("A"));

				Assert::ExpectException<std::runtime_error>([&scope] {auto& val = scope.At("nothing"); UNREFERENCED_LOCAL(val); });
			}
			{
				Scope scopeProto;
				scopeProto.Append("A"s) = aDatum;
				scopeProto.Append("B"s) = bDatum;
				scopeProto.Append("C"s) = cDatum;
				Scope& childScope = scopeProto.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope.Append("childA") = aDatum;
				childChildScope.Append("A"s) = cDatum;

				const Scope scope = scopeProto;

				Assert::AreEqual(aDatum, scope.At("A"));
				Assert::AreEqual(bDatum, scope.At("B"));
				Assert::AreEqual(cDatum, scope.At("C"));
				Assert::AreEqual(childScope, scope.At("child").GetAsTable());
				Assert::AreEqual(childChildScope, scope.At("child").GetAsTable().At("childChild").GetAsTable());
				Assert::AreEqual(aDatum, childScope.At("childA"));
				Assert::AreEqual(cDatum, childChildScope.At("A"));

				Assert::ExpectException<std::runtime_error>([&scope] {auto& val = scope.At("nothing"); UNREFERENCED_LOCAL(val); });
			}
		}
		TEST_METHOD(BracketOperatorString)
		{
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };
			{
				Scope scope;
				scope.Append("A"s) = aDatum;
				scope.Append("B"s) = bDatum;
				scope.Append("C"s) = cDatum;
				Scope& childScope = scope.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope.Append("childA") = aDatum;
				childChildScope.Append("A"s) = cDatum;

				Assert::AreEqual(aDatum, scope["A"]);
				Assert::AreEqual(bDatum, scope["B"]);
				Assert::AreEqual(cDatum, scope["C"]);
				Assert::AreEqual(childScope, scope["child"].GetAsTable());
				Assert::AreEqual(childChildScope, scope["child"].GetAsTable()["childChild"].GetAsTable());
				Assert::AreEqual(aDatum, childScope["childA"]);
				Assert::AreEqual(cDatum, childChildScope["A"]);
			}
			{
				Scope scope;
				scope["A"s] = aDatum;
				scope["B"s] = bDatum;
				scope["C"s] = cDatum;
				Datum& emptyDatum = scope["D"];
				UNREFERENCED_LOCAL(emptyDatum);
				Scope& childScope = scope.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope["childA"] = aDatum;
				childChildScope["A"s] = cDatum;

				Assert::AreEqual(aDatum, scope["A"]);
				Assert::AreEqual(bDatum, scope["B"]);
				Assert::AreEqual(cDatum, scope["C"]);
				Assert::AreEqual(Datum(), scope["D"]);
				Assert::AreEqual(childScope, scope["child"].GetAsTable());
				Assert::AreEqual(childChildScope, scope["child"].GetAsTable()["childChild"].GetAsTable());
				Assert::AreEqual(aDatum, childScope["childA"]);
				Assert::AreEqual(cDatum, childChildScope["A"]);

				Assert::ExpectException<runtime_error>([&scope] {auto& value = scope[""]; UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(BracketOperatorIndex)
		{
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };
			{
				Scope scope;
				scope.Append("A"s) = aDatum;
				scope.Append("B"s) = bDatum;
				scope.Append("C"s) = cDatum;
				Scope& childScope = scope.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope.Append("childA") = aDatum;
				childChildScope.Append("A"s) = cDatum;

				Assert::AreEqual(aDatum, scope[0]);
				Assert::AreEqual(bDatum, scope[1]);
				Assert::AreEqual(cDatum, scope[2]);
				Assert::AreEqual(childScope, scope[3].GetAsTable());
				Assert::AreEqual(childChildScope, scope[3].GetAsTable()[0].GetAsTable());
				Assert::AreEqual(aDatum, childScope[1]);
				Assert::AreEqual(cDatum, childChildScope[0]);

				Assert::ExpectException<runtime_error>([&scope] {auto& value = scope[5]; UNREFERENCED_LOCAL(value); });
			}
			{
				Scope scope;
				scope.Append("A"s) = aDatum;
				scope.Append("B"s) = bDatum;
				scope.Append("C"s) = cDatum;
				Scope& childScope = scope.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope.Append("childA") = aDatum;
				childChildScope.Append("A"s) = cDatum;

				const Scope scopeCopy = scope;

				Assert::AreEqual(aDatum, scopeCopy[0]);
				Assert::AreEqual(bDatum, scopeCopy[1]);
				Assert::AreEqual(cDatum, scopeCopy[2]);
				Assert::AreEqual(childScope, scopeCopy[3].GetAsTable());
				Assert::AreEqual(childChildScope, scopeCopy[3].GetAsTable()[0].GetAsTable());
				Assert::AreEqual(aDatum, scopeCopy[3].GetAsTable()[1]);
				Assert::AreEqual(cDatum, scopeCopy[3].GetAsTable()[0].GetAsTable()[0]);

				Assert::ExpectException<runtime_error>([&scopeCopy] {auto& value = scopeCopy[5]; UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(EqualityTesting)
		{
			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = { 5.5f, 7.0f };

			{
				Scope scope;
				scope.Append("A"s) = aDatum;
				scope.Append("B"s) = bDatum;
				scope.Append("C"s) = cDatum;
				Scope& childScope = scope.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope.Append("childA") = aDatum;
				childChildScope.Append("A"s) = cDatum;

				Scope scope2 = scope;
				Assert::AreEqual(scope, scope2);
				Assert::IsFalse(scope != scope2);
			}
			{
				Scope scope;
				scope.Append("A"s) = aDatum;
				scope.Append("B"s) = bDatum;
				scope.Append("C"s) = cDatum;
				Scope& childScope = scope.AppendScope("child");
				Scope& childChildScope = childScope.AppendScope("childChild");
				childScope.Append("childA") = aDatum;
				childChildScope.Append("A"s) = cDatum;

				Scope scope2;
				scope2.Append("C"s) = cDatum;
				Scope& childScope2 = scope2.AppendScope("child");
				scope2.Append("B"s) = bDatum;
				scope2.Append("A"s) = aDatum;
				childScope2.Append("childA") = aDatum;
				Scope& childChildScope2 = childScope2.AppendScope("childChild");
				childChildScope2.Append("A"s) = cDatum;

				Assert::AreEqual(scope, scope2);
				Assert::IsFalse(scope != scope2);
			}
			{
				Scope scope1;
				Scope scope2;

				Assert::AreEqual(scope1, scope2);
				Assert::IsFalse(scope1 != scope2);
			}
			{
				Scope scope1;
				scope1.Append("A") = 1;
				Scope scope2;
				scope2.Append("B") = 1;

				Assert::AreNotEqual(scope1, scope2);
				Assert::IsTrue(scope1 != scope2);
			}
			{
				Scope scope1;
				Scope scope2;
				Scope& scope3 = scope1.AppendScope("child");

				Assert::AreEqual(scope3, scope2);
				Assert::IsFalse(scope3 != scope2);
				Assert::AreNotEqual(scope1, scope2);
				Assert::IsTrue(scope1 != scope2);
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}