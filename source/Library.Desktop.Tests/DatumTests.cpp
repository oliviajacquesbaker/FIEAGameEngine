/// <summary>
/// Unit tests exercising the functionality of Datum in all its possible type states.
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Datum.h"
#include "Scope.h"
#include "Foo.h"

#pragma warning(push)
#pragma warning(disable:4201)
#include "glm/gtx/string_cast.hpp"
#pragma warning(pop)

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace glm;
using namespace std::string_literals;

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
	std::wstring ToString<RTTI>(RTTI* t) {
		std::string stringResult = t->ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<RTTI*>(RTTI** t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<string>(string* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<vec4>(const vec4& t) {
		std::string stringResult = glm::to_string(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<vec4>(vec4* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<mat4>(const mat4& t) {
		std::string stringResult = glm::to_string(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<mat4>(mat4* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<Scope>(const Scope& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace LibraryDesktopTests
{
	TEST_CLASS(DatumTests)
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
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());
			}
			{
				Datum datum(Datum::DatumTypes::Integer);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
			}
			{
				Datum datum(Datum::DatumTypes::Pointer, size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
			}
			{
				Datum datum(Datum::DatumTypes::String, size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
			}
			{
				Assert::ExpectException<runtime_error>([] {Datum datum(Datum::DatumTypes::Unknown); });
				Assert::ExpectException<runtime_error>([] {Datum datum(Datum::DatumTypes::Unknown, size_t(10)); });
			}
		}

		TEST_METHOD(CopySemantics)
		{			
			{
				const int a = 10;
				const int b = 20;
				const int c = 30;

				Datum datum(Datum::DatumTypes::Integer, size_t(3));
				datum.PushBack(a);
				datum.PushBack(b);
				datum.PushBack(c);
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(a, datum.GetAsInt(size_t(0)));
				Assert::AreEqual(b, datum.GetAsInt(size_t(1)));
				Assert::AreEqual(c, datum.GetAsInt(size_t(2)));

				int e = 13;
				Datum externalDatum;
				externalDatum.SetStorage(&e, size_t(1));
				{
					Datum anotherDatum{ externalDatum };
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Integer, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsInt(size_t(0)));
				}
				{
					Datum anotherDatum = 0;
					anotherDatum = externalDatum;
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Integer, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsInt(size_t(0)));
				}
				{
					Datum anotherDatum{ datum };
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Integer, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsInt(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsInt(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsInt(size_t(2)));
				}
				{
					Datum anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Integer, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsInt(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsInt(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsInt(size_t(2)));
				}
				{
					Datum anotherDatum;
					anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Integer, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsInt(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsInt(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsInt(size_t(2)));
				}
			}
			{
				const float a = 1.0f;
				const float b = 2.0f;
				const float c = 3.0f;

				Datum datum(Datum::DatumTypes::Float, size_t(3));
				datum.PushBack(a);
				datum.PushBack(b);
				datum.PushBack(c);
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(a, datum.GetAsFloat(size_t(0)));
				Assert::AreEqual(b, datum.GetAsFloat(size_t(1)));
				Assert::AreEqual(c, datum.GetAsFloat(size_t(2)));

				float e = 1.3f;
				Datum externalDatum;
				externalDatum.SetStorage(&e, size_t(1));
				{
					Datum anotherDatum{ externalDatum };
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Float, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsFloat(size_t(0)));
				}
				{
					Datum anotherDatum = 0.9f;
					anotherDatum = externalDatum;
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Float, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsFloat(size_t(0)));
				}
				{
					Datum anotherDatum{ datum };
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Float, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsFloat(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsFloat(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsFloat(size_t(2)));
				}
				{
					Datum anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Float, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsFloat(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsFloat(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsFloat(size_t(2)));
				}
				{
					Datum anotherDatum;
					anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Float, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsFloat(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsFloat(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsFloat(size_t(2)));
				}
			}
			{
				const string a = "owo";
				const string b = "uwu";
				const string c = "umu";

				Datum datum(Datum::DatumTypes::String, size_t(3));
				datum.PushBack(a);
				datum.PushBack(b);
				datum.PushBack(c);
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(a, datum.GetAsString(size_t(0)));
				Assert::AreEqual(b, datum.GetAsString(size_t(1)));
				Assert::AreEqual(c, datum.GetAsString(size_t(2)));

				string e = "13";
				Datum externalDatum;
				externalDatum.SetStorage(&e, size_t(1));
				{
					Datum anotherDatum{ externalDatum };
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::String, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsString(size_t(0)));
				}
				{
					Datum anotherDatum = "test"s;
					anotherDatum = externalDatum;
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::String, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsString(size_t(0)));
				}
				{
					Datum anotherDatum{ datum };
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::String, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsString(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsString(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsString(size_t(2)));
				}
				{
					Datum anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::String, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsString(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsString(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsString(size_t(2)));
				}
				{
					Datum anotherDatum;
					anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::String, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsString(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsString(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsString(size_t(2)));
				}
			}
			{
				const vec4 a{ 10 };
				const vec4 b{ 20 };
				const vec4 c{ 30 };

				Datum datum(Datum::DatumTypes::Vector, size_t(3));
				datum.PushBack(a);
				datum.PushBack(b);
				datum.PushBack(c);
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(a, datum.GetAsVector(size_t(0)));
				Assert::AreEqual(b, datum.GetAsVector(size_t(1)));
				Assert::AreEqual(c, datum.GetAsVector(size_t(2)));

				vec4 e(13);
				Datum externalDatum;
				externalDatum.SetStorage(&e, size_t(1));
				{
					Datum anotherDatum{ externalDatum };
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Vector, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsVector(size_t(0)));
				}
				{
					Datum anotherDatum = vec4(9);
					anotherDatum = externalDatum;
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Vector, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsVector(size_t(0)));
				}
				{
					Datum anotherDatum{ datum };
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Vector, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsVector(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsVector(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsVector(size_t(2)));
				}
				{
					Datum anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Vector, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsVector(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsVector(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsVector(size_t(2)));
				}
				{
					Datum anotherDatum;
					anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Vector, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsVector(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsVector(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsVector(size_t(2)));
				}
			}
			{
				const mat4 a{ 10 };
				const mat4 b{ 20 };
				const mat4 c{ 30 };

				Datum datum(Datum::DatumTypes::Matrix, size_t(3));
				datum.PushBack(a);
				datum.PushBack(b);
				datum.PushBack(c);
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(a, datum.GetAsMatrix(size_t(0)));
				Assert::AreEqual(b, datum.GetAsMatrix(size_t(1)));
				Assert::AreEqual(c, datum.GetAsMatrix(size_t(2)));

				mat4 e(13);
				Datum externalDatum;
				externalDatum.SetStorage(&e, size_t(1));
				{
					Datum anotherDatum{ externalDatum };
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Matrix, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsMatrix(size_t(0)));
				}
				{
					Datum anotherDatum = mat4(9);
					anotherDatum = externalDatum;
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Matrix, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsMatrix(size_t(0)));
				}
				{
					Datum anotherDatum{ datum };
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Matrix, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsMatrix(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsMatrix(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsMatrix(size_t(2)));
				}
				{
					Datum anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Matrix, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsMatrix(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsMatrix(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsMatrix(size_t(2)));
				}
				{
					Datum anotherDatum;
					anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Matrix, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsMatrix(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsMatrix(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsMatrix(size_t(2)));
				}
			}
			{
				Foo aFoo{ 10 };
				Foo bFoo{ 20 };
				Foo cFoo{ 30 };
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;

				Datum datum(Datum::DatumTypes::Pointer, size_t(3));
				datum.PushBack(a);
				datum.PushBack(b);
				datum.PushBack(c);
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(a, datum.GetAsPointer(size_t(0)));
				Assert::AreEqual(b, datum.GetAsPointer(size_t(1)));
				Assert::AreEqual(c, datum.GetAsPointer(size_t(2)));

				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				Datum externalDatum;
				externalDatum.SetStorage(&e, size_t(1));
				{
					Datum anotherDatum{ externalDatum };
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Pointer, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsPointer(size_t(0)));
				}
				{
					Foo fFoo{ 15 };
					RTTI* f = &fFoo;
					Datum anotherDatum = f;
					anotherDatum = externalDatum;
					Assert::IsTrue(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Pointer, anotherDatum.Type());
					Assert::AreEqual(externalDatum.Size(), anotherDatum.Size());
					Assert::AreEqual(externalDatum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(e, anotherDatum.GetAsPointer(size_t(0)));
				}
				{
					Datum anotherDatum{ datum };
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Pointer, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsPointer(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsPointer(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsPointer(size_t(2)));
				}
				{
					Datum anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Pointer, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsPointer(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsPointer(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsPointer(size_t(2)));
				}
				{
					Datum anotherDatum;
					anotherDatum = datum;
					Assert::IsFalse(anotherDatum.IsExternal());
					Assert::AreEqual(Datum::DatumTypes::Pointer, anotherDatum.Type());
					Assert::AreEqual(datum.Size(), anotherDatum.Size());
					Assert::AreEqual(datum.Capacity(), anotherDatum.Capacity());
					Assert::AreEqual(a, anotherDatum.GetAsPointer(size_t(0)));
					Assert::AreEqual(b, anotherDatum.GetAsPointer(size_t(1)));
					Assert::AreEqual(c, anotherDatum.GetAsPointer(size_t(2)));
				}
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			{
				int a{ 10 };
				int b{ 20 };
				int c{ 30 };
				Datum datum{ a, b, c };

				Datum anotherDatum{ std::move(datum) };
				Assert::AreEqual(std::size_t(3), anotherDatum.Size());
				Assert::AreEqual(a, anotherDatum.GetAsInt(size_t(0)));
				Assert::AreEqual(b, anotherDatum.GetAsInt(size_t(1)));
				Assert::AreEqual(c, anotherDatum.GetAsInt(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datum.IsEmpty());
#pragma warning(pop)

				Datum yetAnotherDatum{ c, b, a };
				yetAnotherDatum = std::move(anotherDatum);
				Assert::AreEqual(std::size_t(3), yetAnotherDatum.Size());
				Assert::AreEqual(a, yetAnotherDatum.GetAsInt(size_t(0)));
				Assert::AreEqual(b, yetAnotherDatum.GetAsInt(size_t(1)));
				Assert::AreEqual(c, yetAnotherDatum.GetAsInt(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(anotherDatum.IsEmpty());
#pragma warning(pop)

				yetAnotherDatum.SetStorage(&a, size_t(1));
				Datum datumAgain;
				datumAgain = std::move(yetAnotherDatum);
				Assert::AreEqual(std::size_t(1), datumAgain.Size());
				Assert::AreEqual(a, datumAgain.GetAsInt(size_t(0)));
				Assert::IsTrue(datumAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(yetAnotherDatum.IsEmpty());
#pragma warning(pop)

				datumAgain.SetStorage(&a, size_t(1));
				Datum datumAgainAgain{ std::move(datumAgain) };
				Assert::AreEqual(std::size_t(1), datumAgainAgain.Size());
				Assert::AreEqual(a, datumAgainAgain.GetAsInt(size_t(0)));
				Assert::IsTrue(datumAgainAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datumAgain.IsEmpty());
#pragma warning(pop)
			}
			{
				float a{ 1.0f };
				float b{ 2.0f };
				float c{ 3.0f };
				Datum datum{ a, b, c };

				Datum anotherDatum{ std::move(datum) };
				Assert::AreEqual(std::size_t(3), anotherDatum.Size());
				Assert::AreEqual(a, anotherDatum.GetAsFloat(size_t(0)));
				Assert::AreEqual(b, anotherDatum.GetAsFloat(size_t(1)));
				Assert::AreEqual(c, anotherDatum.GetAsFloat(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datum.IsEmpty());
#pragma warning(pop)

				Datum yetAnotherDatum{ c, b, a };
				yetAnotherDatum = std::move(anotherDatum);
				Assert::AreEqual(std::size_t(3), yetAnotherDatum.Size());
				Assert::AreEqual(a, yetAnotherDatum.GetAsFloat(size_t(0)));
				Assert::AreEqual(b, yetAnotherDatum.GetAsFloat(size_t(1)));
				Assert::AreEqual(c, yetAnotherDatum.GetAsFloat(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(anotherDatum.IsEmpty());
#pragma warning(pop)

				yetAnotherDatum.SetStorage(&a, size_t(1));
				Datum datumAgain;
				datumAgain = std::move(yetAnotherDatum);
				Assert::AreEqual(std::size_t(1), datumAgain.Size());
				Assert::AreEqual(a, datumAgain.GetAsFloat(size_t(0)));
				Assert::IsTrue(datumAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(yetAnotherDatum.IsEmpty());
#pragma warning(pop)

				datumAgain.SetStorage(&a, size_t(1));
				Datum datumAgainAgain{ std::move(datumAgain) };
				Assert::AreEqual(std::size_t(1), datumAgainAgain.Size());
				Assert::AreEqual(a, datumAgainAgain.GetAsFloat(size_t(0)));
				Assert::IsTrue(datumAgainAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datumAgain.IsEmpty());
#pragma warning(pop)
			}
			{
				string a{ "10" };
				string b{ "20" };
				string c{ "30" };
				Datum datum{ a, b, c };

				Datum anotherDatum{ std::move(datum) };
				Assert::AreEqual(std::size_t(3), anotherDatum.Size());
				Assert::AreEqual(a, anotherDatum.GetAsString(size_t(0)));
				Assert::AreEqual(b, anotherDatum.GetAsString(size_t(1)));
				Assert::AreEqual(c, anotherDatum.GetAsString(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datum.IsEmpty());
#pragma warning(pop)

				Datum yetAnotherDatum{ c, b, a };
				yetAnotherDatum = std::move(anotherDatum);
				Assert::AreEqual(std::size_t(3), yetAnotherDatum.Size());
				Assert::AreEqual(a, yetAnotherDatum.GetAsString(size_t(0)));
				Assert::AreEqual(b, yetAnotherDatum.GetAsString(size_t(1)));
				Assert::AreEqual(c, yetAnotherDatum.GetAsString(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(anotherDatum.IsEmpty());
#pragma warning(pop)

				yetAnotherDatum.SetStorage(&a, size_t(1));
				Datum datumAgain;
				datumAgain = std::move(yetAnotherDatum);
				Assert::AreEqual(std::size_t(1), datumAgain.Size());
				Assert::AreEqual(a, datumAgain.GetAsString(size_t(0)));
				Assert::IsTrue(datumAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(yetAnotherDatum.IsEmpty());
#pragma warning(pop)

				datumAgain.SetStorage(&a, size_t(1));
				Datum datumAgainAgain{ std::move(datumAgain) };
				Assert::AreEqual(std::size_t(1), datumAgainAgain.Size());
				Assert::AreEqual(a, datumAgainAgain.GetAsString(size_t(0)));
				Assert::IsTrue(datumAgainAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datumAgain.IsEmpty());
#pragma warning(pop)
			}
			{
				vec4 a{ 10 };
				vec4 b{ 20 };
				vec4 c{ 30 };
				Datum datum{ a, b, c };

				Datum anotherDatum{ std::move(datum) };
				Assert::AreEqual(std::size_t(3), anotherDatum.Size());
				Assert::AreEqual(a, anotherDatum.GetAsVector(size_t(0)));
				Assert::AreEqual(b, anotherDatum.GetAsVector(size_t(1)));
				Assert::AreEqual(c, anotherDatum.GetAsVector(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datum.IsEmpty());
#pragma warning(pop)

				Datum yetAnotherDatum{ c, b, a };
				yetAnotherDatum = std::move(anotherDatum);
				Assert::AreEqual(std::size_t(3), yetAnotherDatum.Size());
				Assert::AreEqual(a, yetAnotherDatum.GetAsVector(size_t(0)));
				Assert::AreEqual(b, yetAnotherDatum.GetAsVector(size_t(1)));
				Assert::AreEqual(c, yetAnotherDatum.GetAsVector(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(anotherDatum.IsEmpty());
#pragma warning(pop)

				yetAnotherDatum.SetStorage(&a, size_t(1));
				Datum datumAgain;
				datumAgain = std::move(yetAnotherDatum);
				Assert::AreEqual(std::size_t(1), datumAgain.Size());
				Assert::AreEqual(a, datumAgain.GetAsVector(size_t(0)));
				Assert::IsTrue(datumAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(yetAnotherDatum.IsEmpty());
#pragma warning(pop)

				datumAgain.SetStorage(&a, size_t(1));
				Datum datumAgainAgain{ std::move(datumAgain) };
				Assert::AreEqual(std::size_t(1), datumAgainAgain.Size());
				Assert::AreEqual(a, datumAgainAgain.GetAsVector(size_t(0)));
				Assert::IsTrue(datumAgainAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datumAgain.IsEmpty());
#pragma warning(pop)
			}
			{
				mat4 a{ 10 };
				mat4 b{ 20 };
				mat4 c{ 30 };
				Datum datum{ a, b, c };

				Datum anotherDatum{ std::move(datum) };
				Assert::AreEqual(std::size_t(3), anotherDatum.Size());
				Assert::AreEqual(a, anotherDatum.GetAsMatrix(size_t(0)));
				Assert::AreEqual(b, anotherDatum.GetAsMatrix(size_t(1)));
				Assert::AreEqual(c, anotherDatum.GetAsMatrix(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datum.IsEmpty());
#pragma warning(pop)

				Datum yetAnotherDatum{ c, b, a };
				yetAnotherDatum = std::move(anotherDatum);
				Assert::AreEqual(std::size_t(3), yetAnotherDatum.Size());
				Assert::AreEqual(a, yetAnotherDatum.GetAsMatrix(size_t(0)));
				Assert::AreEqual(b, yetAnotherDatum.GetAsMatrix(size_t(1)));
				Assert::AreEqual(c, yetAnotherDatum.GetAsMatrix(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(anotherDatum.IsEmpty());
#pragma warning(pop)

				yetAnotherDatum.SetStorage(&a, size_t(1));
				Datum datumAgain;
				datumAgain = std::move(yetAnotherDatum);
				Assert::AreEqual(std::size_t(1), datumAgain.Size());
				Assert::AreEqual(a, datumAgain.GetAsMatrix(size_t(0)));
				Assert::IsTrue(datumAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(yetAnotherDatum.IsEmpty());
#pragma warning(pop)

				datumAgain.SetStorage(&a, size_t(1));
				Datum datumAgainAgain{ std::move(datumAgain) };
				Assert::AreEqual(std::size_t(1), datumAgainAgain.Size());
				Assert::AreEqual(a, datumAgainAgain.GetAsMatrix(size_t(0)));
				Assert::IsTrue(datumAgainAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datumAgain.IsEmpty());
#pragma warning(pop)
			}
			{
				Foo aFoo{ 10 };
				Foo bFoo{ 20 };
				Foo cFoo{ 30 };
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;
				Datum datum{ a, b, c };

				Datum anotherDatum{ std::move(datum) };
				Assert::AreEqual(std::size_t(3), anotherDatum.Size());
				Assert::AreEqual(a, anotherDatum.GetAsPointer(size_t(0)));
				Assert::AreEqual(b, anotherDatum.GetAsPointer(size_t(1)));
				Assert::AreEqual(c, anotherDatum.GetAsPointer(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datum.IsEmpty());
#pragma warning(pop)

				Datum yetAnotherDatum{ c, b, a };
				yetAnotherDatum = std::move(anotherDatum);
				Assert::AreEqual(std::size_t(3), yetAnotherDatum.Size());
				Assert::AreEqual(a, yetAnotherDatum.GetAsPointer(size_t(0)));
				Assert::AreEqual(b, yetAnotherDatum.GetAsPointer(size_t(1)));
				Assert::AreEqual(c, yetAnotherDatum.GetAsPointer(size_t(2)));

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(anotherDatum.IsEmpty());
#pragma warning(pop)

				yetAnotherDatum.SetStorage(&a, size_t(1));
				Datum datumAgain;
				datumAgain = std::move(yetAnotherDatum);
				Assert::AreEqual(std::size_t(1), datumAgain.Size());
				Assert::AreEqual(a, datumAgain.GetAsPointer(size_t(0)));
				Assert::IsTrue(datumAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(yetAnotherDatum.IsEmpty());
#pragma warning(pop)

				datumAgain.SetStorage(&a, size_t(1));
				Datum datumAgainAgain{ std::move(datumAgain) };
				Assert::AreEqual(std::size_t(1), datumAgainAgain.Size());
				Assert::AreEqual(a, datumAgainAgain.GetAsPointer(size_t(0)));
				Assert::IsTrue(datumAgainAgain.IsExternal());

#pragma warning(push)
#pragma warning(disable:26800)
				Assert::IsTrue(datumAgain.IsEmpty());
#pragma warning(pop)
			}
		}

		TEST_METHOD(TypeCastContructors)
		{
			{
				const int a = 1;
				Datum datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::AreEqual(a, datum.GetAsInt());
				Assert::AreEqual(a, datum.GetAsInt(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });
			}
			{
				const float a = 1.0f;
				Datum datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::AreEqual(a, datum.GetAsFloat());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });
			}
			{
				const string a("owo");
				Datum datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::AreEqual(a, datum.GetAsString());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });
			}
			{
				const vec4 a(1, 2, 3, 4);
				Datum datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::AreEqual(a, datum.GetAsVector());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });
			}
			{
				const mat4 a(1);
				Datum datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::AreEqual(a, datum.GetAsMatrix());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });
			}
			{
				Foo a(1);
				RTTI* aPointer = &a;
				Datum datum = aPointer;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::AreEqual(aPointer, datum.GetAsPointer());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });
			}
		}

		TEST_METHOD(TypeCastAssignment) 
		{
			{
				const int a = 1;
				const int b = 2;
				Datum datum;
				datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::AreEqual(a, datum.GetAsInt());
				Assert::AreEqual(a, datum.GetAsInt(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

				datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::AreEqual(b, datum.GetAsInt());
				Assert::AreEqual(b, datum.GetAsInt(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

				const float c = 1.0f;
				Assert::ExpectException<runtime_error>([&datum, &c] {datum = c; });
			}
			{
				const float a = 1.0f;
				const float b = 2.0f;
				Datum datum;
				datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::AreEqual(a, datum.GetAsFloat());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

				datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::AreEqual(b, datum.GetAsFloat());
				Assert::AreEqual(b, datum.GetAsFloat(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

				const int c = 1;
				Assert::ExpectException<runtime_error>([&datum, &c] {datum = c; });
			}
			{
				const string a("owo");
				const string b("uwu");
				Datum datum;
				datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::AreEqual(a, datum.GetAsString());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

				datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::AreEqual(b, datum.GetAsString());
				Assert::AreEqual(b, datum.GetAsString(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

				const float c = 1.0f;
				Assert::ExpectException<runtime_error>([&datum, &c] {datum = c; });
			}
			{
				const vec4 a(1, 2, 3, 4);
				const vec4 b(2, 3, 1, 4);
				Datum datum;
				datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::AreEqual(a, datum.GetAsVector());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

				datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::AreEqual(b, datum.GetAsVector());
				Assert::AreEqual(b, datum.GetAsVector(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

				const float c = 1.0f;
				Assert::ExpectException<runtime_error>([&datum, &c] {datum = c; });
			}
			{
				const mat4 a(1);
				const mat4 b(2);
				Datum datum;
				datum = a;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::AreEqual(a, datum.GetAsMatrix());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

				datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::AreEqual(b, datum.GetAsMatrix());
				Assert::AreEqual(b, datum.GetAsMatrix(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

				const float c = 1.0f;
				Assert::ExpectException<runtime_error>([&datum, &c] {datum = c; });
			}
			{
				Foo a(1);
				Foo b(2);
				RTTI* aPointer = &a;
				RTTI* bPointer = &b;
				Datum datum;
				datum = aPointer;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::AreEqual(aPointer, datum.GetAsPointer());
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

				datum = bPointer;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::AreEqual(bPointer, datum.GetAsPointer());
				Assert::AreEqual(bPointer, datum.GetAsPointer(size_t(0)));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

				const float c = 1.0f;
				Assert::ExpectException<runtime_error>([&datum, &c] {datum = c; });
			}
		}

		TEST_METHOD(InitializerListContstructor) {
			{
				const int a(10);
				const int b(20);
				const int c(30);

				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreNotSame(a, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreNotSame(c, datum.BackAsInt());
				Assert::AreEqual(b, datum.GetAsInt(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsInt(size_t(1)));
			}
			{
				const float a(1.0f);
				const float b(2.0f);
				const float c(3.0f);

				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreNotSame(a, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreNotSame(c, datum.BackAsFloat());
				Assert::AreEqual(b, datum.GetAsFloat(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsFloat(size_t(1)));
			}
			{
				const string a("10");
				const string b("20");
				const string c("30");

				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreNotSame(a, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreNotSame(c, datum.BackAsString());
				Assert::AreEqual(b, datum.GetAsString(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsString(size_t(1)));
			}
			{
				const vec4 a(10);
				const vec4 b(20);
				const vec4 c(30);

				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreNotSame(a, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreNotSame(c, datum.BackAsVector());
				Assert::AreEqual(b, datum.GetAsVector(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsVector(size_t(1)));
			}
			{
				const mat4 a(10);
				const mat4 b(20);
				const mat4 c(30);

				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreNotSame(a, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreNotSame(c, datum.BackAsMatrix());
				Assert::AreEqual(b, datum.GetAsMatrix(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsMatrix(size_t(1)));
			}
			{
				Foo aFoo(10);
				Foo bFoo(20);
				Foo cFoo(30);
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;

				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreNotSame(a, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreNotSame(c, datum.BackAsPointer());
				Assert::AreEqual(b, datum.GetAsPointer(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsPointer(size_t(1)));
			}
		}

		TEST_METHOD(InitializerListAssignment) {
			{
				const int a(10);
				const int b(20);
				const int c(30);

				Datum datum;
				datum = { a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreNotSame(a, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreNotSame(c, datum.BackAsInt());
				Assert::AreEqual(b, datum.GetAsInt(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsInt(size_t(1)));
			}
			{
				const float a(1.0f);
				const float b(2.0f);
				const float c(3.0f);

				Datum datum;
				datum = { a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreNotSame(a, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreNotSame(c, datum.BackAsFloat());
				Assert::AreEqual(b, datum.GetAsFloat(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsFloat(size_t(1)));
			}
			{
				const string a("10");
				const string b("20");
				const string c("30");

				Datum datum;
				datum = { a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreNotSame(a, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreNotSame(c, datum.BackAsString());
				Assert::AreEqual(b, datum.GetAsString(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsString(size_t(1)));
			}
			{
				const vec4 a(10);
				const vec4 b(20);
				const vec4 c(30);

				Datum datum;
				datum = { a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreNotSame(a, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreNotSame(c, datum.BackAsVector());
				Assert::AreEqual(b, datum.GetAsVector(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsVector(size_t(1)));
			}
			{
				const mat4 a(10);
				const mat4 b(20);
				const mat4 c(30);

				Datum datum;
				datum = { a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreNotSame(a, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreNotSame(c, datum.BackAsMatrix());
				Assert::AreEqual(b, datum.GetAsMatrix(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsMatrix(size_t(1)));
			}
			{
				Foo aFoo(10);
				Foo bFoo(20);
				Foo cFoo(30);
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;

				Datum datum;
				datum = { a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreNotSame(a, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreNotSame(c, datum.BackAsPointer());
				Assert::AreEqual(b, datum.GetAsPointer(size_t(1)));
				Assert::AreNotSame(b, datum.GetAsPointer(size_t(1)));
			}
		}

		TEST_METHOD(Size) {
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const int a{ 10 };
				int e{ 13 };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());

				const Datum anotherDatum{ a };
				Assert::AreEqual(size_t(1), anotherDatum.Size());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const float a{ 1.0f };
				float e{ 1.3f };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());

				const Datum anotherDatum{ a };
				Assert::AreEqual(size_t(1), anotherDatum.Size());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const string a{ "owo"};
				string e{ "uwu" };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());

				const Datum anotherDatum{ a };
				Assert::AreEqual(size_t(1), anotherDatum.Size());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const vec4 a{ 10 };
				vec4 e{ 13 };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());

				const Datum anotherDatum{ a };
				Assert::AreEqual(size_t(1), anotherDatum.Size());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const mat4 a{ 10 };
				mat4 e{ 13 };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());

				const Datum anotherDatum{ a };
				Assert::AreEqual(size_t(1), anotherDatum.Size());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				Foo aFoo(10);
				Foo eFoo(20);
				RTTI* a = &aFoo;
				RTTI* e = &eFoo;
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());

				const Datum anotherDatum{ a };
				Assert::AreEqual(size_t(1), anotherDatum.Size());
			}
		}

		TEST_METHOD(Capacity) {
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Capacity());

				const int a{ 10 };
				int e{ 13 };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(4), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(10), datum.Capacity());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Capacity());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Capacity());

				const float a{ 1.0f };
				float e{ 1.3f };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(4), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(10), datum.Capacity());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Capacity());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Capacity());

				const string a{ "owo" };
				string e{ "uwu" };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(4), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(10), datum.Capacity());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Capacity());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Capacity());

				const vec4 a{ 10 };
				vec4 e{ 13 };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(4), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(10), datum.Capacity());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Capacity());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Capacity());

				const mat4 a{ 10 };
				mat4 e{ 13 };
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(4), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(10), datum.Capacity());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Capacity());
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Capacity());

				Foo aFoo(10);
				Foo eFoo(20);
				RTTI* a = &aFoo;
				RTTI* e = &eFoo;
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(2), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(4), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(6), datum.Capacity());

				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(10), datum.Capacity());

				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Capacity());
			}
		}

		TEST_METHOD(Type) {
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());

				datum = 5;
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
			}
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());

				string s = "owo";
				datum.SetStorage(&s, size_t(1));
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
			}
			{
				Foo aFoo(10);
				RTTI* a = &aFoo;
				Datum datum = a;
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
			}
		}

		TEST_METHOD(IsExternal) {
			{
				Datum datum;
				Assert::IsFalse(datum.IsExternal());

				datum = 5;
				Assert::IsFalse(datum.IsExternal());

				int a = 8;
				datum.SetStorage(&a, size_t(1));
				Assert::IsTrue(datum.IsExternal());

				Datum anotherDatum(Datum::DatumTypes::Pointer, size_t(5));
				Assert::IsFalse(anotherDatum.IsExternal());
			}
		}

		TEST_METHOD(IsEmpty) {
			Datum datum;
			Assert::IsTrue(datum.IsEmpty());

			int a{ 10 };
			datum.PushBack(a);
			Assert::IsFalse(datum.IsEmpty());

			datum.PopBack();
			Assert::IsTrue(datum.IsEmpty());

			datum.Reserve(10);
			Assert::IsTrue(datum.IsEmpty());

			datum.Resize(4);
			Assert::IsFalse(datum.IsEmpty());

			datum.SetStorage(&a, size_t(1));
			Assert::IsFalse(datum.IsEmpty());
		}

		TEST_METHOD(SetType) {
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());
				Assert::ExpectException<runtime_error>([&datum] {datum.SetType(Datum::DatumTypes::Unknown); });

				datum.SetType(Datum::DatumTypes::Integer);
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::ExpectException<runtime_error>([&datum] {datum.SetType(Datum::DatumTypes::Float); });
			}
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());

				datum.SetType(Datum::DatumTypes::Float);
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::ExpectException<runtime_error>([&datum] {datum.SetType(Datum::DatumTypes::Integer); });
			}
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());

				datum.SetType(Datum::DatumTypes::String);
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::ExpectException<runtime_error>([&datum] {datum.SetType(Datum::DatumTypes::Integer); });
			}
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());

				datum.SetType(Datum::DatumTypes::Vector);
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::ExpectException<runtime_error>([&datum] {datum.SetType(Datum::DatumTypes::Integer); });
			}
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());

				datum.SetType(Datum::DatumTypes::Matrix);
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::ExpectException<runtime_error>([&datum] {datum.SetType(Datum::DatumTypes::Integer); });
			}
			{
				Datum datum;
				Assert::AreEqual(Datum::DatumTypes::Unknown, datum.Type());

				datum.SetType(Datum::DatumTypes::Pointer);
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::ExpectException<runtime_error>([&datum] {datum.SetType(Datum::DatumTypes::Integer); });
			}
		}

		TEST_METHOD(GetAndSetIncrementFunctor) {
			Datum datum;
			Assert::AreEqual(size_t(0), datum.Size());
			Assert::AreEqual(size_t(0), datum.Capacity());

			auto incrementFunctor = datum.GetIncrementFunction();
			Assert::AreEqual(size_t(5), incrementFunctor(size_t(10)));

			datum.PushBack(10);
			Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
			Assert::AreEqual(size_t(1), datum.Size());
			Assert::AreEqual(size_t(1), datum.Capacity());

			datum.PushBack(10);
			Assert::AreEqual(size_t(2), datum.Size());
			Assert::AreEqual(size_t(2), datum.Capacity());

			datum.PushBack(10);
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(size_t(3), datum.Capacity());

			datum.PushBack(10);
			Assert::AreEqual(size_t(4), datum.Size());
			Assert::AreEqual(size_t(4), datum.Capacity());

			datum.PushBack(10);
			Assert::AreEqual(size_t(5), datum.Size());
			Assert::AreEqual(size_t(6), datum.Capacity());

			datum.Clear();
			datum.ShrinkToFit();

			datum.SetIncrementFunction([](size_t capacity) {return capacity * 2; });
			Assert::AreEqual(size_t(0), datum.Size());
			Assert::AreEqual(size_t(0), datum.Capacity());

			incrementFunctor = datum.GetIncrementFunction();
			Assert::AreEqual(size_t(10), incrementFunctor(size_t(5)));

			datum.PushBack(10);
			Assert::AreEqual(size_t(1), datum.Size());
			Assert::AreEqual(size_t(1), datum.Capacity());

			datum.PushBack(10);
			Assert::AreEqual(size_t(2), datum.Size());
			Assert::AreEqual(size_t(3), datum.Capacity());

			datum.PushBack(10);
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(size_t(3), datum.Capacity());

			datum.PushBack(10);
			Assert::AreEqual(size_t(4), datum.Size());
			Assert::AreEqual(size_t(9), datum.Capacity());
		}

		TEST_METHOD(Clear) {
			{
				int a = 10;
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				float a = 1.0f;
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				std::string a = "owo";
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				glm::vec4 a(10);
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				glm::mat4 a(10);
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				Foo aFoo(10);
				RTTI* a = &aFoo;
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				int a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());
				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Datum anotherDatum{ a, a, a, a, a };
				Assert::AreEqual(size_t(5), anotherDatum.Size());
				size_t originalCapacity = anotherDatum.Capacity();
				anotherDatum.Clear();
				Assert::AreEqual(size_t(0), anotherDatum.Size());
				Assert::AreEqual(originalCapacity, anotherDatum.Capacity());

				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				float a{ 1.0f };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());
				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Datum anotherDatum{ a, a, a, a, a };
				Assert::AreEqual(size_t(5), anotherDatum.Size());
				size_t originalCapacity = anotherDatum.Capacity();
				anotherDatum.Clear();
				Assert::AreEqual(size_t(0), anotherDatum.Size());
				Assert::AreEqual(originalCapacity, anotherDatum.Capacity());

				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				string a{ "owo" };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());
				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Datum anotherDatum{ a, a, a, a, a };
				Assert::AreEqual(size_t(5), anotherDatum.Size());
				size_t originalCapacity = anotherDatum.Capacity();
				anotherDatum.Clear();
				Assert::AreEqual(size_t(0), anotherDatum.Size());
				Assert::AreEqual(originalCapacity, anotherDatum.Capacity());

				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				vec4 a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());
				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Datum anotherDatum{ a, a, a, a, a };
				Assert::AreEqual(size_t(5), anotherDatum.Size());
				size_t originalCapacity = anotherDatum.Capacity();
				anotherDatum.Clear();
				Assert::AreEqual(size_t(0), anotherDatum.Size());
				Assert::AreEqual(originalCapacity, anotherDatum.Capacity());

				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				mat4 a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Datum anotherDatum{ a, a, a, a, a };
				Assert::AreEqual(size_t(5), anotherDatum.Size());
				size_t originalCapacity = anotherDatum.Capacity();
				anotherDatum.Clear();
				Assert::AreEqual(size_t(0), anotherDatum.Size());
				Assert::AreEqual(originalCapacity, anotherDatum.Capacity());

				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
			{
				Foo aFoo{ 10 };
				RTTI* a = &aFoo;
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());
				datum.Clear();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Datum anotherDatum{ a, a, a, a, a };
				Assert::AreEqual(size_t(5), anotherDatum.Size());
				size_t originalCapacity = anotherDatum.Capacity();
				anotherDatum.Clear();
				Assert::AreEqual(size_t(0), anotherDatum.Size());
				Assert::AreEqual(originalCapacity, anotherDatum.Capacity());

				datum.SetStorage(&a, 1);
				Assert::ExpectException<runtime_error>([&datum] {datum.Clear(); });
			}
		} 

		TEST_METHOD(ShrinkToFit) {
			{
				int a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.SetType(Datum::DatumTypes::Integer);
				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Assert::AreEqual(a, datum.FrontAsInt());
				datum.Reserve(10);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.ShrinkToFit(); });
			}
			{
				float a{ 1.0f };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.SetType(Datum::DatumTypes::Float);
				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Assert::AreEqual(a, datum.FrontAsFloat());
				datum.Reserve(10);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.ShrinkToFit(); });
			}
			{
				string a{ "owo"};
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.SetType(Datum::DatumTypes::String);
				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Assert::AreEqual(a, datum.FrontAsString());
				datum.Reserve(10);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.ShrinkToFit(); });
			}
			{
				vec4 a{ 1.0f };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.SetType(Datum::DatumTypes::Vector);
				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Assert::AreEqual(a, datum.FrontAsVector());
				datum.Reserve(10);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.ShrinkToFit(); });
			}
			{
				mat4 a{ 1.0f };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.SetType(Datum::DatumTypes::Matrix);
				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Assert::AreEqual(a, datum.FrontAsMatrix());
				datum.Reserve(10);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.ShrinkToFit(); });
			}
			{
				Foo aFoo{ 10 };
				RTTI* a = &aFoo;
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.SetType(Datum::DatumTypes::Pointer);
				datum.Reserve(size_t(10));
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());

				Assert::AreEqual(a, datum.FrontAsPointer());
				datum.Reserve(10);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());
				datum.ShrinkToFit();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.ShrinkToFit(); });
			}
		}

		TEST_METHOD(Reserve) {
			{
				int a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(1); });

				datum.SetType(Datum::DatumTypes::Integer);
				datum.Reserve(1);
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());

				datum.Reserve(5);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());

				datum.Reserve(0);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(5); });
			}
			{
				float a{ 1.0f };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(1); });

				datum.SetType(Datum::DatumTypes::Float);
				datum.Reserve(1);
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());

				datum.Reserve(5);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());

				datum.Reserve(0);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(5); });
			}
			{
				string a{ "10" };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(1); });

				datum.SetType(Datum::DatumTypes::String);
				datum.Reserve(1);
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());

				datum.Reserve(5);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());

				datum.Reserve(0);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(5); });
			}
			{
				vec4 a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(1); });

				datum.SetType(Datum::DatumTypes::Vector);
				datum.Reserve(1);
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());

				datum.Reserve(5);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());

				datum.Reserve(0);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(5); });
			}
			{
				mat4 a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(1); });

				datum.SetType(Datum::DatumTypes::Matrix);
				datum.Reserve(1);
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());

				datum.Reserve(5);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());

				datum.Reserve(0);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(5); });
			}
			{
				Foo aFoo{ 10 };
				RTTI* a = &aFoo;
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(1); });

				datum.SetType(Datum::DatumTypes::Pointer);
				datum.Reserve(1);
				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());

				datum.Reserve(5);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());

				datum.Reserve(0);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(5), datum.Capacity());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Reserve(5); });
			}
		}

		TEST_METHOD(Resize) {
			{
				int a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(1); });

				datum.SetType(Datum::DatumTypes::Integer);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(int(), datum.GetAsInt());

				datum.Resize(0);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsInt());

				datum.Resize(2);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(2), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsInt());
				Assert::AreEqual(int(), datum.GetAsInt(size_t(1)));

				datum.Reserve(10);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsInt());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(5); });
			}
			{
				float a{ 1.0f };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(1); });

				datum.SetType(Datum::DatumTypes::Float);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(float(), datum.GetAsFloat());

				datum.Resize(0);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsFloat());

				datum.Resize(2);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(2), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsFloat());
				Assert::AreEqual(float(), datum.GetAsFloat(size_t(1)));

				datum.Reserve(10);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsFloat());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(5); });
			}
			{
				string a{ "10" };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(1); });

				datum.SetType(Datum::DatumTypes::String);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(string(), datum.GetAsString());

				datum.Resize(0);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsString());

				datum.Resize(2);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(2), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsString());
				Assert::AreEqual(string(), datum.GetAsString(size_t(1)));

				datum.Reserve(10);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsString());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(5); });
			}
			{
				vec4 a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(1); });

				datum.SetType(Datum::DatumTypes::Vector);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(vec4(), datum.GetAsVector());

				datum.Resize(0);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsVector());

				datum.Resize(2);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(2), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsVector());
				Assert::AreEqual(vec4(), datum.GetAsVector(size_t(1)));

				datum.Reserve(10);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsVector());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(5); });
			}
			{
				mat4 a{ 10 };
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(1); });

				datum.SetType(Datum::DatumTypes::Matrix);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(mat4(), datum.GetAsMatrix());

				datum.Resize(0);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsMatrix());

				datum.Resize(2);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(2), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsMatrix());
				Assert::AreEqual(mat4(), datum.GetAsMatrix(size_t(1)));

				datum.Reserve(10);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsMatrix());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(5); });
			}
			{
				Foo aFoo{ 10 };
				RTTI* a = &aFoo;
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(1); });

				datum.SetType(Datum::DatumTypes::Pointer);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::IsNull(datum.GetAsPointer());

				datum.Resize(0);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(size_t(0), datum.Capacity());

				datum.PushBack(a);
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsPointer());

				datum.Resize(2);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(2), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsPointer());
				Assert::IsNull(datum.GetAsPointer(size_t(1)));

				datum.Reserve(10);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(size_t(10), datum.Capacity());
				datum.Resize(1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(a, datum.GetAsPointer());

				datum.SetStorage(&a, size_t(1));
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(5); });
			}
			{
				Datum datum;
				datum.SetType(Datum::DatumTypes::Table);
				Assert::ExpectException<runtime_error>([&datum] {datum.Resize(5); });
			}
		}

		TEST_METHOD(SetStorageInt)
		{
			int a = 10;
			int b = 20;
			int c = 30;
			int d[] = { a, b, c };
			int e = 40;
			{
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsInt());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsInt());
				Assert::AreEqual(d[0], datum.GetAsInt(0));
				Assert::AreEqual(d[1], datum.GetAsInt(1));
				Assert::AreEqual(d[2], datum.GetAsInt(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsInt());
				Assert::AreEqual(e, datum.GetAsInt(0));
				Assert::AreEqual(d[1], datum.GetAsInt(1));
				Assert::AreEqual(d[2], datum.GetAsInt(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsInt());

				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsInt());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsInt());
				Assert::AreEqual(d[0], datum.GetAsInt(0));
				Assert::AreEqual(d[1], datum.GetAsInt(1));
				Assert::AreEqual(d[2], datum.GetAsInt(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsInt());
				Assert::AreEqual(e, datum.GetAsInt(0));
				Assert::AreEqual(d[1], datum.GetAsInt(1));
				Assert::AreEqual(d[2], datum.GetAsInt(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsInt(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsInt());
				Assert::AreEqual(Datum::DatumTypes::Integer, datum.Type());

				float x = 0.5f;
				Assert::ExpectException<runtime_error>([&datum, &x] {datum.SetStorage(&x, 1); });
			}
			{
				Datum datum;
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetStorage(&a, 0); });

				int* nullTest = nullptr;
				Assert::ExpectException<runtime_error>([&datum, &nullTest] {datum.SetStorage(nullTest, 1); });
			}
		}

		TEST_METHOD(SetStorageFloat)
		{
			float a = 1.0f;
			float b = 2.0f;
			float c = 3.0f;
			float d[] = { a, b, c };
			float e = 4.0f;
			{
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsFloat());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsFloat());
				Assert::AreEqual(d[0], datum.GetAsFloat(0));
				Assert::AreEqual(d[1], datum.GetAsFloat(1));
				Assert::AreEqual(d[2], datum.GetAsFloat(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsFloat());
				Assert::AreEqual(e, datum.GetAsFloat(0));
				Assert::AreEqual(d[1], datum.GetAsFloat(1));
				Assert::AreEqual(d[2], datum.GetAsFloat(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsFloat());

				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsFloat());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsFloat());
				Assert::AreEqual(d[0], datum.GetAsFloat(0));
				Assert::AreEqual(d[1], datum.GetAsFloat(1));
				Assert::AreEqual(d[2], datum.GetAsFloat(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsFloat());
				Assert::AreEqual(e, datum.GetAsFloat(0));
				Assert::AreEqual(d[1], datum.GetAsFloat(1));
				Assert::AreEqual(d[2], datum.GetAsFloat(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsFloat(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsFloat());
				Assert::AreEqual(Datum::DatumTypes::Float, datum.Type());

				int x = 0;
				Assert::ExpectException<runtime_error>([&datum, &x] {datum.SetStorage(&x, 1); });
			}
			{
				Datum datum;
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetStorage(&a, 0); });

				float* nullTest = nullptr;
				Assert::ExpectException<runtime_error>([&datum, &nullTest] {datum.SetStorage(nullTest, 1); });
			}
		}

		TEST_METHOD(SetStorageString)
		{
			string a = "owo";
			string b = "uwu";
			string c = "ovo";
			string d[] = { a, b, c };
			string e = "umu";
			{
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsString());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsString());
				Assert::AreEqual(d[0], datum.GetAsString(0));
				Assert::AreEqual(d[1], datum.GetAsString(1));
				Assert::AreEqual(d[2], datum.GetAsString(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsString(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsString());
				Assert::AreEqual(e, datum.GetAsString(0));
				Assert::AreEqual(d[1], datum.GetAsString(1));
				Assert::AreEqual(d[2], datum.GetAsString(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsString(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsString());

				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsString());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsString());
				Assert::AreEqual(d[0], datum.GetAsString(0));
				Assert::AreEqual(d[1], datum.GetAsString(1));
				Assert::AreEqual(d[2], datum.GetAsString(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsString(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsString());
				Assert::AreEqual(e, datum.GetAsString(0));
				Assert::AreEqual(d[1], datum.GetAsString(1));
				Assert::AreEqual(d[2], datum.GetAsString(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsString(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsString());
				Assert::AreEqual(Datum::DatumTypes::String, datum.Type());

				int x = 0;
				Assert::ExpectException<runtime_error>([&datum, &x] {datum.SetStorage(&x, 1); });
			}
			{
				Datum datum;
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetStorage(&a, 0); });

				std::string* nullTest = nullptr;
				Assert::ExpectException<runtime_error>([&datum, &nullTest] {datum.SetStorage(nullTest, 1); });
			}
		}

		TEST_METHOD(SetStorageVector)
		{
			vec4 a(1);
			vec4 b(2);
			vec4 c(3);
			vec4 d[] = { a, b, c };
			vec4 e(4);
			{
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsVector());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsVector());
				Assert::AreEqual(d[0], datum.GetAsVector(0));
				Assert::AreEqual(d[1], datum.GetAsVector(1));
				Assert::AreEqual(d[2], datum.GetAsVector(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsVector(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsVector());
				Assert::AreEqual(e, datum.GetAsVector(0));
				Assert::AreEqual(d[1], datum.GetAsVector(1));
				Assert::AreEqual(d[2], datum.GetAsVector(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsVector(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsVector());

				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsVector());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsVector());
				Assert::AreEqual(d[0], datum.GetAsVector(0));
				Assert::AreEqual(d[1], datum.GetAsVector(1));
				Assert::AreEqual(d[2], datum.GetAsVector(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsVector(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsVector());
				Assert::AreEqual(e, datum.GetAsVector(0));
				Assert::AreEqual(d[1], datum.GetAsVector(1));
				Assert::AreEqual(d[2], datum.GetAsVector(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsVector(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsVector());
				Assert::AreEqual(Datum::DatumTypes::Vector, datum.Type());

				int x = 0;
				Assert::ExpectException<runtime_error>([&datum, &x] {datum.SetStorage(&x, 1); });
			}
			{
				Datum datum;
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetStorage(&a, 0); });

				vec4* nullTest = nullptr;
				Assert::ExpectException<runtime_error>([&datum, &nullTest] {datum.SetStorage(nullTest, 1); });
			}
		}

		TEST_METHOD(SetStorageMatrix)
		{
			mat4 a(1);
			mat4 b(2);
			mat4 c(3);
			mat4 d[] = { a, b, c };
			mat4 e(4);
			{
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsMatrix());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsMatrix());
				Assert::AreEqual(d[0], datum.GetAsMatrix(0));
				Assert::AreEqual(d[1], datum.GetAsMatrix(1));
				Assert::AreEqual(d[2], datum.GetAsMatrix(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsMatrix(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsMatrix());
				Assert::AreEqual(e, datum.GetAsMatrix(0));
				Assert::AreEqual(d[1], datum.GetAsMatrix(1));
				Assert::AreEqual(d[2], datum.GetAsMatrix(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsMatrix(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsMatrix());

				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsMatrix());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsMatrix());
				Assert::AreEqual(d[0], datum.GetAsMatrix(0));
				Assert::AreEqual(d[1], datum.GetAsMatrix(1));
				Assert::AreEqual(d[2], datum.GetAsMatrix(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsMatrix(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsMatrix());
				Assert::AreEqual(e, datum.GetAsMatrix(0));
				Assert::AreEqual(d[1], datum.GetAsMatrix(1));
				Assert::AreEqual(d[2], datum.GetAsMatrix(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsMatrix(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsMatrix());
				Assert::AreEqual(Datum::DatumTypes::Matrix, datum.Type());

				int x = 0;
				Assert::ExpectException<runtime_error>([&datum, &x] {datum.SetStorage(&x, 1); });
			}
			{
				Datum datum;
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetStorage(&a, 0); });

				mat4* nullTest = nullptr;
				Assert::ExpectException<runtime_error>([&datum, &nullTest] {datum.SetStorage(nullTest, 1); });
			}
		}

		TEST_METHOD(SetStoragePointer)
		{
			Foo aFoo(10);
			Foo bFoo(20);
			Foo cFoo(30);
			Foo eFoo(40);
			RTTI* a = &aFoo;
			RTTI* b = &bFoo;
			RTTI* c = &cFoo;
			RTTI* d[] = { a, b, c };
			RTTI* e = &eFoo;
			{
				Datum datum;
				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsPointer());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsPointer());
				Assert::AreEqual(d[0], datum.GetAsPointer(0));
				Assert::AreEqual(d[1], datum.GetAsPointer(1));
				Assert::AreEqual(d[2], datum.GetAsPointer(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsPointer(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsPointer());
				Assert::AreEqual(e, datum.GetAsPointer(0));
				Assert::AreEqual(d[1], datum.GetAsPointer(1));
				Assert::AreEqual(d[2], datum.GetAsPointer(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsPointer(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsPointer());

				datum.SetStorage(&a, 1);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&a, &datum.GetAsPointer());

				datum.SetStorage(d, std::size(d));
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsPointer());
				Assert::AreEqual(d[0], datum.GetAsPointer(0));
				Assert::AreEqual(d[1], datum.GetAsPointer(1));
				Assert::AreEqual(d[2], datum.GetAsPointer(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsPointer(3); UNREFERENCED_LOCAL(value); });

				datum.Set(e);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(size_t(3), datum.Capacity());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());
				Assert::IsTrue(datum.IsExternal());
				Assert::AreEqual(&d[0], &datum.GetAsPointer());
				Assert::AreEqual(e, datum.GetAsPointer(0));
				Assert::AreEqual(d[1], datum.GetAsPointer(1));
				Assert::AreEqual(d[2], datum.GetAsPointer(2));
				Assert::ExpectException<runtime_error>([&datum] {auto& value = datum.GetAsPointer(3); UNREFERENCED_LOCAL(value); });
			}
			{
				Datum datum = b;
				Assert::IsFalse(datum.IsExternal());
				Assert::AreEqual(b, datum.GetAsPointer());
				Assert::AreEqual(Datum::DatumTypes::Pointer, datum.Type());

				int x = 0;
				Assert::ExpectException<runtime_error>([&datum, &x] {datum.SetStorage(&x, 1); });
			}
			{
				Datum datum;
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetStorage(&a, 0); });

				RTTI** nullTest = nullptr;
				Assert::ExpectException<runtime_error>([&datum, &nullTest] {datum.SetStorage(nullTest, 1); });
			}
		}

		TEST_METHOD(GetFront)
		{
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsInt(); UNREFERENCED_LOCAL(front); });
				datum.SetType(Datum::DatumTypes::Integer);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsInt(); UNREFERENCED_LOCAL(front); });
				datum = Datum(Datum::DatumTypes::Integer, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsInt(); UNREFERENCED_LOCAL(front); });

				const int a{ 10 };
				datum.PushBack(a);
				const int b = datum.FrontAsInt();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const int c{ 20 };
				datum.PushBack(c);
				const int d = datum.FrontAsInt();
				Assert::AreEqual(a, d);
				Assert::AreNotSame(a, d);

				int e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.FrontAsInt());
				Assert::AreSame(e, datum.FrontAsInt());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsFloat(); UNREFERENCED_LOCAL(front); });
				datum.SetType(Datum::DatumTypes::Float);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsFloat(); UNREFERENCED_LOCAL(front); });
				datum = Datum(Datum::DatumTypes::Float, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsFloat(); UNREFERENCED_LOCAL(front); });

				const float a{ 1.0f };
				datum.PushBack(a);
				const float b = datum.FrontAsFloat();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const float c{ 2.0f };
				datum.PushBack(c);
				const float d = datum.FrontAsFloat();
				Assert::AreEqual(a, d);
				Assert::AreNotSame(a, d);

				float e{ 1.3f };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.FrontAsFloat());
				Assert::AreSame(e, datum.FrontAsFloat());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsString(); UNREFERENCED_LOCAL(front); });
				datum.SetType(Datum::DatumTypes::String);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsString(); UNREFERENCED_LOCAL(front); });
				datum = Datum(Datum::DatumTypes::String, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsString(); UNREFERENCED_LOCAL(front); });

				const string a{ "owo" };
				datum.PushBack(a);
				const string b = datum.FrontAsString();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const string c{ "uwu"};
				datum.PushBack(c);
				const string d = datum.FrontAsString();
				Assert::AreEqual(a, d);
				Assert::AreNotSame(a, d);

				string e{ "eek" };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.FrontAsString());
				Assert::AreSame(e, datum.FrontAsString());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsVector(); UNREFERENCED_LOCAL(front); });
				datum.SetType(Datum::DatumTypes::Vector);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsVector(); UNREFERENCED_LOCAL(front); });
				datum = Datum(Datum::DatumTypes::Vector, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsVector(); UNREFERENCED_LOCAL(front); });

				const vec4 a{ 10 };
				datum.PushBack(a);
				const vec4 b = datum.FrontAsVector();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const vec4 c{ 20 };
				datum.PushBack(c);
				const vec4 d = datum.FrontAsVector();
				Assert::AreEqual(a, d);
				Assert::AreNotSame(a, d);

				vec4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.FrontAsVector());
				Assert::AreSame(e, datum.FrontAsVector());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				datum.SetType(Datum::DatumTypes::Matrix);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				datum = Datum(Datum::DatumTypes::Matrix, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });

				const mat4 a{ 10 };
				datum.PushBack(a);
				const mat4 b = datum.FrontAsMatrix();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const mat4 c{ 20 };
				datum.PushBack(c);
				const mat4 d = datum.FrontAsMatrix();
				Assert::AreEqual(a, d);
				Assert::AreNotSame(a, d);

				mat4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.FrontAsMatrix());
				Assert::AreSame(e, datum.FrontAsMatrix());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsPointer(); UNREFERENCED_LOCAL(front); });
				datum.SetType(Datum::DatumTypes::Pointer);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsPointer(); UNREFERENCED_LOCAL(front); });
				datum = Datum(Datum::DatumTypes::Pointer, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsPointer(); UNREFERENCED_LOCAL(front); });

				Foo aFoo{ 10 };
				RTTI* a{ &aFoo };
				datum.PushBack(a);
				RTTI* b = datum.FrontAsPointer();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				Foo cFoo{ 20 };
				RTTI* c{ &cFoo };
				datum.PushBack(c);
				RTTI* d = datum.FrontAsPointer();
				Assert::AreEqual(a, d);
				Assert::AreNotSame(a, d);

				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.FrontAsPointer());
				Assert::AreSame(e, datum.FrontAsPointer());
			}
		}

		TEST_METHOD(GetFrontConst)
		{
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& front = datum1.FrontAsInt(); UNREFERENCED_LOCAL(front); });
				const Datum datum2(Datum::DatumTypes::Integer);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& front = datum2.FrontAsInt(); UNREFERENCED_LOCAL(front); });
				const Datum datum3 = Datum(Datum::DatumTypes::Integer, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& front = datum3.FrontAsInt(); UNREFERENCED_LOCAL(front); });

				Datum datum;
				const int a{ 10 };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const int b = constDatum.FrontAsInt();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const int c{ 20 };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const int d = constDatum.FrontAsInt();
					Assert::AreEqual(a, d);
					Assert::AreNotSame(a, d);
				}				
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& front = datum1.FrontAsFloat(); UNREFERENCED_LOCAL(front); });
				const Datum datum2(Datum::DatumTypes::Float);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& front = datum2.FrontAsFloat(); UNREFERENCED_LOCAL(front); });
				const Datum datum3 = Datum(Datum::DatumTypes::Float, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& front = datum3.FrontAsFloat(); UNREFERENCED_LOCAL(front); });

				Datum datum;
				const float a{ 1.0f };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const float b = constDatum.FrontAsFloat();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const float c{ 2.0f };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const float d = constDatum.FrontAsFloat();
					Assert::AreEqual(a, d);
					Assert::AreNotSame(a, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& front = datum1.FrontAsString(); UNREFERENCED_LOCAL(front); });
				const Datum datum2(Datum::DatumTypes::String);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& front = datum2.FrontAsString(); UNREFERENCED_LOCAL(front); });
				const Datum datum3 = Datum(Datum::DatumTypes::String, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& front = datum3.FrontAsString(); UNREFERENCED_LOCAL(front); });

				Datum datum;
				const string a{ "owo" };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const string b = constDatum.FrontAsString();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const string c{ "uwu" };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const string d = constDatum.FrontAsString();
					Assert::AreEqual(a, d);
					Assert::AreNotSame(a, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& front = datum1.FrontAsVector(); UNREFERENCED_LOCAL(front); });
				const Datum datum2(Datum::DatumTypes::Vector);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& front = datum2.FrontAsVector(); UNREFERENCED_LOCAL(front); });
				const Datum datum3 = Datum(Datum::DatumTypes::Vector, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& front = datum3.FrontAsVector(); UNREFERENCED_LOCAL(front); });

				Datum datum;
				const vec4 a{ 10 };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const vec4 b = constDatum.FrontAsVector();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const vec4 c{ 20 };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const vec4 d = constDatum.FrontAsVector();
					Assert::AreEqual(a, d);
					Assert::AreNotSame(a, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& front = datum1.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				const Datum datum2(Datum::DatumTypes::Matrix);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& front = datum2.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				const Datum datum3 = Datum(Datum::DatumTypes::Matrix, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& front = datum3.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });

				Datum datum;
				const mat4 a{ 10 };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const mat4 b = constDatum.FrontAsMatrix();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const mat4 c{ 20 };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const mat4 d = constDatum.FrontAsMatrix();
					Assert::AreEqual(a, d);
					Assert::AreNotSame(a, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& front = datum1.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				const Datum datum2(Datum::DatumTypes::Matrix);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& front = datum2.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				const Datum datum3 = Datum(Datum::DatumTypes::Matrix, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& front = datum3.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });

				Datum datum;
				Foo aFoo{ 10 };
				RTTI* a{ &aFoo };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const RTTI* b = constDatum.FrontAsPointer();
					Assert::IsTrue(a == b);
				}

				Foo cFoo{ 20 };
				RTTI* c{ &cFoo };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const RTTI* d = constDatum.FrontAsPointer();
					Assert::IsTrue(a == d);
				}				
			}
		}

		TEST_METHOD(GetBack)
		{
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsInt(); UNREFERENCED_LOCAL(back); });
				datum.SetType(Datum::DatumTypes::Integer);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsInt(); UNREFERENCED_LOCAL(back); });
				datum = Datum(Datum::DatumTypes::Integer, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsInt(); UNREFERENCED_LOCAL(back); });

				const int a{ 10 };
				datum.PushBack(a);
				const int b = datum.BackAsInt();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const int c{ 20 };
				datum.PushBack(c);
				const int d = datum.BackAsInt();
				Assert::AreEqual(c, d);
				Assert::AreNotEqual(a, d);
				Assert::AreNotSame(c, d);

				int e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.BackAsInt());
				Assert::AreSame(e, datum.BackAsInt());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.FrontAsFloat(); UNREFERENCED_LOCAL(back); });
				datum.SetType(Datum::DatumTypes::Float);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.FrontAsFloat(); UNREFERENCED_LOCAL(back); });
				datum = Datum(Datum::DatumTypes::Float, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.FrontAsFloat(); UNREFERENCED_LOCAL(back); });

				const float a{ 1.0f };
				datum.PushBack(a);
				const float b = datum.BackAsFloat();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const float c{ 2.0f };
				datum.PushBack(c);
				const float d = datum.BackAsFloat();
				Assert::AreEqual(c, d);
				Assert::AreNotEqual(a, d);
				Assert::AreNotSame(c, d);

				float e{ 1.3f };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.BackAsFloat());
				Assert::AreSame(e, datum.BackAsFloat());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsString(); UNREFERENCED_LOCAL(back); });
				datum.SetType(Datum::DatumTypes::String);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsString(); UNREFERENCED_LOCAL(back); });
				datum = Datum(Datum::DatumTypes::String, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsString(); UNREFERENCED_LOCAL(back); });

				const string a{ "owo" };
				datum.PushBack(a);
				const string b = datum.BackAsString();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const string c{ "uwu" };
				datum.PushBack(c);
				const string d = datum.BackAsString();
				Assert::AreEqual(c, d);
				Assert::AreNotEqual(a, d);
				Assert::AreNotSame(c, d);

				string e{ "eek" };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.BackAsString());
				Assert::AreSame(e, datum.BackAsString());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsVector(); UNREFERENCED_LOCAL(back); });
				datum.SetType(Datum::DatumTypes::Vector);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsVector(); UNREFERENCED_LOCAL(back); });
				datum = Datum(Datum::DatumTypes::Vector, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsVector(); UNREFERENCED_LOCAL(back); });

				const vec4 a{ 10 };
				datum.PushBack(a);
				const vec4 b = datum.BackAsVector();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const vec4 c{ 20 };
				datum.PushBack(c);
				const vec4 d = datum.BackAsVector();
				Assert::AreEqual(c, d);
				Assert::AreNotEqual(a, d);
				Assert::AreNotSame(c, d);

				vec4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.BackAsVector());
				Assert::AreSame(e, datum.BackAsVector());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				datum.SetType(Datum::DatumTypes::Matrix);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				datum = Datum(Datum::DatumTypes::Matrix, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsMatrix(); UNREFERENCED_LOCAL(back); });

				const mat4 a{ 10 };
				datum.PushBack(a);
				const mat4 b = datum.BackAsMatrix();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				const mat4 c{ 20 };
				datum.PushBack(c);
				const mat4 d = datum.BackAsMatrix();
				Assert::AreEqual(c, d);
				Assert::AreNotEqual(a, d);
				Assert::AreNotSame(c, d);

				mat4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.BackAsMatrix());
				Assert::AreSame(e, datum.BackAsMatrix());
			}
			{
				Datum datum;
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsPointer(); UNREFERENCED_LOCAL(back); });
				datum.SetType(Datum::DatumTypes::Pointer);
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsPointer(); UNREFERENCED_LOCAL(back); });
				datum = Datum(Datum::DatumTypes::Pointer, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsPointer(); UNREFERENCED_LOCAL(back); });

				Foo aFoo{ 10 };
				RTTI* a{ &aFoo };
				datum.PushBack(a);
				RTTI* b = datum.BackAsPointer();
				Assert::AreEqual(a, b);
				Assert::AreNotSame(a, b);

				Foo cFoo{ 20 };
				RTTI* c{ &cFoo };
				datum.PushBack(c);
				RTTI* d = datum.BackAsPointer();
				Assert::AreEqual(c, d);
				Assert::AreNotEqual(a, d);
				Assert::AreNotSame(c, d);

				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				datum.SetStorage(&e, 1);
				Assert::AreEqual(e, datum.BackAsPointer());
				Assert::AreSame(e, datum.BackAsPointer());
			}
		}

		TEST_METHOD(GetBackConst)
		{
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& back = datum1.BackAsInt(); UNREFERENCED_LOCAL(back); });
				const Datum datum2(Datum::DatumTypes::Integer);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& back = datum2.BackAsInt(); UNREFERENCED_LOCAL(back); });
				const Datum datum3 = Datum(Datum::DatumTypes::Integer, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& back = datum3.BackAsInt(); UNREFERENCED_LOCAL(back); });

				Datum datum;
				const int a{ 10 };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const int b = constDatum.BackAsInt();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const int c{ 20 };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const int d = constDatum.BackAsInt();
					Assert::AreEqual(c, d);
					Assert::AreNotEqual(a, d);
					Assert::AreNotSame(c, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& back = datum1.BackAsFloat(); UNREFERENCED_LOCAL(back); });
				const Datum datum2(Datum::DatumTypes::Float);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& back = datum2.BackAsFloat(); UNREFERENCED_LOCAL(back); });
				const Datum datum3 = Datum(Datum::DatumTypes::Float, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& back = datum3.BackAsFloat(); UNREFERENCED_LOCAL(back); });

				Datum datum;
				const float a{ 1.0f };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const float b = constDatum.BackAsFloat();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const float c{ 2.0f };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const float d = constDatum.BackAsFloat();
					Assert::AreEqual(c, d);
					Assert::AreNotEqual(a, d);
					Assert::AreNotSame(c, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& back = datum1.BackAsString(); UNREFERENCED_LOCAL(back); });
				const Datum datum2(Datum::DatumTypes::String);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& back = datum2.BackAsString(); UNREFERENCED_LOCAL(back); });
				const Datum datum3 = Datum(Datum::DatumTypes::String, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& back = datum3.BackAsString(); UNREFERENCED_LOCAL(back); });

				Datum datum;
				const string a{ "owo" };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const string b = constDatum.BackAsString();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const string c{ "uwu" };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const string d = constDatum.BackAsString();
					Assert::AreEqual(c, d);
					Assert::AreNotEqual(a, d);
					Assert::AreNotSame(c, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& back = datum1.BackAsVector(); UNREFERENCED_LOCAL(back); });
				const Datum datum2(Datum::DatumTypes::Vector);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& back = datum2.BackAsVector(); UNREFERENCED_LOCAL(back); });
				const Datum datum3 = Datum(Datum::DatumTypes::Vector, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& back = datum3.BackAsVector(); UNREFERENCED_LOCAL(back); });

				Datum datum;
				const vec4 a{ 10 };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const vec4 b = constDatum.BackAsVector();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const vec4 c{ 20 };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const vec4 d = constDatum.BackAsVector();
					Assert::AreEqual(c, d);
					Assert::AreNotEqual(a, d);
					Assert::AreNotSame(c, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& back = datum1.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				const Datum datum2(Datum::DatumTypes::Matrix);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& back = datum2.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				const Datum datum3 = Datum(Datum::DatumTypes::Matrix, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& back = datum3.BackAsMatrix(); UNREFERENCED_LOCAL(back); });

				Datum datum;
				const mat4 a{ 10 };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const mat4 b = constDatum.BackAsMatrix();
					Assert::AreEqual(a, b);
					Assert::AreNotSame(a, b);
				}
				const mat4 c{ 20 };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const mat4 d = constDatum.BackAsMatrix();
					Assert::AreEqual(c, d);
					Assert::AreNotEqual(a, d);
					Assert::AreNotSame(c, d);
				}
			}
			{
				const Datum datum1;
				Assert::ExpectException<std::runtime_error>([&datum1]() {const auto& back = datum1.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				const Datum datum2(Datum::DatumTypes::Matrix);
				Assert::ExpectException<std::runtime_error>([&datum2]() {const auto& back = datum2.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				const Datum datum3 = Datum(Datum::DatumTypes::Matrix, std::size_t(1));
				Assert::ExpectException<std::runtime_error>([&datum3]() {const auto& back = datum3.BackAsMatrix(); UNREFERENCED_LOCAL(back); });

				Datum datum;
				Foo aFoo{ 10 };
				RTTI* a{ &aFoo };
				datum.PushBack(a);
				{
					const Datum constDatum{ datum };
					const RTTI* b = constDatum.BackAsPointer();
					Assert::IsTrue(a == b);
				}

				Foo cFoo{ 20 };
				RTTI* c{ &cFoo };
				datum.PushBack(c);
				{
					const Datum constDatum{ datum };
					const RTTI* d = constDatum.BackAsPointer();
					Assert::IsTrue(c == d);
					Assert::IsTrue(a != d);
				}
			}
		}

		TEST_METHOD(GetAsInt) {
			const int a(10);
			const int b(10);
			const int c(20);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsInt());
			Assert::AreNotSame(a, datum.GetAsInt());
			Assert::AreEqual(b, datum.GetAsInt(1));
			Assert::AreNotSame(b, datum.GetAsInt(1));
			Assert::AreEqual(c, datum.GetAsInt(2));
			Assert::AreNotSame(c, datum.GetAsInt(2));

			datum.GetAsInt() = c;
			Assert::AreEqual(c, datum.GetAsInt());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsInt(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsInt(3); UNREFERENCED_LOCAL(value); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsInt(3));
			Assert::AreNotSame(a, datum.GetAsInt(3));
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsInt(4); UNREFERENCED_LOCAL(value); });

			int e{ 13 };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsInt());
			Assert::AreSame(e, datum.GetAsInt());
			datum.GetAsInt() = c;
			Assert::AreEqual(c, datum.GetAsInt());
			Assert::AreSame(e, datum.GetAsInt());
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsInt(1); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsIntConst) {
			const int a(10);
			const int b(10);
			const int c(20);

			const Datum testDatum;
			Assert::ExpectException<std::runtime_error>([&testDatum]() {auto value = testDatum.GetAsInt(); UNREFERENCED_LOCAL(value); });

			const Datum datum{ a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsInt());
			Assert::AreNotSame(a, datum.GetAsInt());
			Assert::AreEqual(b, datum.GetAsInt(1));
			Assert::AreNotSame(b, datum.GetAsInt(1));
			Assert::AreEqual(c, datum.GetAsInt(2));
			Assert::AreNotSame(c, datum.GetAsInt(2));

			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsInt(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsFloat) {
			const float a(1.0f);
			const float b(1.0f);
			const float c(2.0f);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsFloat());
			Assert::AreNotSame(a, datum.GetAsFloat());
			Assert::AreEqual(b, datum.GetAsFloat(1));
			Assert::AreNotSame(b, datum.GetAsFloat(1));
			Assert::AreEqual(c, datum.GetAsFloat(2));
			Assert::AreNotSame(c, datum.GetAsFloat(2));

			datum.GetAsFloat() = c;
			Assert::AreEqual(c, datum.GetAsFloat());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsFloat(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsFloat(3); UNREFERENCED_LOCAL(value); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsFloat(3));
			Assert::AreNotSame(a, datum.GetAsFloat(3));
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsFloat(4); UNREFERENCED_LOCAL(value); });

			float e{ 1.3f };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsFloat());
			Assert::AreSame(e, datum.GetAsFloat());
			datum.GetAsFloat() = c;
			Assert::AreEqual(c, datum.GetAsFloat());
			Assert::AreSame(e, datum.GetAsFloat());
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsFloat(1); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsFloatConst) {
			const float a(1.0f);
			const float b(1.0f);
			const float c(2.0f);

			const Datum testDatum;
			Assert::ExpectException<std::runtime_error>([&testDatum]() {auto value = testDatum.GetAsFloat(); UNREFERENCED_LOCAL(value); });

			const Datum datum{ a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsFloat());
			Assert::AreNotSame(a, datum.GetAsFloat());
			Assert::AreEqual(b, datum.GetAsFloat(1));
			Assert::AreNotSame(b, datum.GetAsFloat(1));
			Assert::AreEqual(c, datum.GetAsFloat(2));
			Assert::AreNotSame(c, datum.GetAsFloat(2));

			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsFloat(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsString) {
			const string a("10");
			const string b("10");
			const string c("20");

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsString(); UNREFERENCED_LOCAL(value); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsString());
			Assert::AreNotSame(a, datum.GetAsString());
			Assert::AreEqual(b, datum.GetAsString(1));
			Assert::AreNotSame(b, datum.GetAsString(1));
			Assert::AreEqual(c, datum.GetAsString(2));
			Assert::AreNotSame(c, datum.GetAsString(2));

			datum.GetAsString() = c;
			Assert::AreEqual(c, datum.GetAsString());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsString(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsString(3); UNREFERENCED_LOCAL(value); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsString(3));
			Assert::AreNotSame(a, datum.GetAsString(3));
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsString(4); UNREFERENCED_LOCAL(value); });

			string e{ "13" };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsString());
			Assert::AreSame(e, datum.GetAsString());
			datum.GetAsString() = c;
			Assert::AreEqual(c, datum.GetAsString());
			Assert::AreSame(e, datum.GetAsString());
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsString(1); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsStringConst) {
			const string a("10");
			const string b("10");
			const string c("20");

			const Datum testDatum;
			Assert::ExpectException<std::runtime_error>([&testDatum]() {auto value = testDatum.GetAsString(); UNREFERENCED_LOCAL(value); });

			const Datum datum{ a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsString());
			Assert::AreNotSame(a, datum.GetAsString());
			Assert::AreEqual(b, datum.GetAsString(1));
			Assert::AreNotSame(b, datum.GetAsString(1));
			Assert::AreEqual(c, datum.GetAsString(2));
			Assert::AreNotSame(c, datum.GetAsString(2));

			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsString(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsVector) {
			const vec4 a(10);
			const vec4 b(10);
			const vec4 c(20);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsVector());
			Assert::AreNotSame(a, datum.GetAsVector());
			Assert::AreEqual(b, datum.GetAsVector(1));
			Assert::AreNotSame(b, datum.GetAsVector(1));
			Assert::AreEqual(c, datum.GetAsVector(2));
			Assert::AreNotSame(c, datum.GetAsVector(2));

			datum.GetAsVector() = c;
			Assert::AreEqual(c, datum.GetAsVector());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(3); UNREFERENCED_LOCAL(value); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsVector(3));
			Assert::AreNotSame(a, datum.GetAsVector(3));
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(4); UNREFERENCED_LOCAL(value); });

			vec4 e{ 13 };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsVector());
			Assert::AreSame(e, datum.GetAsVector());
			datum.GetAsVector() = c;
			Assert::AreEqual(c, datum.GetAsVector());
			Assert::AreSame(e, datum.GetAsVector());
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(1); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsVectorConst) {
			const vec4 a(10);
			const vec4 b(10);
			const vec4 c(20);

			const Datum testDatum;
			Assert::ExpectException<std::runtime_error>([&testDatum]() {auto value = testDatum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			const Datum datum{ a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsVector());
			Assert::AreNotSame(a, datum.GetAsVector());
			Assert::AreEqual(b, datum.GetAsVector(1));
			Assert::AreNotSame(b, datum.GetAsVector(1));
			Assert::AreEqual(c, datum.GetAsVector(2));
			Assert::AreNotSame(c, datum.GetAsVector(2));

			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsVector(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsInt(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsMatrix) {
			const mat4 a(10);
			const mat4 b(10);
			const mat4 c(20);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsMatrix(); UNREFERENCED_LOCAL(value); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsMatrix());
			Assert::AreNotSame(a, datum.GetAsMatrix());
			Assert::AreEqual(b, datum.GetAsMatrix(1));
			Assert::AreNotSame(b, datum.GetAsMatrix(1));
			Assert::AreEqual(c, datum.GetAsMatrix(2));
			Assert::AreNotSame(c, datum.GetAsMatrix(2));

			datum.GetAsMatrix() = c;
			Assert::AreEqual(c, datum.GetAsMatrix());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsMatrix(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsMatrix(3); UNREFERENCED_LOCAL(value); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsMatrix(3));
			Assert::AreNotSame(a, datum.GetAsMatrix(3));
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsMatrix(4); UNREFERENCED_LOCAL(value); });

			mat4 e{ 13 };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsMatrix());
			Assert::AreSame(e, datum.GetAsMatrix());
			datum.GetAsMatrix() = c;
			Assert::AreEqual(c, datum.GetAsMatrix());
			Assert::AreSame(e, datum.GetAsMatrix());
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsMatrix(1); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsMatrixConst) {
			const mat4 a(10);
			const mat4 b(10);
			const mat4 c(20);

			const Datum testDatum;
			Assert::ExpectException<std::runtime_error>([&testDatum]() {auto value = testDatum.GetAsMatrix(); UNREFERENCED_LOCAL(value); });

			const Datum datum{ a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsMatrix());
			Assert::AreNotSame(a, datum.GetAsMatrix());
			Assert::AreEqual(b, datum.GetAsMatrix(1));
			Assert::AreNotSame(b, datum.GetAsMatrix(1));
			Assert::AreEqual(c, datum.GetAsMatrix(2));
			Assert::AreNotSame(c, datum.GetAsMatrix(2));

			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsMatrix(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsPointer) {
			Foo aFoo(10);
			Foo bFoo(10);
			Foo cFoo(20);
			RTTI* a = &aFoo;
			RTTI* b = &bFoo;
			RTTI* c = &cFoo;

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsPointer(); UNREFERENCED_LOCAL(value); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsPointer());
			Assert::AreNotSame(a, datum.GetAsPointer());
			Assert::AreEqual(b, datum.GetAsPointer(1));
			Assert::AreNotSame(b, datum.GetAsPointer(1));
			Assert::AreEqual(c, datum.GetAsPointer(2));
			Assert::AreNotSame(c, datum.GetAsPointer(2));

			datum.GetAsPointer() = c;
			Assert::AreEqual(c, datum.GetAsPointer());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsPointer(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsPointer(3); UNREFERENCED_LOCAL(value); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsPointer(3));
			Assert::AreNotSame(a, datum.GetAsPointer(3));
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsPointer(4); UNREFERENCED_LOCAL(value); });

			Foo eFoo{ 13 };
			RTTI* e = &eFoo;
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsPointer());
			Assert::AreSame(e, datum.GetAsPointer());
			datum.GetAsPointer() = c;
			Assert::AreEqual(c, datum.GetAsPointer());
			Assert::AreSame(e, datum.GetAsPointer());
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsPointer(1); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsPointerConst) {
			Foo aFoo(10);
			Foo bFoo(10);
			Foo cFoo(20);
			RTTI* a = &aFoo;
			RTTI* b = &bFoo;
			RTTI* c = &cFoo;

			const Datum testDatum;
			Assert::ExpectException<std::runtime_error>([&testDatum]() {auto value = testDatum.GetAsPointer(); UNREFERENCED_LOCAL(value); });

			const Datum datum{ a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());
			Assert::AreEqual(a, datum.GetAsPointer());
			Assert::AreNotSame(a, datum.GetAsPointer());
			Assert::AreEqual(b, datum.GetAsPointer(1));
			Assert::AreNotSame(b, datum.GetAsPointer(1));
			Assert::AreEqual(c, datum.GetAsPointer(2));
			Assert::AreNotSame(c, datum.GetAsPointer(2));

			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsPointer(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsTable) {
			Scope parentScope;
			Scope& scope = parentScope.AppendScope("Child");

			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = 700.5f;
			scope.Append("A") = aDatum;
			scope.Append("B") = bDatum;
			scope.Append("C") = cDatum;
			auto& catchUnused = scope["D"];
			UNREFERENCED_LOCAL(catchUnused);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsTable(); UNREFERENCED_LOCAL(value); });

			datum = parentScope[0];
			Assert::AreEqual(size_t(1), datum.Size());
			Assert::AreEqual(scope, datum.GetAsTable());

			size_t initSize = datum.GetAsTable().Size();
			datum.GetAsTable().Append("test");
			Assert::AreEqual(initSize + 1, datum.GetAsTable().Size());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsTable(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsTable(3); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(GetAsTableConst) {
			Scope parentScope;
			Scope& scope = parentScope.AppendScope("Child");

			Datum aDatum = 1;
			Datum bDatum = "owo"s;
			Datum cDatum = 700.5f;
			scope.Append("A") = aDatum;
			scope.Append("B") = bDatum;
			scope.Append("C") = cDatum;
			auto& catchUnused = scope["D"];
			UNREFERENCED_LOCAL(catchUnused);

			const Datum datum1;
			Assert::ExpectException<std::runtime_error>([&datum1]() {auto& value = datum1.GetAsTable(); UNREFERENCED_LOCAL(value); });

			const Datum datum = parentScope[0];
			Assert::AreEqual(size_t(1), datum.Size());
			Assert::AreEqual(scope, datum.GetAsTable());

			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsTable(3); UNREFERENCED_LOCAL(value); });
			Assert::ExpectException<std::runtime_error>([&datum]() {auto& value = datum.GetAsVector(); UNREFERENCED_LOCAL(value); });
		}

		TEST_METHOD(SetInt) {
			const int a(10);
			const int b(10);
			const int c(20);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());

			Assert::AreEqual(a, datum.GetAsInt());
			Assert::AreNotSame(a, datum.GetAsInt());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsInt());

			Assert::AreEqual(b, datum.GetAsInt(size_t(1)));
			Assert::AreNotSame(b, datum.GetAsInt(size_t(1)));
			datum.Set(c, size_t(1));
			Assert::AreEqual(c, datum.GetAsInt(size_t(1)));

			Assert::AreEqual(c, datum.GetAsInt(size_t(2)));
			Assert::AreNotSame(c, datum.GetAsInt(size_t(2)));
			datum.Set(a, size_t(2));
			Assert::AreEqual(a, datum.GetAsInt(size_t(2)));

			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(0.5f); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsInt(size_t(3)));
			Assert::AreNotSame(a, datum.GetAsInt(size_t(3)));
			datum.Set(b, size_t(3));
			Assert::AreEqual(b, datum.GetAsInt(size_t(3)));
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(4)); });

			int e{ 13 };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsInt());
			Assert::AreSame(e, datum.GetAsInt());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsInt());
			Assert::AreSame(e, datum.GetAsInt());
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(0.5f); });
		}

		TEST_METHOD(SetFloat) {
			const float a(1.0f);
			const float b(1.0f);
			const float c(2.0f);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());

			Assert::AreEqual(a, datum.GetAsFloat());
			Assert::AreNotSame(a, datum.GetAsFloat());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsFloat());

			Assert::AreEqual(b, datum.GetAsFloat(size_t(1)));
			Assert::AreNotSame(b, datum.GetAsFloat(size_t(1)));
			datum.Set(c, size_t(1));
			Assert::AreEqual(c, datum.GetAsFloat(size_t(1)));

			Assert::AreEqual(c, datum.GetAsFloat(size_t(2)));
			Assert::AreNotSame(c, datum.GetAsFloat(size_t(2)));
			datum.Set(a, size_t(2));
			Assert::AreEqual(a, datum.GetAsFloat(size_t(2)));

			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsFloat(size_t(3)));
			Assert::AreNotSame(a, datum.GetAsFloat(size_t(3)));
			datum.Set(b, size_t(3));
			Assert::AreEqual(b, datum.GetAsFloat(size_t(3)));
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(4)); });

			float e{ 1.3f };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsFloat());
			Assert::AreSame(e, datum.GetAsFloat());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsFloat());
			Assert::AreSame(e, datum.GetAsFloat());
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });
		}

		TEST_METHOD(SetString) {
			const string a("10");
			const string b("10");
			const string c("20");

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());

			Assert::AreEqual(a, datum.GetAsString());
			Assert::AreNotSame(a, datum.GetAsString());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsString());

			Assert::AreEqual(b, datum.GetAsString(size_t(1)));
			Assert::AreNotSame(b, datum.GetAsString(size_t(1)));
			datum.Set(c, size_t(1));
			Assert::AreEqual(c, datum.GetAsString(size_t(1)));

			Assert::AreEqual(c, datum.GetAsString(size_t(2)));
			Assert::AreNotSame(c, datum.GetAsString(size_t(2)));
			datum.Set(a, size_t(2));
			Assert::AreEqual(a, datum.GetAsString(size_t(2)));

			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsString(size_t(3)));
			Assert::AreNotSame(a, datum.GetAsString(size_t(3)));
			datum.Set(b, size_t(3));
			Assert::AreEqual(b, datum.GetAsString(size_t(3)));
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(4)); });

			string e{ "13" };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsString());
			Assert::AreSame(e, datum.GetAsString());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsString());
			Assert::AreSame(e, datum.GetAsString());
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });
		}

		TEST_METHOD(SetVector) {
			const vec4 a(1.0f);
			const vec4 b(1.0f);
			const vec4 c(2.0f);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());

			Assert::AreEqual(a, datum.GetAsVector());
			Assert::AreNotSame(a, datum.GetAsVector());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsVector());

			Assert::AreEqual(b, datum.GetAsVector(size_t(1)));
			Assert::AreNotSame(b, datum.GetAsVector(size_t(1)));
			datum.Set(c, size_t(1));
			Assert::AreEqual(c, datum.GetAsVector(size_t(1)));

			Assert::AreEqual(c, datum.GetAsVector(size_t(2)));
			Assert::AreNotSame(c, datum.GetAsVector(size_t(2)));
			datum.Set(a, size_t(2));
			Assert::AreEqual(a, datum.GetAsVector(size_t(2)));

			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(mat4(1)); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsVector(size_t(3)));
			Assert::AreNotSame(a, datum.GetAsVector(size_t(3)));
			datum.Set(b, size_t(3));
			Assert::AreEqual(b, datum.GetAsVector(size_t(3)));
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(4)); });

			vec4 e{ 1.3f };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsVector());
			Assert::AreSame(e, datum.GetAsVector());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsVector());
			Assert::AreSame(e, datum.GetAsVector());
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(mat4(1)); });
		}

		TEST_METHOD(SetMatrix) {
			const mat4 a(1.0f);
			const mat4 b(1.0f);
			const mat4 c(2.0f);

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());

			Assert::AreEqual(a, datum.GetAsMatrix());
			Assert::AreNotSame(a, datum.GetAsMatrix());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsMatrix());

			Assert::AreEqual(b, datum.GetAsMatrix(size_t(1)));
			Assert::AreNotSame(b, datum.GetAsMatrix(size_t(1)));
			datum.Set(c, size_t(1));
			Assert::AreEqual(c, datum.GetAsMatrix(size_t(1)));

			Assert::AreEqual(c, datum.GetAsMatrix(size_t(2)));
			Assert::AreNotSame(c, datum.GetAsMatrix(size_t(2)));
			datum.Set(a, size_t(2));
			Assert::AreEqual(a, datum.GetAsMatrix(size_t(2)));

			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsMatrix(size_t(3)));
			Assert::AreNotSame(a, datum.GetAsMatrix(size_t(3)));
			datum.Set(b, size_t(3));
			Assert::AreEqual(b, datum.GetAsMatrix(size_t(3)));
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(4)); });

			mat4 e{ 1.3f };
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsMatrix());
			Assert::AreSame(e, datum.GetAsMatrix());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsMatrix());
			Assert::AreSame(e, datum.GetAsMatrix());
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });
		}

		TEST_METHOD(SetPointer) {
			Foo aFoo(10);
			Foo bFoo(10);
			Foo cFoo(20);
			RTTI* a = &aFoo;
			RTTI* b = &bFoo;
			RTTI* c = &cFoo;

			Datum datum;
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a); });

			datum = { a, b, c };
			Assert::AreEqual(size_t(3), datum.Size());

			Assert::AreEqual(a, datum.GetAsPointer());
			Assert::AreNotSame(a, datum.GetAsPointer());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsPointer());

			Assert::AreEqual(b, datum.GetAsPointer(size_t(1)));
			Assert::AreNotSame(b, datum.GetAsPointer(size_t(1)));
			datum.Set(c, size_t(1));
			Assert::AreEqual(c, datum.GetAsPointer(size_t(1)));

			Assert::AreEqual(c, datum.GetAsPointer(size_t(2)));
			Assert::AreNotSame(c, datum.GetAsPointer(size_t(2)));
			datum.Set(a, size_t(2));
			Assert::AreEqual(a, datum.GetAsPointer(size_t(2)));

			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });

			datum.Reserve(50);
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });

			datum.PushBack(a);
			Assert::AreEqual(a, datum.GetAsPointer(size_t(3)));
			Assert::AreNotSame(a, datum.GetAsPointer(size_t(3)));
			datum.Set(b, size_t(3));
			Assert::AreEqual(b, datum.GetAsPointer(size_t(3)));
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(4)); });

			Foo eFoo{ 13 };
			RTTI* e = &eFoo;
			datum.SetStorage(&e, 1);
			Assert::AreEqual(e, datum.GetAsPointer());
			Assert::AreSame(e, datum.GetAsPointer());
			datum.Set(c);
			Assert::AreEqual(c, datum.GetAsPointer());
			Assert::AreSame(e, datum.GetAsPointer());
			Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Set(a, size_t(3)); });
			Assert::ExpectException<std::runtime_error>([&datum]() {datum.Set(vec4(1)); });
		}

		TEST_METHOD(PushBack)
		{
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const int a{ 10 };
				const int b{ 20 };
				const int c{ 30 };

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreNotSame(a, datum.FrontAsInt());
				Assert::AreEqual(a, datum.BackAsInt());
				Assert::AreNotSame(a, datum.BackAsInt());

				datum.PushBack(b);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreNotSame(a, datum.FrontAsInt());
				Assert::AreEqual(b, datum.BackAsInt());
				Assert::AreNotSame(b, datum.BackAsInt());

				datum.PushBack(c);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreNotSame(a, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreNotSame(c, datum.BackAsInt());

				datum.Clear();
				datum.ShrinkToFit();
				datum.PushBack(a);

				int e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::ExpectException<runtime_error>([&datum, &e] {datum.PushBack(e); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const float a{ 1.0f };
				const float b{ 2.0f };
				const float c{ 3.0f };

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreNotSame(a, datum.FrontAsFloat());
				Assert::AreEqual(a, datum.BackAsFloat());
				Assert::AreNotSame(a, datum.BackAsFloat());

				datum.PushBack(b);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreNotSame(a, datum.FrontAsFloat());
				Assert::AreEqual(b, datum.BackAsFloat());
				Assert::AreNotSame(b, datum.BackAsFloat());

				datum.PushBack(c);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreNotSame(a, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreNotSame(c, datum.BackAsFloat());

				datum.Clear();
				datum.ShrinkToFit();
				datum.PushBack(a);

				float e{ 1.3f };
				datum.SetStorage(&e, 1);
				Assert::ExpectException<runtime_error>([&datum, &e] {datum.PushBack(e); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const string a{ "10" };
				const string b{ "20" };
				const string c{ "30" };

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreNotSame(a, datum.FrontAsString());
				Assert::AreEqual(a, datum.BackAsString());
				Assert::AreNotSame(a, datum.BackAsString());

				datum.PushBack(b);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreNotSame(a, datum.FrontAsString());
				Assert::AreEqual(b, datum.BackAsString());
				Assert::AreNotSame(b, datum.BackAsString());

				datum.PushBack(c);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreNotSame(a, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreNotSame(c, datum.BackAsString());

				datum.Clear();
				datum.ShrinkToFit();
				datum.PushBack(a);

				string e{ "13" };
				datum.SetStorage(&e, 1);
				Assert::ExpectException<runtime_error>([&datum, &e] {datum.PushBack(e); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const vec4 a{ 10 };
				const vec4 b{ 20 };
				const vec4 c{ 30 };

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreNotSame(a, datum.FrontAsVector());
				Assert::AreEqual(a, datum.BackAsVector());
				Assert::AreNotSame(a, datum.BackAsVector());

				datum.PushBack(b);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreNotSame(a, datum.FrontAsVector());
				Assert::AreEqual(b, datum.BackAsVector());
				Assert::AreNotSame(b, datum.BackAsVector());

				datum.PushBack(c);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreNotSame(a, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreNotSame(c, datum.BackAsVector());

				datum.Clear();
				datum.ShrinkToFit();
				datum.PushBack(a);

				vec4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::ExpectException<runtime_error>([&datum, &e] {datum.PushBack(e); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const mat4 a{ 10 };
				const mat4 b{ 20 };
				const mat4 c{ 30 };

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreNotSame(a, datum.FrontAsMatrix());
				Assert::AreEqual(a, datum.BackAsMatrix());
				Assert::AreNotSame(a, datum.BackAsMatrix());

				datum.PushBack(b);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreNotSame(a, datum.FrontAsMatrix());
				Assert::AreEqual(b, datum.BackAsMatrix());
				Assert::AreNotSame(b, datum.BackAsMatrix());

				datum.PushBack(c);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreNotSame(a, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreNotSame(c, datum.BackAsMatrix());

				datum.Clear();
				datum.ShrinkToFit();
				datum.PushBack(a);

				mat4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::ExpectException<runtime_error>([&datum, &e] {datum.PushBack(e); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				Foo aFoo{ 10 };
				Foo bFoo{ 20 };
				Foo cFoo{ 30 };
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;

				datum.PushBack(a);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreNotSame(a, datum.FrontAsPointer());
				Assert::AreEqual(a, datum.BackAsPointer());
				Assert::AreNotSame(a, datum.BackAsPointer());

				datum.PushBack(b);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreNotSame(a, datum.FrontAsPointer());
				Assert::AreEqual(b, datum.BackAsPointer());
				Assert::AreNotSame(b, datum.BackAsPointer());

				datum.PushBack(c);
				Assert::AreEqual(size_t(3), datum.Size());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreNotSame(a, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreNotSame(c, datum.BackAsPointer());

				datum.Clear();
				datum.ShrinkToFit();
				datum.PushBack(a);

				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				datum.SetStorage(&e, 1);
				Assert::ExpectException<runtime_error>([&datum, &e] {datum.PushBack(e); });
			}
		}

		TEST_METHOD(PopBack) 
		{
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const int a{ 10 };
				const int b{ 20 };
				const int c{ 30 };
				datum.PushBack(c);
				datum.PushBack(b);
				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Size());

				std::size_t originalCapacity = datum.Capacity();

				datum.PopBack();
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsInt());
				Assert::AreNotSame(c, datum.FrontAsInt());
				Assert::AreEqual(b, datum.BackAsInt());
				Assert::AreNotSame(b, datum.BackAsInt());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsInt());
				Assert::AreNotSame(c, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreNotSame(c, datum.BackAsInt());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsInt(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsInt(); UNREFERENCED_LOCAL(back); });

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });

				int e{ 13 };
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const float a{ 1.0f };
				const float b{ 2.0f };
				const float c{ 3.0f };
				datum.PushBack(c);
				datum.PushBack(b);
				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Size());

				std::size_t originalCapacity = datum.Capacity();

				datum.PopBack();
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsFloat());
				Assert::AreNotSame(c, datum.FrontAsFloat());
				Assert::AreEqual(b, datum.BackAsFloat());
				Assert::AreNotSame(b, datum.BackAsFloat());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsFloat());
				Assert::AreNotSame(c, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreNotSame(c, datum.BackAsFloat());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsFloat(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsFloat(); UNREFERENCED_LOCAL(back); });

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });

				float e{ 1.3f };
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const string a{ "owo" };
				const string b{ "uwu" };
				const string c{ "ewe" };
				datum.PushBack(c);
				datum.PushBack(b);
				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Size());

				std::size_t originalCapacity = datum.Capacity();

				datum.PopBack();
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsString());
				Assert::AreNotSame(c, datum.FrontAsString());
				Assert::AreEqual(b, datum.BackAsString());
				Assert::AreNotSame(b, datum.BackAsString());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsString());
				Assert::AreNotSame(c, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreNotSame(c, datum.BackAsString());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsString(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsString(); UNREFERENCED_LOCAL(back); });

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });

				string e{ "heyo"};
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const vec4 a{ 10 };
				const vec4 b{ 20 };
				const vec4 c{ 30 };
				datum.PushBack(c);
				datum.PushBack(b);
				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Size());

				std::size_t originalCapacity = datum.Capacity();

				datum.PopBack();
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsVector());
				Assert::AreNotSame(c, datum.FrontAsVector());
				Assert::AreEqual(b, datum.BackAsVector());
				Assert::AreNotSame(b, datum.BackAsVector());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsVector());
				Assert::AreNotSame(c, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreNotSame(c, datum.BackAsVector());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsVector(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsVector(); UNREFERENCED_LOCAL(back); });

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });

				vec4 e{ 13 };
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				const mat4 a{ 10 };
				const mat4 b{ 20 };
				const mat4 c{ 30 };
				datum.PushBack(c);
				datum.PushBack(b);
				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Size());

				std::size_t originalCapacity = datum.Capacity();

				datum.PopBack();
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsMatrix());
				Assert::AreNotSame(c, datum.FrontAsMatrix());
				Assert::AreEqual(b, datum.BackAsMatrix());
				Assert::AreNotSame(b, datum.BackAsMatrix());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsMatrix());
				Assert::AreNotSame(c, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreNotSame(c, datum.BackAsMatrix());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsMatrix(); UNREFERENCED_LOCAL(back); });

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });

				mat4 e{ 13 };
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());

				Foo aFoo{ 10 };
				Foo bFoo{ 20 };
				Foo cFoo{ 30 };
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;
				datum.PushBack(c);
				datum.PushBack(b);
				datum.PushBack(a);
				Assert::AreEqual(size_t(3), datum.Size());

				std::size_t originalCapacity = datum.Capacity();

				datum.PopBack();
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsPointer());
				Assert::AreNotSame(c, datum.FrontAsPointer());
				Assert::AreEqual(b, datum.BackAsPointer());
				Assert::AreNotSame(b, datum.BackAsPointer());

				datum.PopBack();
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(c, datum.FrontAsPointer());
				Assert::AreNotSame(c, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreNotSame(c, datum.BackAsPointer());

				datum.PopBack();
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsPointer(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsPointer(); UNREFERENCED_LOCAL(back); });

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });

				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(size_t(1), datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.PopBack(); });
			}
		}

		TEST_METHOD(RemoveByValueInt)
		{
			const int a(10);
			const int b(20);
			const int c(30);
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreEqual(b, datum.BackAsInt());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsInt(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsInt(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsInt(), datum.BackAsInt());
				Assert::AreEqual(b, datum.FrontAsInt());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.Remove(a); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				int e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum, &e]() {datum.Remove(e); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsInt());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(a);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByValueFloat)
		{
			const float a(1.0f);
			const float b(2.0f);
			const float c(3.0f);
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreEqual(b, datum.BackAsFloat());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsFloat(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsFloat(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsFloat(), datum.BackAsFloat());
				Assert::AreEqual(b, datum.FrontAsFloat());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Remove(a); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				float e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum, &e]() {datum.Remove(e); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsFloat());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(a);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByValueString)
		{
			const string a("owo");
			const string b("uwu");
			const string c("uvu");
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreEqual(b, datum.BackAsString());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsString(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsString(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsString(), datum.BackAsString());
				Assert::AreEqual(b, datum.FrontAsString());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Remove(a); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				string e{ "xvx" };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum, &e]() {datum.Remove(e); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsString());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(a);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByValueVector)
		{
			const vec4 a{ 10 };
			const vec4 b{ 20 };
			const vec4 c{ 30 };
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreEqual(b, datum.BackAsVector());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsVector(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsVector(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsVector(), datum.BackAsVector());
				Assert::AreEqual(b, datum.FrontAsVector());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Remove(a); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				vec4 e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum, &e]() {datum.Remove(e); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsVector());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(a);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByValueMatrix)
		{
			const mat4 a{ 10 };
			const mat4 b{ 20 };
			const mat4 c{ 30 };
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreEqual(b, datum.BackAsMatrix());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsMatrix(), datum.BackAsMatrix());
				Assert::AreEqual(b, datum.FrontAsMatrix());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Remove(a); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				mat4 e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum, &e]() {datum.Remove(e); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsMatrix());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(a);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByValuePointer)
		{
			Foo aFoo(10);
			Foo bFoo(20);
			Foo cFoo(30);
			Foo zFoo(10);
			RTTI* a = &aFoo;
			RTTI* b = &bFoo;
			RTTI* c = &cFoo;
			RTTI* z = &zFoo;
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(c);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreEqual(b, datum.BackAsPointer());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsPointer(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsPointer(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsPointer(), datum.BackAsPointer());
				Assert::AreEqual(b, datum.FrontAsPointer());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum, &a]() {datum.Remove(a); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);

				removed = datum.Remove(z);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsTrue(removed);
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
				Assert::AreEqual(datum.Size(), datum.FindIndex(z));
			}
			{
				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum, &e]() {datum.Remove(e); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.Remove(a);
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsPointer());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(b);
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.Remove(a);
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByPositionInt)
		{
			const int a(10);
			const int b(20);
			const int c(30);
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();
				
				bool removed = datum.RemoveAt(size_t(1));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreEqual(c, datum.BackAsInt());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(2));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsInt());
				Assert::AreEqual(b, datum.BackAsInt());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsInt(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsInt(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsInt(), datum.BackAsInt());
				Assert::AreEqual(b, datum.FrontAsInt());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(10));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				int e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsInt());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByPositionFloat)
		{
			const float a(1.0f);
			const float b(2.0f);
			const float c(3.0f);
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(1));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreEqual(c, datum.BackAsFloat());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(2));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsFloat());
				Assert::AreEqual(b, datum.BackAsFloat());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsFloat(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsFloat(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsFloat(), datum.BackAsFloat());
				Assert::AreEqual(b, datum.FrontAsFloat());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(10));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				float e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsFloat());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByPositionString)
		{
			const string a("owo");
			const string b("uwu");
			const string c("uvu");
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(1));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreEqual(c, datum.BackAsString());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(2));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsString());
				Assert::AreEqual(b, datum.BackAsString());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsString(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsString(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsString(), datum.BackAsString());
				Assert::AreEqual(b, datum.FrontAsString());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(10));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				string e{ "xvx" };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsString());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByPositionVector)
		{
			const vec4 a{ 10 };
			const vec4 b{20};
			const vec4 c{30};
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(1));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreEqual(c, datum.BackAsVector());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(2));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsVector());
				Assert::AreEqual(b, datum.BackAsVector());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsVector(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsVector(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsVector(), datum.BackAsVector());
				Assert::AreEqual(b, datum.FrontAsVector());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(10));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				vec4 e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsVector());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByPositionMatrix)
		{
			const mat4 a{ 10 };
			const mat4 b{ 20 };
			const mat4 c{ 30 };
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(1));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreEqual(c, datum.BackAsMatrix());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(2));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsMatrix());
				Assert::AreEqual(b, datum.BackAsMatrix());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsMatrix(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsMatrix(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsMatrix(), datum.BackAsMatrix());
				Assert::AreEqual(b, datum.FrontAsMatrix());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(10));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				mat4 e{ 13 };
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsMatrix());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(RemoveByPositionPointer)
		{
			Foo aFoo(10);
			Foo bFoo(20);
			Foo cFoo(30);
			RTTI* a = &aFoo;
			RTTI* b = &bFoo;
			RTTI* c = &cFoo;
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(b, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(1));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreEqual(c, datum.BackAsPointer());
				Assert::AreEqual(datum.Size(), datum.FindIndex(b));
			}
			{
				Datum datum{ a, b, c };
				Assert::AreEqual(size_t(3), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(2));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(2), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::AreEqual(b, datum.BackAsPointer());
				Assert::AreEqual(datum.Size(), datum.FindIndex(c));
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& front = datum.FrontAsPointer(); UNREFERENCED_LOCAL(front); });
				Assert::ExpectException<std::runtime_error>([&datum]() {auto& back = datum.BackAsPointer(); UNREFERENCED_LOCAL(back); });
				Assert::AreEqual(datum.Size(), datum.FindIndex(a));

				datum.PushBack(b);
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::AreEqual(datum.FrontAsPointer(), datum.BackAsPointer());
				Assert::AreEqual(b, datum.FrontAsPointer());

				datum.PushBack(c);
				Datum anotherDatum{ b, c };
				Assert::IsTrue(datum == anotherDatum);
			}
			{
				Datum datum;
				Assert::AreEqual(size_t(0), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
			{
				Datum datum{ a };
				Assert::AreEqual(size_t(1), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(10));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
				Assert::IsFalse(removed);
			}
			{
				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				Datum datum;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::ExpectException<std::runtime_error>([&datum]() {datum.RemoveAt(size_t(0)); });
			}
			{
				Datum datum{ a, b };
				Assert::AreEqual(size_t(2), datum.Size());
				std::size_t originalCapacity = datum.Capacity();

				bool removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(b, datum.FrontAsPointer());
				Assert::AreEqual(size_t(1), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsTrue(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());

				removed = datum.RemoveAt(size_t(0));
				Assert::IsFalse(removed);
				Assert::AreEqual(size_t(0), datum.Size());
				Assert::AreEqual(originalCapacity, datum.Capacity());
			}
		}

		TEST_METHOD(FindIndex) {
			{
				const int a(10);
				const int b(20);
				const int c(30);
				{
					Datum datum{ a, b, c };

					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(a, datum.GetAsInt(ind));

					ind = datum.FindIndex(b);
					Assert::AreEqual(size_t(1), ind);
					Assert::AreEqual(b, datum.GetAsInt(ind));

					ind = datum.FindIndex(c);
					Assert::AreEqual(size_t(2), ind);
					Assert::AreEqual(c, datum.GetAsInt(ind));

					ind = datum.FindIndex(13);
					Assert::AreEqual(datum.Size(), ind);

					Assert::ExpectException<std::runtime_error>([&datum, &ind]() {ind = datum.FindIndex(vec4(1.0f)); });
				}
				{
					Datum datum;
					Assert::ExpectException<std::runtime_error>([&datum]() {auto ind = datum.FindIndex(vec4(1.0f)); UNREFERENCED_LOCAL(ind); });

					datum.SetType(Datum::DatumTypes::Integer);
					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(datum.Size(), ind);
				}
				{
					int e = 13;
					Datum datum;
					datum.SetStorage(&e, size_t(1));

					size_t ind = datum.FindIndex(e);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(e, datum.GetAsInt(ind));
				}
			}
			{
				const float a(1.0f);
				const float b(2.0f);
				const float c(3.0f);
				{
					Datum datum{ a, b, c };

					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(a, datum.GetAsFloat(ind));

					ind = datum.FindIndex(b);
					Assert::AreEqual(size_t(1), ind);
					Assert::AreEqual(b, datum.GetAsFloat(ind));

					ind = datum.FindIndex(c);
					Assert::AreEqual(size_t(2), ind);
					Assert::AreEqual(c, datum.GetAsFloat(ind));

					ind = datum.FindIndex(1.3f);
					Assert::AreEqual(datum.Size(), ind);

					Assert::ExpectException<std::runtime_error>([&datum, &ind]() {ind = datum.FindIndex(vec4(1.0f)); });
				}
				{
					Datum datum;
					Assert::ExpectException<std::runtime_error>([&datum]() {auto ind = datum.FindIndex(vec4(1.0f)); UNREFERENCED_LOCAL(ind); });

					datum.SetType(Datum::DatumTypes::Float);
					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(datum.Size(), ind);
				}
				{
					float e = 1.3f;
					Datum datum;
					datum.SetStorage(&e, size_t(1));

					size_t ind = datum.FindIndex(e);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(e, datum.GetAsFloat(ind));
				}
			}
			{
				const std::string a("owo");
				const std::string b("uwu");
				const std::string c("uvu");
				{
					Datum datum{ a, b, c };

					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(a, datum.GetAsString(ind));

					ind = datum.FindIndex(b);
					Assert::AreEqual(size_t(1), ind);
					Assert::AreEqual(b, datum.GetAsString(ind));

					ind = datum.FindIndex(c);
					Assert::AreEqual(size_t(2), ind);
					Assert::AreEqual(c, datum.GetAsString(ind));

					ind = datum.FindIndex("heyo");
					Assert::AreEqual(datum.Size(), ind);

					Assert::ExpectException<std::runtime_error>([&datum, &ind]() {ind = datum.FindIndex(vec4(1.0f)); });
				}
				{
					Datum datum;
					Assert::ExpectException<std::runtime_error>([&datum]() {auto ind = datum.FindIndex(vec4(1.0f)); UNREFERENCED_LOCAL(ind); });

					datum.SetType(Datum::DatumTypes::String);
					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(datum.Size(), ind);
				}
				{
					string e = "ewe";
					Datum datum;
					datum.SetStorage(&e, size_t(1));

					size_t ind = datum.FindIndex(e);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(e, datum.GetAsString(ind));
				}
			}
			{
				const glm::vec4 a(1.0f);
				const glm::vec4 b(2.0f);
				const glm::vec4 c(3.0f);
				{
					Datum datum{ a, b, c };

					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(a, datum.GetAsVector(ind));

					ind = datum.FindIndex(b);
					Assert::AreEqual(size_t(1), ind);
					Assert::AreEqual(b, datum.GetAsVector(ind));

					ind = datum.FindIndex(c);
					Assert::AreEqual(size_t(2), ind);
					Assert::AreEqual(c, datum.GetAsVector(ind));

					ind = datum.FindIndex(vec4(13));
					Assert::AreEqual(datum.Size(), ind);

					Assert::ExpectException<std::runtime_error>([&datum, &ind]() {ind = datum.FindIndex(1.0f); });
				}
				{
					Datum datum;
					Assert::ExpectException<std::runtime_error>([&datum]() {auto ind = datum.FindIndex(1.0f); UNREFERENCED_LOCAL(ind); });

					datum.SetType(Datum::DatumTypes::Vector);
					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(datum.Size(), ind);
				}
				{
					vec4 e(13);
					Datum datum;
					datum.SetStorage(&e, size_t(1));

					size_t ind = datum.FindIndex(e);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(e, datum.GetAsVector(ind));
				}
			}
			{
				const glm::mat4 a(1.0f);
				const glm::mat4 b(2.0f);
				const glm::mat4 c(3.0f);
				{
					Datum datum{ a, b, c };

					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(a, datum.GetAsMatrix(ind));

					ind = datum.FindIndex(b);
					Assert::AreEqual(size_t(1), ind);
					Assert::AreEqual(b, datum.GetAsMatrix(ind));

					ind = datum.FindIndex(c);
					Assert::AreEqual(size_t(2), ind);
					Assert::AreEqual(c, datum.GetAsMatrix(ind));

					ind = datum.FindIndex(mat4(13));
					Assert::AreEqual(datum.Size(), ind);

					Assert::ExpectException<std::runtime_error>([&datum, &ind]() {ind = datum.FindIndex(1.0f); });
				}
				{
					Datum datum;
					Assert::ExpectException<std::runtime_error>([&datum]() {auto ind = datum.FindIndex(1.0f); UNREFERENCED_LOCAL(ind); });

					datum.SetType(Datum::DatumTypes::Matrix);
					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(datum.Size(), ind);
				}
				{
					mat4 e(13);
					Datum datum;
					datum.SetStorage(&e, size_t(1));

					size_t ind = datum.FindIndex(e);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(e, datum.GetAsMatrix(ind));
				}
			}
			{
				Foo aFoo(10);
				Foo bFoo(20);
				Foo cFoo(30);
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;
				{
					Datum datum{ a, b, c };

					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(a, datum.GetAsPointer(ind));

					ind = datum.FindIndex(b);
					Assert::AreEqual(size_t(1), ind);
					Assert::AreEqual(b, datum.GetAsPointer(ind));

					ind = datum.FindIndex(c);
					Assert::AreEqual(size_t(2), ind);
					Assert::AreEqual(c, datum.GetAsPointer(ind));

					Foo eFoo(13);
					ind = datum.FindIndex(&eFoo);
					Assert::AreEqual(datum.Size(), ind);

					Assert::ExpectException<std::runtime_error>([&datum, &ind]() {ind = datum.FindIndex(1.0f); });
				}
				{
					Datum datum;
					Assert::ExpectException<std::runtime_error>([&datum]() {auto ind = datum.FindIndex(1.0f); UNREFERENCED_LOCAL(ind); });

					datum.SetType(Datum::DatumTypes::Pointer);
					size_t ind = datum.FindIndex(a);
					Assert::AreEqual(datum.Size(), ind);
				}
				{
					Foo eFoo(13);
					RTTI* e = &eFoo;
					Datum datum;
					datum.SetStorage(&e, size_t(1));

					size_t ind = datum.FindIndex(e);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(e, datum.GetAsPointer(ind));
				}
			}
			{
				Scope parentScope;
				Scope& scope = parentScope.AppendScope("Child");
				Scope& scopeA = scope.AppendScope("A");
				Scope& scopeB = scope.AppendScope("B");
				Scope& scopeC = scope.AppendScope("C");
				Scope& scopeD = scope.AppendScope("D");
				auto catchOrphan = scopeD.Orphan();
				{
					Datum datum = scope[0];

					size_t ind = datum.FindIndex(scopeA);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(scopeA, datum.GetAsTable(ind));
					ind = datum.FindIndex(scopeB);
					Assert::AreEqual(datum.Size(), ind);

					datum = scope[1];
					ind = datum.FindIndex(scopeB);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(scopeB, datum.GetAsTable(ind));

					datum = scope[2];
					ind = datum.FindIndex(scopeC);
					Assert::AreEqual(size_t(0), ind);
					Assert::AreEqual(scopeC, datum.GetAsTable(ind));

					ind = datum.FindIndex(*catchOrphan);
					Assert::AreEqual(datum.Size(), ind);

					Datum emptyDatum;
					emptyDatum.SetType(Datum::DatumTypes::Integer);
					Assert::ExpectException<std::runtime_error>([&emptyDatum, &ind, &scope]() {ind = emptyDatum.FindIndex(scope); });
				}
				{
					Datum datum;
					Assert::ExpectException<std::runtime_error>([&datum]() {auto ind = datum.FindIndex(1.0f); UNREFERENCED_LOCAL(ind); });

					datum.SetType(Datum::DatumTypes::Table);
					size_t ind = datum.FindIndex(scopeA);
					Assert::AreEqual(datum.Size(), ind);
				}
				delete catchOrphan;
			}
		}

		TEST_METHOD(SetFromString) {
			{
				Datum datum = 5;
				Assert::AreEqual(5, datum.FrontAsInt());
				datum.SetFromString("10");
				Assert::AreEqual(10, datum.FrontAsInt());

				datum.PushBack(5);
				Assert::AreEqual(5, datum.GetAsInt(size_t(1)));
				datum.SetFromString("20", size_t(1));
				Assert::AreEqual(20, datum.GetAsInt(size_t(1)));

				datum.Resize(3);
				datum.SetFromString("20", size_t(2));
				Assert::AreEqual(20, datum.GetAsInt(size_t(2)));

				Assert::ExpectException<invalid_argument>([&datum] {datum.SetFromString("hello"); });
				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("11", size_t(10)); });

				int e = 13;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(13, datum.GetAsInt());
				datum.SetFromString("20");
				Assert::AreEqual(20, datum.GetAsInt());
			}
			{
				Datum datum = 5.0f;
				Assert::AreEqual(5.0f, datum.FrontAsFloat());
				datum.SetFromString("10.000000");
				Assert::AreEqual(10.0f, datum.FrontAsFloat());

				datum.PushBack(5.0f);
				Assert::AreEqual(5.0f, datum.GetAsFloat(size_t(1)));
				datum.SetFromString("20.000000", size_t(1));
				Assert::AreEqual(20.0f, datum.GetAsFloat(size_t(1)));

				datum.Resize(3);
				datum.SetFromString("20.000000", size_t(2));
				Assert::AreEqual(20.0f, datum.GetAsFloat(size_t(2)));

				Assert::ExpectException<invalid_argument>([&datum] {datum.SetFromString("hello"); });
				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("1.100000", size_t(10)); });

				float e = 1.3f;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(1.3f, datum.GetAsFloat());
				datum.SetFromString("2.0");
				Assert::AreEqual(2.0f, datum.GetAsFloat());
			}
			{
				Datum datum = std::string("hello");
				Assert::AreEqual(std::string("hello"), datum.FrontAsString());
				datum.SetFromString("10");
				Assert::AreEqual(std::string("10"), datum.FrontAsString());

				datum.PushBack(std::string("owo"));
				Assert::AreEqual(std::string("owo"), datum.GetAsString(size_t(1)));
				datum.SetFromString(std::string("yikes"), size_t(1));
				Assert::AreEqual(std::string("yikes"), datum.GetAsString(size_t(1)));

				datum.Resize(3);
				datum.SetFromString(std::string("uwu"), size_t(2));
				Assert::AreEqual(std::string("uwu"), datum.GetAsString(size_t(2)));

				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("1.100000", size_t(10)); });

				string e("oops");
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(string("oops"), datum.GetAsString());
				datum.SetFromString("20");
				Assert::AreEqual(string("20"), datum.GetAsString());
			}
			{
				vec4 a(1);
				vec4 b(3);

				Datum datum = a;
				Assert::AreEqual(a, datum.FrontAsVector());
				datum.SetFromString("vec4(1.000000, 2.000000, 3.000000, 4.000000)");
				Assert::AreEqual(vec4(1, 2, 3, 4), datum.FrontAsVector());

				datum.PushBack(vec4(1));
				Assert::AreEqual(vec4(1), datum.GetAsVector(size_t(1)));
				datum.SetFromString(glm::to_string(b), size_t(1));
				Assert::AreEqual(b, datum.GetAsVector(size_t(1)));

				datum.Resize(3);
				datum.SetFromString("vec4(3.000000, 2.000000, 3.000000, 2.000000)", size_t(2));
				Assert::AreEqual(vec4(3, 2, 3, 2), datum.GetAsVector(size_t(2)));

				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("hello"); });
				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("vec4(1.000000, 2.000000, 3.000000, 4.000000)", size_t(10)); });

				vec4 e(13);
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(vec4(13), datum.GetAsVector());
				datum.SetFromString(glm::to_string(a));
				Assert::AreEqual(a, datum.GetAsVector());
			}
			{
				mat4 a(2);
				mat4 b(3);

				Datum datum = a;
				Assert::AreEqual(a, datum.FrontAsMatrix());
				datum.SetFromString("mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))");
				Assert::AreEqual(mat4(1), datum.FrontAsMatrix());

				datum.PushBack(mat4(1));
				Assert::AreEqual(mat4(1), datum.GetAsMatrix());
				datum.SetFromString(glm::to_string(b));
				Assert::AreEqual(b, datum.GetAsMatrix());

				datum.Resize(3);
				datum.SetFromString("mat4x4((5.000000, 0.000000, 0.000000, 0.000000), (0.000000, 5.000000, 0.000000, 0.000000), (0.000000, 0.000000, 5.000000, 0.000000), (0.000000, 0.000000, 0.000000, 5.000000))", size_t(2));
				Assert::AreEqual(mat4(5), datum.GetAsMatrix(size_t(2)));

				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("hello"); });
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetFromString(glm::to_string(a), size_t(10)); });

				mat4 e(13);
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(mat4(13), datum.GetAsMatrix());
				datum.SetFromString(glm::to_string(a));
				Assert::AreEqual(a, datum.GetAsMatrix());
			}
			{
				Foo aFoo(10);
				RTTI* a = &aFoo;

				Datum datum = a;
				Assert::AreEqual(a, datum.FrontAsPointer());
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetFromString(a->ToString()); });
				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("hello"); });
				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("11", size_t(10)); });

				Foo eFoo(13);
				RTTI* e = &eFoo;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(e, datum.FrontAsPointer());
				Assert::ExpectException<runtime_error>([&datum, &a] {datum.SetFromString(a->ToString()); });
				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("hello"); });
				Assert::ExpectException<runtime_error>([&datum] {datum.SetFromString("11", size_t(10)); });
			}
		}

		TEST_METHOD(ToString) {
			{
				Datum datum = 10;
				Assert::AreEqual(std::string("10"), datum.ToString());

				datum.PushBack(20);
				datum.PushBack(30);
				Assert::AreEqual(std::string("20"), datum.ToString(size_t(1)));
				Assert::AreEqual(std::string("30"), datum.ToString(size_t(2)));
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });

				int e = 13;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(std::string("13"), datum.ToString());
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });
			}
			{
				Datum datum = 1.0f;
				Assert::AreEqual(std::string("1.000000"), datum.ToString());

				datum.PushBack(2.0f);
				datum.PushBack(3.0f);
				Assert::AreEqual(std::string("2.000000"), datum.ToString(size_t(1)));
				Assert::AreEqual(std::string("3.000000"), datum.ToString(size_t(2)));
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });

				float e = 1.3f;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(std::string("1.300000"), datum.ToString());
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });
			}
			{
				Datum datum = std::string("hello");
				Assert::AreEqual(std::string("hello"), datum.ToString());

				datum.PushBack(std::string("owo"));
				datum.PushBack(std::string("uwu"));
				Assert::AreEqual(std::string("owo"), datum.ToString(size_t(1)));
				Assert::AreEqual(std::string("uwu"), datum.ToString(size_t(2)));
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });

				std::string e("heyo");
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(std::string("heyo"), datum.ToString());
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });
			}
			{
				Datum datum = vec4(1.0f, 2.0f, 3.0f, 4.0f);
				Assert::AreEqual(std::string("vec4(1.000000, 2.000000, 3.000000, 4.000000)"), datum.ToString());

				datum.PushBack(vec4(10.0f, 20.0f, 30.0f, 40.0f));
				datum.PushBack(vec4(2.0f, 2.0f, 2.0f, 2.0f));
				Assert::AreEqual(std::string("vec4(10.000000, 20.000000, 30.000000, 40.000000)"), datum.ToString(size_t(1)));
				Assert::AreEqual(std::string("vec4(2.000000, 2.000000, 2.000000, 2.000000)"), datum.ToString(size_t(2)));
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });

				vec4 e(8.0f);
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(std::string("vec4(8.000000, 8.000000, 8.000000, 8.000000)"), datum.ToString());
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });
			}
			{
				Datum datum = mat4(1.0f);
				Assert::AreEqual(std::string("mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))"), datum.ToString());

				datum.PushBack(mat4(2.0f));
				datum.PushBack(mat4(5.0f));
				Assert::AreEqual(std::string("mat4x4((2.000000, 0.000000, 0.000000, 0.000000), (0.000000, 2.000000, 0.000000, 0.000000), (0.000000, 0.000000, 2.000000, 0.000000), (0.000000, 0.000000, 0.000000, 2.000000))"), datum.ToString(size_t(1)));
				Assert::AreEqual(std::string("mat4x4((5.000000, 0.000000, 0.000000, 0.000000), (0.000000, 5.000000, 0.000000, 0.000000), (0.000000, 0.000000, 5.000000, 0.000000), (0.000000, 0.000000, 0.000000, 5.000000))"), datum.ToString(size_t(2)));
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });

				mat4 e(8.0f);
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(std::string("mat4x4((8.000000, 0.000000, 0.000000, 0.000000), (0.000000, 8.000000, 0.000000, 0.000000), (0.000000, 0.000000, 8.000000, 0.000000), (0.000000, 0.000000, 0.000000, 8.000000))"), datum.ToString());
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });
			}
			{
				Foo aFoo(10);
				Foo bFoo(20);
				Foo cFoo(30);
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;
				Datum datum = a;
				Assert::AreEqual(a->ToString(), datum.ToString());

				datum.PushBack(b);
				datum.PushBack(c);
				Assert::AreEqual(b->ToString(), datum.ToString(size_t(1)));
				Assert::AreEqual(c->ToString(), datum.ToString(size_t(2)));
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });

				RTTI* nullTest = nullptr;
				datum.PushBack(nullTest);
				Assert::AreEqual(string("nullptr"), datum.ToString(size_t(3)));

				Foo eFoo(15);
				RTTI* e = &eFoo;
				datum.SetStorage(&e, size_t(1));
				Assert::AreEqual(e->ToString(), datum.ToString());
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(size_t(10)); });
			}
			{
				Datum datum;
				Assert::ExpectException<runtime_error>([&datum] {datum.ToString(); });
			}
			{
				Scope scope;
				scope.AppendScope("child").Append("key") = 10;

				Datum datum = scope[0];
				Assert::AreEqual(std::string("key: (10) "), datum.ToString());
			}
		}

		TEST_METHOD(EqualityDatumToDatum) {
			{
				Datum datum;
				Datum anotherDatum;
				Assert::AreEqual(datum, datum);
				Assert::AreEqual(datum, anotherDatum);
				Assert::AreEqual(anotherDatum, anotherDatum);
			}
			{
				Datum datum = 10;
				Datum anotherDatum = 10;
				Datum differentDatum = 20;
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);

				int a = 10;
				anotherDatum.SetStorage(&a, 1);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);
			}
			{
				Datum datum = 10.0f;
				Datum anotherDatum = 10.0f;
				Datum differentDatum = 20.f;
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);

				float a = 10.0f;
				anotherDatum.SetStorage(&a, 1);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);
			}
			{
				Datum datum = glm::vec4(1);
				Datum anotherDatum = glm::vec4(1);
				Datum differentDatum = glm::vec4(5);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);

				vec4 a(1);
				anotherDatum.SetStorage(&a, 1);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);
			}
			{
				Datum datum = glm::mat4(1);
				Datum anotherDatum = glm::mat4(1);
				Datum differentDatum = glm::mat4(5);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);

				mat4 a(1);
				anotherDatum.SetStorage(&a, 1);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);
			}
			{
				Datum datum = "a"s;
				Datum anotherDatum = "a"s;
				Datum differentDatum = "owo"s;
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);

				string a = "a"s;
				anotherDatum.SetStorage(&a, 1);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);
			}
			{
				Foo a(10);
				Foo b(10);
				Foo c(13);
				RTTI* ap = &a;
				RTTI* bp = &b;
				RTTI* cp = &c;
				Datum datum = ap;
				Datum anotherDatum = bp;
				Datum differentDatum = cp;
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);

				anotherDatum.SetStorage(&ap, 1);
				Assert::AreEqual(datum, anotherDatum);
				Assert::IsTrue(datum != differentDatum);
			}
		}

		TEST_METHOD(EqualityDatumToScalar) {
			{
				const int a = 10;
				const int b = 20;
				const int c = 10;
				Datum datum = a;
				Assert::IsTrue(a == datum);
				Assert::IsTrue(datum == a);
				Assert::IsTrue(c == datum);
				Assert::IsTrue(datum == c);
				Assert::IsFalse(b == datum);
				Assert::IsFalse(datum == b);

				Assert::IsFalse(a != datum);
				Assert::IsFalse(datum != a);
				Assert::IsFalse(c != datum);
				Assert::IsFalse(datum != c);
				Assert::IsTrue(b != datum);
				Assert::IsTrue(datum != b);

				int e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::IsTrue(e == datum);
				Assert::IsTrue(datum == e);
				Assert::IsFalse(a == datum);
				Assert::IsFalse(datum == a);

				Assert::IsFalse(e != datum);
				Assert::IsFalse(datum != e);
				Assert::IsTrue(a != datum);
				Assert::IsTrue(datum != a);
			}
			{
				const float a = 1.0f;
				const float b = 2.0f;
				const float c = 1.0f;
				Datum datum = a;
				Assert::IsTrue(a == datum);
				Assert::IsTrue(datum == a);
				Assert::IsTrue(c == datum);
				Assert::IsTrue(datum == c);
				Assert::IsFalse(b == datum);
				Assert::IsFalse(datum == b);

				Assert::IsFalse(a != datum);
				Assert::IsFalse(datum != a);
				Assert::IsFalse(c != datum);
				Assert::IsFalse(datum != c);
				Assert::IsTrue(b != datum);
				Assert::IsTrue(datum != b);

				float e{ 1.3f };
				datum.SetStorage(&e, 1);
				Assert::IsTrue(e == datum);
				Assert::IsTrue(datum == e);
				Assert::IsFalse(a == datum);
				Assert::IsFalse(datum == a);

				Assert::IsFalse(e != datum);
				Assert::IsFalse(datum != e);
				Assert::IsTrue(a != datum);
				Assert::IsTrue(datum != a);
			}
			{
				const string a = "owo";
				const string b = "uwu";
				const string c = "owo";
				Datum datum = a;
				Assert::IsTrue(a == datum);
				Assert::IsTrue(datum == a);
				Assert::IsTrue(c == datum);
				Assert::IsTrue(datum == c);
				Assert::IsFalse(b == datum);
				Assert::IsFalse(datum == b);

				Assert::IsFalse(a != datum);
				Assert::IsFalse(datum != a);
				Assert::IsFalse(c != datum);
				Assert::IsFalse(datum != c);
				Assert::IsTrue(b != datum);
				Assert::IsTrue(datum != b);

				string e{ "eek"};
				datum.SetStorage(&e, 1);
				Assert::IsTrue(e == datum);
				Assert::IsTrue(datum == e);
				Assert::IsFalse(a == datum);
				Assert::IsFalse(datum == a);

				Assert::IsFalse(e != datum);
				Assert::IsFalse(datum != e);
				Assert::IsTrue(a != datum);
				Assert::IsTrue(datum != a);
			}
			{
				const vec4 a(10);
				const vec4 b(20);
				const vec4 c(10);
				Datum datum = a;
				Assert::IsTrue(a == datum);
				Assert::IsTrue(datum == a);
				Assert::IsTrue(c == datum);
				Assert::IsTrue(datum == c);
				Assert::IsFalse(b == datum);
				Assert::IsFalse(datum == b);

				Assert::IsFalse(a != datum);
				Assert::IsFalse(datum != a);
				Assert::IsFalse(c != datum);
				Assert::IsFalse(datum != c);
				Assert::IsTrue(b != datum);
				Assert::IsTrue(datum != b);

				vec4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::IsTrue(e == datum);
				Assert::IsTrue(datum == e);
				Assert::IsFalse(a == datum);
				Assert::IsFalse(datum == a);

				Assert::IsFalse(e != datum);
				Assert::IsFalse(datum != e);
				Assert::IsTrue(a != datum);
				Assert::IsTrue(datum != a);
			}
			{
				const mat4 a(10);
				const mat4 b(20);
				const mat4 c(10);
				Datum datum = a;
				Assert::IsTrue(a == datum);
				Assert::IsTrue(datum == a);
				Assert::IsTrue(c == datum);
				Assert::IsTrue(datum == c);
				Assert::IsFalse(b == datum);
				Assert::IsFalse(datum == b);

				Assert::IsFalse(a != datum);
				Assert::IsFalse(datum != a);
				Assert::IsFalse(c != datum);
				Assert::IsFalse(datum != c);
				Assert::IsTrue(b != datum);
				Assert::IsTrue(datum != b);

				mat4 e{ 13 };
				datum.SetStorage(&e, 1);
				Assert::IsTrue(e == datum);
				Assert::IsTrue(datum == e);
				Assert::IsFalse(a == datum);
				Assert::IsFalse(datum == a);

				Assert::IsFalse(e != datum);
				Assert::IsFalse(datum != e);
				Assert::IsTrue(a != datum);
				Assert::IsTrue(datum != a);
			}
			{
				Foo aFoo(10);
				Foo bFoo(20);
				Foo cFoo(10);
				RTTI* a = &aFoo;
				RTTI* b = &bFoo;
				RTTI* c = &cFoo;
				Datum datum = a;
				Assert::IsTrue(a == datum);
				Assert::IsTrue(datum == a);
				Assert::IsTrue(c == datum);
				Assert::IsTrue(datum == c);
				Assert::IsFalse(b == datum);
				Assert::IsFalse(datum == b);

				Assert::IsFalse(a != datum);
				Assert::IsFalse(datum != a);
				Assert::IsFalse(c != datum);
				Assert::IsFalse(datum != c);
				Assert::IsTrue(b != datum);
				Assert::IsTrue(datum != b);

				Foo eFoo{ 13 };
				RTTI* e = &eFoo;
				datum.SetStorage(&e, 1);
				Assert::IsTrue(e == datum);
				Assert::IsTrue(datum == e);
				Assert::IsFalse(a == datum);
				Assert::IsFalse(datum == a);

				Assert::IsFalse(e != datum);
				Assert::IsFalse(datum != e);
				Assert::IsTrue(a != datum);
				Assert::IsTrue(datum != a);
			}
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}