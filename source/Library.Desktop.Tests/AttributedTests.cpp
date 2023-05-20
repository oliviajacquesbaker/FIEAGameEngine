#include "pch.h"
#include "CppUnitTest.h"
#include "AttributedFoo.h"
#include "Foo.h"
#include "ToStringSpecializations.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<Datum::DatumTypes>(const Datum::DatumTypes& t) {
		std::string stringResult = Datum::typeToString.At(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<AttributedFoo>(AttributedFoo* t) {
		std::string stringResult = t->ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<AttributedFoo>(const AttributedFoo* t) {
		std::string stringResult = t->ToString();
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
}

namespace  LibraryDesktopTests
{
	class UnregisteredAttributed final : public Attributed {
	public:
		UnregisteredAttributed() :
			Attributed(UnregisteredAttributed::TypeIdClass())
		{
		}

		[[nodiscard]] gsl::owner<UnregisteredAttributed*> Clone() const override {
			return new UnregisteredAttributed(*this);
		}
	};

	TEST_CLASS(AttributedTests)
	{
	public:
		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			RegisterType<AttributedFoo>();
		}

		TEST_CLASS_CLEANUP(TestClassCleanup)
		{
			TypeManager::Clear();
		}

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

		TEST_METHOD(ConstructionAndPopulation)
		{
			AttributedFoo attributedFoo;
			Assert::IsFalse(attributedFoo.IsEmpty());
			Assert::AreEqual(AttributedFoo::Signatures().Size() + 1, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsAttribute("this"));
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("externalInteger"));
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("nestedScope"));
			Assert::IsNull(attributedFoo.GetParent());

			Assert::ExpectException<std::runtime_error>([] {UnregisteredAttributed unregistered; UNREFERENCED_LOCAL(unregistered); });
		}

		TEST_METHOD(CopySemantics)
		{
			AttributedFoo attributedFoo;
			std::size_t expectedAttributesSize = AttributedFoo::Signatures().Size() + 1;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("nestedScope"));

			attributedFoo.externalInteger = 2;

			attributedFoo.Append("auxiliary") = 10;
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			{
				AttributedFoo copyOfAttributedFoo = attributedFoo;
				Assert::AreEqual(expectedAttributesSize, copyOfAttributedFoo.Size());

				Assert::IsTrue(copyOfAttributedFoo.IsPrescribedAttribute("nestedScope"));

				Assert::IsTrue(copyOfAttributedFoo.IsPrescribedAttribute("externalInteger"));
				Assert::AreEqual(2, copyOfAttributedFoo["externalInteger"].GetAsInt());
				copyOfAttributedFoo.externalInteger = 5;
				Assert::AreEqual(5, copyOfAttributedFoo["externalInteger"].GetAsInt());
				Assert::AreEqual(2, attributedFoo["externalInteger"].GetAsInt());

				Assert::IsTrue(copyOfAttributedFoo.IsAuxiliaryAttribute("auxiliary"));
				Assert::AreEqual(10, copyOfAttributedFoo["auxiliary"].GetAsInt());
				copyOfAttributedFoo["auxiliary"] = 5;
				Assert::AreEqual(5, copyOfAttributedFoo["auxiliary"].GetAsInt());
				Assert::AreEqual(10, attributedFoo["auxiliary"].GetAsInt());
			}
			{
				AttributedFoo copyOfAttributedFoo;
				copyOfAttributedFoo.Append("someotherthing") = glm::vec4(1);
				copyOfAttributedFoo.AppendScope("childToBeAbandoned").AppendScope("childChildToBeAbandoned");
				copyOfAttributedFoo = attributedFoo;
				Assert::AreEqual(expectedAttributesSize, copyOfAttributedFoo.Size());
				Assert::IsTrue(copyOfAttributedFoo.IsPrescribedAttribute("nestedScope"));

				Assert::IsTrue(copyOfAttributedFoo.IsPrescribedAttribute("externalInteger"));
				Assert::AreEqual(2, copyOfAttributedFoo["externalInteger"].GetAsInt());
				copyOfAttributedFoo.externalInteger = 5;
				Assert::AreEqual(5, copyOfAttributedFoo["externalInteger"].GetAsInt());
				Assert::AreEqual(2, attributedFoo["externalInteger"].GetAsInt());

				Assert::IsTrue(copyOfAttributedFoo.IsAuxiliaryAttribute("auxiliary"));
				Assert::AreEqual(10, copyOfAttributedFoo["auxiliary"].GetAsInt());
				copyOfAttributedFoo["auxiliary"] = 5;
				Assert::AreEqual(5, copyOfAttributedFoo["auxiliary"].GetAsInt());
				Assert::AreEqual(10, attributedFoo["auxiliary"].GetAsInt());
			}
		}

		TEST_METHOD(MoveSemantics)
		{
			AttributedFoo attributedFoo;
			std::size_t expectedAttributesSize = AttributedFoo::Signatures().Size() + 1;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("nestedScope"));

			attributedFoo.externalInteger = 2;

			attributedFoo.Append("auxiliary") = 10;
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			
			AttributedFoo copyOfAttributedFoo = std::move(attributedFoo);
			Assert::AreEqual(expectedAttributesSize, copyOfAttributedFoo.Size());

			Assert::IsTrue(copyOfAttributedFoo.IsPrescribedAttribute("nestedScope"));

			Assert::IsTrue(copyOfAttributedFoo.IsPrescribedAttribute("externalInteger"));
			Assert::AreEqual(2, copyOfAttributedFoo["externalInteger"].GetAsInt());
			copyOfAttributedFoo.externalInteger = 5;
			Assert::AreEqual(5, copyOfAttributedFoo["externalInteger"].GetAsInt());

			Assert::IsTrue(copyOfAttributedFoo.IsAuxiliaryAttribute("auxiliary"));
			Assert::AreEqual(10, copyOfAttributedFoo["auxiliary"].GetAsInt());
			copyOfAttributedFoo["auxiliary"] = 5;
			Assert::AreEqual(5, copyOfAttributedFoo["auxiliary"].GetAsInt());
			
			AttributedFoo copyOfAttributedFoo2;
			copyOfAttributedFoo2.Append("someotherthing") = glm::vec4(1);
			copyOfAttributedFoo2.AppendScope("childToBeAbandoned").AppendScope("childChildToBeAbandoned");
			copyOfAttributedFoo2 = std::move(copyOfAttributedFoo);

			Assert::AreEqual(expectedAttributesSize, copyOfAttributedFoo2.Size());
			Assert::IsTrue(copyOfAttributedFoo2.IsPrescribedAttribute("nestedScope"));

			Assert::IsTrue(copyOfAttributedFoo2.IsPrescribedAttribute("externalInteger"));
			Assert::AreEqual(5, copyOfAttributedFoo2["externalInteger"].GetAsInt());
			copyOfAttributedFoo2.externalInteger = 8;
			Assert::AreEqual(8, copyOfAttributedFoo2["externalInteger"].GetAsInt());

			Assert::IsTrue(copyOfAttributedFoo2.IsAuxiliaryAttribute("auxiliary"));
			Assert::AreEqual(5, copyOfAttributedFoo2["auxiliary"].GetAsInt());
			copyOfAttributedFoo2["auxiliary"] = 8;
			Assert::AreEqual(8, copyOfAttributedFoo2["auxiliary"].GetAsInt());
			
		}

		TEST_METHOD(IsAttribute)
		{
			AttributedFoo attributedFoo;
			Assert::IsTrue(attributedFoo.IsAttribute("externalFloatArray"));
			Assert::IsTrue(attributedFoo.IsAttribute("nestedScope"));

			attributedFoo.Append("auxiliary") = 10;
			Assert::IsTrue(attributedFoo.IsAttribute("auxiliary"));

			attributedFoo.AppendScope("auxiliaryScope");
			Assert::IsTrue(attributedFoo.IsAttribute("auxiliaryScope"));

			Assert::IsFalse(attributedFoo.IsAttribute("notThere"));

			Assert::IsTrue(attributedFoo.IsAttribute("this"));
		}

		TEST_METHOD(IsPrescribedAttribute)
		{
			AttributedFoo attributedFoo;
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("externalFloatArray"));
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("nestedScope"));

			attributedFoo.Append("auxiliary") = 10;
			Assert::IsFalse(attributedFoo.IsPrescribedAttribute("auxiliary"));

			attributedFoo.AppendScope("auxiliaryScope");
			Assert::IsFalse(attributedFoo.IsPrescribedAttribute("auxiliaryScope"));

			Assert::IsFalse(attributedFoo.IsPrescribedAttribute("notThere"));

			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("this"));
		}

		TEST_METHOD(IsAuxiliaryAttribute)
		{
			AttributedFoo attributedFoo;
			Assert::IsFalse(attributedFoo.IsAuxiliaryAttribute("externalFloatArray"));
			Assert::IsFalse(attributedFoo.IsAuxiliaryAttribute("nestedScope"));

			attributedFoo.Append("auxiliary") = 10;
			Assert::IsTrue(attributedFoo.IsAuxiliaryAttribute("auxiliary"));

			attributedFoo.AppendScope("auxiliaryScope");
			Assert::IsTrue(attributedFoo.IsAuxiliaryAttribute("auxiliaryScope"));

			Assert::IsFalse(attributedFoo.IsAuxiliaryAttribute("notThere"));

			Assert::IsFalse(attributedFoo.IsAuxiliaryAttribute("this"));
		}

		TEST_METHOD(AppendAuxiliaryAttribute)
		{
			AttributedFoo attributedFoo;
			std::size_t expectedAttributesSize = AttributedFoo::Signatures().Size() + 1;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			Datum& intDatum = attributedFoo.AppendAuxiliaryAttribute("auxiliaryInt");
			++expectedAttributesSize;
			intDatum = 10;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsAuxiliaryAttribute("auxiliaryInt"));
			Assert::AreEqual(10, attributedFoo["auxiliaryInt"].GetAsInt());
			Assert::IsFalse(attributedFoo["auxiliaryInt"].IsExternal());

			Datum& sameIntDatum = attributedFoo.AppendAuxiliaryAttribute("auxiliaryInt");
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsAuxiliaryAttribute("auxiliaryInt"));
			Assert::AreEqual(10, attributedFoo["auxiliaryInt"].GetAsInt());
			Assert::IsFalse(attributedFoo["auxiliaryInt"].IsExternal());
			Assert::AreEqual(intDatum, sameIntDatum);

			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("externalInteger"));
			Assert::ExpectException<std::runtime_error>([&attributedFoo] {attributedFoo.AppendAuxiliaryAttribute("externalInteger"); });
			Assert::ExpectException<std::runtime_error>([&attributedFoo] {attributedFoo.AppendAuxiliaryAttribute(""); });
		}

		TEST_METHOD(ForEachAttribute)
		{
			AttributedFoo attributedFoo;
			std::size_t expectedAttributesSize = AttributedFoo::Signatures().Size() + 1;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("nestedScope"));

			attributedFoo.Append("auxiliaryInt") = 10;
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			attributedFoo.Append("auxiliaryVector") = glm::vec4();
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			size_t attributeCount = 0;
			std::string names = "";
			attributedFoo.ForEachAttribute([&attributeCount, &names] (const std::string& key, Datum&) {
				++attributeCount;
				names += key;
				return false;
			});
			Assert::AreEqual(expectedAttributesSize, attributeCount);
			std::string allNames = "thisexternalIntegerexternalFloatexternalStringexternalVectorexternalMatrixexternalPointernestedScopeexternalIntegerArrayexternalFloatArrayexternalStringArrayexternalVectorArrayexternalMatrixArrayexternalPointerArraynestedScopeArrayauxiliaryIntauxiliaryVector";
			Assert::AreEqual(allNames, names);

			names = "";
			attributeCount = 0;
			attributedFoo.ForEachAttribute([&attributeCount, &names](const std::string& key, Datum&) {
				++attributeCount;
				names += key;
				if (key == "this") return true;
				return false;
			});
			Assert::AreEqual("this"s, names);
			Assert::AreEqual(size_t(1), attributeCount);
		}

		TEST_METHOD(ForEachPrescribedAttribute)
		{
			AttributedFoo attributedFoo;
			std::size_t expectedAttributesSize = AttributedFoo::Signatures().Size() + 1;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("nestedScope"));

			attributedFoo.Append("auxiliaryInt") = 10;
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			attributedFoo.Append("auxiliaryVector") = glm::vec4();
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			size_t attributeCount = 0;
			std::string names = "";
			attributedFoo.ForEachPrescribedAttribute([&attributeCount, &names](const std::string& key, Datum&) {
				++attributeCount;
				names += key;
				return false;
				});
			Assert::AreEqual(expectedAttributesSize - 2, attributeCount);
			std::string allNames = "thisexternalIntegerexternalFloatexternalStringexternalVectorexternalMatrixexternalPointernestedScopeexternalIntegerArrayexternalFloatArrayexternalStringArrayexternalVectorArrayexternalMatrixArrayexternalPointerArraynestedScopeArray";
			Assert::AreEqual(allNames, names); 

			names = "";
			attributeCount = 0;
			attributedFoo.ForEachPrescribedAttribute([&attributeCount, &names](const std::string& key, Datum&) {
				++attributeCount;
				names += key;
				if (key == "this") return true;
				return false;
				});
			Assert::AreEqual("this"s, names);
			Assert::AreEqual(size_t(1), attributeCount);
		}

		TEST_METHOD(ForEachAuxiliaryAttribute)
		{
			AttributedFoo attributedFoo;
			std::size_t expectedAttributesSize = AttributedFoo::Signatures().Size() + 1;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());
			Assert::IsTrue(attributedFoo.IsPrescribedAttribute("nestedScope"));

			attributedFoo.Append("auxiliaryInt") = 10;
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			attributedFoo.Append("auxiliaryVector") = glm::vec4();
			++expectedAttributesSize;
			Assert::AreEqual(expectedAttributesSize, attributedFoo.Size());

			size_t attributeCount = 0;
			std::string names = "";
			attributedFoo.ForEachAuxiliaryAttribute([&attributeCount, &names](const std::string& key, Datum&) {
				++attributeCount;
				names += key;
				return false;
			});
			Assert::AreEqual(size_t(2), attributeCount);
			std::string allNames = "auxiliaryIntauxiliaryVector";
			Assert::AreEqual(allNames, names);

			names = "";
			attributeCount = 0;
			attributedFoo.ForEachAuxiliaryAttribute([&attributeCount, &names](const std::string& key, Datum&) {
				++attributeCount;
				names += key;
				if (key == "auxiliaryInt") return true;
				return false;
				});
			Assert::AreEqual("auxiliaryInt"s, names);
			Assert::AreEqual(size_t(1), attributeCount);
		}

		TEST_METHOD(AttributedFooRTTIBasics)
		{
			{
				AttributedFoo a;
				RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(AttributedFoo::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(AttributedFoo::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				AttributedFoo* s = rtti->As<AttributedFoo>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				Attributed* s2 = rtti->As<Attributed>();
				Assert::AreEqual(rtti->TypeIdInstance(), s2->TypeIdInstance());
				Assert::IsNotNull(s2);
			}
			{
				const AttributedFoo a;
				const RTTI* rtti = &a;
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(AttributedFoo::TypeIdClass()));
				Assert::IsTrue(rtti->Is(Attributed::TypeIdClass()));
				Assert::AreEqual(AttributedFoo::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const AttributedFoo* s = rtti->As<AttributedFoo>();
				Assert::IsNotNull(s);
				Assert::AreEqual(&a, s);

				const Attributed* s2 = rtti->As<Attributed>();
				Assert::IsNotNull(s2);
			}
		}

		TEST_METHOD(AttributedFooRTTIOverrides)
		{
			AttributedFoo a;
			RTTI* rtti = &a;

			std::string expected = "this: (pointer to this object) \nexternalInteger: (0) \nexternalFloat: (0.000000) \nexternalString: () \nexternalVector: (vec4(0.000000, 0.000000, 0.000000, 0.000000)) \nexternalMatrix: (mat4x4((0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000))) \nexternalPointer: (nullptr) \nnestedScope:\nexternalIntegerArray: (0)  (0)  (0)  (0)  (0) \nexternalFloatArray: (0.000000)  (0.000000)  (0.000000)  (0.000000)  (0.000000) \nexternalStringArray: ()  ()  ()  ()  () \nexternalVectorArray: (vec4(0.000000, 0.000000, 0.000000, 0.000000))  (vec4(0.000000, 0.000000, 0.000000, 0.000000))  (vec4(0.000000, 0.000000, 0.000000, 0.000000))  (vec4(0.000000, 0.000000, 0.000000, 0.000000))  (vec4(0.000000, 0.000000, 0.000000, 0.000000)) \nexternalMatrixArray: (mat4x4((0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000)))  (mat4x4((0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000)))  (mat4x4((0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000)))  (mat4x4((0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000)))  (mat4x4((0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000), (0.000000, 0.000000, 0.000000, 0.000000))) \nexternalPointerArray: (nullptr)  (nullptr)  (nullptr)  (nullptr)  (nullptr) \nnestedScopeArray:";
			Assert::AreEqual(expected, rtti->ToString());

			AttributedFoo e;
			RTTI* eRtti = &e;
			Bar d;
			RTTI* dRtti = &d;
			
			Assert::IsTrue(rtti->Equals(eRtti));
			Assert::IsFalse(rtti->Equals(dRtti));
			Assert::IsFalse(rtti->Equals(nullptr));

			a.Append("key2") = { "A"s, "B"s };
			expected += "\nkey2: (A)  (B) ";
			Assert::AreEqual(expected, rtti->ToString());
			Assert::IsFalse(rtti->Equals(eRtti));
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}