#include "pch.h"
#include "CppUnitTest.h"
#include "TypeManager.h"
#include "Vector.h"
#include "AttributedFoo.h"
#include "ToStringSpecializations.h"
#include <algorithm>

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
	std::wstring ToString<Signature>(const Signature& t) {
		return ToString(t._name) + ToString(t._type) + ToString(t._size) + ToString(t._offset);
	}
}

namespace  LibraryDesktopTests
{
	class ParentFoo : public Attributed { 
		RTTI_DECLARATIONS(ParentFoo, FIEAGameEngine::RTTI);
		float externalFloat = 0;

	public:
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "externalFloat"s, Datum::DatumTypes::Float, 1, offsetof(ParentFoo, externalFloat) },
			};
		}
	};
	RTTI_DEFINITIONS(ParentFoo);

	class DerivedFoo final : public  ParentFoo {
		RTTI_DECLARATIONS(DerivedFoo, FIEAGameEngine::RTTI);
		float externalInteger = 0;

	public:
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures() {
			return Vector<Signature> {
				{ "externalInteger"s, Datum::DatumTypes::Integer, 1, offsetof(DerivedFoo, externalInteger) },
			};
		}
	};
	RTTI_DEFINITIONS(DerivedFoo);

	TEST_CLASS(TypeManagerTests)
	{
	public:
		TEST_METHOD_INITIALIZE(Initialize)
		{
			TypeManager::Clear(); 

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

		TEST_METHOD(SignatureConstructor)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);

			Assert::AreEqual(aName, signature._name);
			Assert::AreEqual(Datum::DatumTypes::Integer, signature._type);
			Assert::AreEqual(aSize, signature._size);
			Assert::AreEqual(aOffset, signature._offset);
		}

		TEST_METHOD(SignatureCopySemantics)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);

			Assert::AreEqual(aName, signature._name);
			Assert::AreEqual(Datum::DatumTypes::Integer, signature._type);
			Assert::AreEqual(aSize, signature._size);
			Assert::AreEqual(aOffset, signature._offset);

			{
				Signature copyOfSignature = signature;
				Assert::AreEqual(aName, copyOfSignature._name);
				Assert::AreEqual(Datum::DatumTypes::Integer, copyOfSignature._type);
				Assert::AreEqual(aSize, copyOfSignature._size);
				Assert::AreEqual(aOffset, copyOfSignature._offset);
			}
			{
				Signature copyOfSignature("B", Datum::DatumTypes::String, 0, 0);;
				copyOfSignature = signature;
				Assert::AreEqual(aName, copyOfSignature._name);
				Assert::AreEqual(Datum::DatumTypes::Integer, copyOfSignature._type);
				Assert::AreEqual(aSize, copyOfSignature._size);
				Assert::AreEqual(aOffset, copyOfSignature._offset);
			}
		}

		TEST_METHOD(SignatureMoveSemantics)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);

			Assert::AreEqual(aName, signature._name);
			Assert::AreEqual(Datum::DatumTypes::Integer, signature._type);
			Assert::AreEqual(aSize, signature._size);
			Assert::AreEqual(aOffset, signature._offset);

			Signature copyOfSignature{ std::move(signature) };
			Assert::AreEqual(aName, copyOfSignature._name);
			Assert::AreEqual(Datum::DatumTypes::Integer, copyOfSignature._type);
			Assert::AreEqual(aSize, copyOfSignature._size);
			Assert::AreEqual(aOffset, copyOfSignature._offset);
			
			Signature copyOfSignature2("B", Datum::DatumTypes::String, 0, 0);;
			copyOfSignature2 = std::move(copyOfSignature);
			Assert::AreEqual(aName, copyOfSignature2._name);
			Assert::AreEqual(Datum::DatumTypes::Integer, copyOfSignature2._type);
			Assert::AreEqual(aSize, copyOfSignature2._size);
			Assert::AreEqual(aOffset, copyOfSignature2._offset);
		}

		TEST_METHOD(SignatureEquality)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signature2(bName, Datum::DatumTypes::Integer, bSize, bOffset);

			Assert::AreNotEqual(signature, signature2);
			Assert::IsTrue(signature != signature2);

			signature2 = signature;
			Assert::AreEqual(signature, signature2);
			Assert::IsFalse(signature != signature2);
		}

		TEST_METHOD(Add)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			const Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);
			RTTI::IdType id = 13;
			Vector<Signature> vector{ signature };

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signatureParent(bName, Datum::DatumTypes::Integer, bSize, bOffset);
			Signature signatureParent2(bName + "2", Datum::DatumTypes::Integer, bSize, bOffset);
			RTTI::IdType idParent = 15;
			Vector<Signature> vectorParent{ signatureParent, signatureParent2 };
			Vector<Signature> vectorTogether{ signature, signatureParent, signatureParent2 };

			Assert::IsFalse(TypeManager::ContainsSignature(id));
			
			TypeManager::Add(id, vector);
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(id).cbegin(), TypeManager::GetSignatureByTypeID(id).cend(), vector.cbegin()));
			TypeManager::Remove(id);
			Assert::IsFalse(TypeManager::ContainsSignature(id));

			Assert::ExpectException<std::runtime_error>([&id, &idParent, &vector] {TypeManager::Add(id, idParent, vector); });
			
			TypeManager::Add(idParent, vectorParent);
			Assert::IsTrue(TypeManager::ContainsSignature(idParent));
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(idParent).cbegin(), TypeManager::GetSignatureByTypeID(idParent).cend(), vectorParent.cbegin()));
			TypeManager::Add(id, idParent, vector);
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(id).cbegin(), TypeManager::GetSignatureByTypeID(id).cend(), vectorTogether.cbegin()));

			//reset memory
			TypeManager::Remove(id);
			TypeManager::Remove(idParent);
		}

		TEST_METHOD(AddMoveSemantics)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			const Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);
			RTTI::IdType id = 13;
			Vector<Signature> vector{ signature };

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signatureParent(bName, Datum::DatumTypes::Integer, bSize, bOffset);
			Signature signatureParent2(bName + "2", Datum::DatumTypes::Integer, bSize, bOffset);
			RTTI::IdType idParent = 15;
			Vector<Signature> vectorParent{ signatureParent, signatureParent2 };
			Vector<Signature> vectorTogether{ signature, signatureParent, signatureParent2 };

			Assert::IsFalse(TypeManager::ContainsSignature(id));

			TypeManager::Add(id, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(id).cbegin(), TypeManager::GetSignatureByTypeID(id).cend(), vector.cbegin()));
			TypeManager::Remove(id);
			Assert::IsFalse(TypeManager::ContainsSignature(id));

			Assert::ExpectException<std::runtime_error>([&id, &idParent, &signature] {TypeManager::Add(id, idParent, { signature }); });

			TypeManager::Add(idParent, { signatureParent, signatureParent2 });
			Assert::IsTrue(TypeManager::ContainsSignature(idParent));
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(idParent).cbegin(), TypeManager::GetSignatureByTypeID(idParent).cend(), vectorParent.cbegin()));
			TypeManager::Add(id, idParent, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(id).cbegin(), TypeManager::GetSignatureByTypeID(id).cend(), vectorTogether.cbegin()));

			//reset memory
			TypeManager::Remove(id);
			TypeManager::Remove(idParent);
		}

		TEST_METHOD(Remove) {
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			const Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);
			RTTI::IdType id = 13;

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signatureParent(bName, Datum::DatumTypes::Integer, bSize, bOffset);
			Signature signatureParent2(bName + "2", Datum::DatumTypes::Integer, bSize, bOffset);
			RTTI::IdType idParent = 15;

			Assert::IsFalse(TypeManager::ContainsSignature(id));
			TypeManager::Add(id, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			TypeManager::Remove(id);
			Assert::IsFalse(TypeManager::ContainsSignature(id));

			TypeManager::Add(idParent, { signatureParent, signatureParent2 });
			Assert::IsTrue(TypeManager::ContainsSignature(idParent));
			TypeManager::Add(id, idParent, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));

			TypeManager::Remove(idParent);
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			Assert::IsFalse(TypeManager::ContainsSignature(idParent));
			TypeManager::Remove(id);
			Assert::IsFalse(TypeManager::ContainsSignature(id));

			Assert::IsFalse(TypeManager::ContainsSignature(0));
			TypeManager::Remove(0);
			Assert::IsFalse(TypeManager::ContainsSignature(0));
		}

		TEST_METHOD(Clear)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			const Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);
			RTTI::IdType id = 13;

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signatureParent(bName, Datum::DatumTypes::Integer, bSize, bOffset);
			Signature signatureParent2(bName + "2", Datum::DatumTypes::Integer, bSize, bOffset);
			RTTI::IdType idParent = 15;

			Assert::IsFalse(TypeManager::ContainsSignature(id));

			TypeManager::Add(id, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			TypeManager::Clear();
			Assert::IsFalse(TypeManager::ContainsSignature(id));

			TypeManager::Add(idParent, { signatureParent, signatureParent2 });
			Assert::IsTrue(TypeManager::ContainsSignature(idParent));
			TypeManager::Add(id, idParent, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));

			TypeManager::Clear();
			Assert::IsFalse(TypeManager::ContainsSignature(id));
			Assert::IsFalse(TypeManager::ContainsSignature(idParent));

			Assert::IsFalse(TypeManager::ContainsSignature(0));
			TypeManager::Remove(0);
			Assert::IsFalse(TypeManager::ContainsSignature(0));
		}

		TEST_METHOD(ContainsSignature)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			const Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);
			RTTI::IdType id = 13;

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signatureParent(bName, Datum::DatumTypes::Integer, bSize, bOffset);
			Signature signatureParent2(bName + "2", Datum::DatumTypes::Integer, bSize, bOffset);
			RTTI::IdType idParent = 15;

			Assert::IsFalse(TypeManager::ContainsSignature(id));
			TypeManager::Add(id, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			TypeManager::Remove(id);
			Assert::IsFalse(TypeManager::ContainsSignature(id));

			Assert::IsFalse(TypeManager::ContainsSignature(idParent));
			TypeManager::Add(idParent, { signatureParent, signatureParent2 });
			Assert::IsTrue(TypeManager::ContainsSignature(idParent));
			TypeManager::Add(id, idParent, { signature });
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			Assert::IsTrue(TypeManager::ContainsSignature(idParent));

			TypeManager::Remove(idParent);
			Assert::IsTrue(TypeManager::ContainsSignature(id));
			Assert::IsFalse(TypeManager::ContainsSignature(idParent));
			TypeManager::Remove(id);
			Assert::IsFalse(TypeManager::ContainsSignature(id));

			Assert::IsFalse(TypeManager::ContainsSignature(0));
			TypeManager::Remove(0);
			Assert::IsFalse(TypeManager::ContainsSignature(0));
		}

		TEST_METHOD(GetSignatureByTypeID)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			const Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);
			RTTI::IdType id = 13;
			Vector<Signature> vector{ signature };

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signatureParent(bName, Datum::DatumTypes::Integer, bSize, bOffset);
			Signature signatureParent2(bName + "2", Datum::DatumTypes::Integer, bSize, bOffset);
			RTTI::IdType idParent = 15;
			Vector<Signature> vectorParent{ signatureParent, signatureParent2 };
			Vector<Signature> vectorTogether{ signature, signatureParent, signatureParent2 };

			Assert::ExpectException<std::runtime_error>([&id] {auto& val = TypeManager::GetSignatureByTypeID(id); UNREFERENCED_LOCAL(val); });
			TypeManager::Add(id, { signature });
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(id).cbegin(), TypeManager::GetSignatureByTypeID(id).cend(), vector.cbegin()));
			Assert::ExpectException<std::runtime_error>([&idParent] {auto& val = TypeManager::GetSignatureByTypeID(idParent); UNREFERENCED_LOCAL(val); });

			TypeManager::Add(idParent, { signatureParent, signatureParent2 });
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(idParent).cbegin(), TypeManager::GetSignatureByTypeID(idParent).cend(), vectorParent.cbegin()));
			TypeManager::Add(id, idParent, { signature });
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(idParent).cbegin(), TypeManager::GetSignatureByTypeID(idParent).cend(), vectorParent.cbegin()));
			Assert::IsTrue(std::equal(TypeManager::GetSignatureByTypeID(id).cbegin(), TypeManager::GetSignatureByTypeID(id).cend(), vectorTogether.cbegin()));
			 
			//reset memory
			TypeManager::Remove(id);
			TypeManager::Remove(idParent);
		}

		TEST_METHOD(Types)
		{
			std::string aName = "A"s;
			std::size_t aSize = 5;
			std::size_t aOffset = 10;
			const Signature signature(aName, Datum::DatumTypes::Integer, aSize, aOffset);
			RTTI::IdType id = 13;
			Vector<Signature> vector{ signature };

			std::string bName = "B"s;
			std::size_t bSize = 2;
			std::size_t bOffset = 11;
			Signature signatureParent(bName, Datum::DatumTypes::Integer, bSize, bOffset);
			Signature signatureParent2(bName + "2", Datum::DatumTypes::Integer, bSize, bOffset);
			RTTI::IdType idParent = 15;
			Vector<Signature> vectorParent{ signatureParent, signatureParent2 };
			Vector<Signature> vectorTogether{ signature, signatureParent, signatureParent2 };

			Assert::AreEqual(size_t(0), TypeManager::Types().Size());
			TypeManager::Add(id, { signature });
			Assert::AreEqual(size_t(1), TypeManager::Types().Size());
			Assert::IsTrue(std::equal(TypeManager::Types().At(id).cbegin(), TypeManager::Types().At(id).cend(), vector.cbegin()));

			TypeManager::Add(idParent, { signatureParent, signatureParent2 });
			Assert::AreEqual(size_t(2), TypeManager::Types().Size());
			Assert::IsTrue(std::equal(TypeManager::Types().At(idParent).cbegin(), TypeManager::Types().At(idParent).cend(), vectorParent.cbegin()));
			TypeManager::Add(id, idParent, { signature });
			Assert::AreEqual(size_t(2), TypeManager::Types().Size());
			Assert::IsTrue(std::equal(TypeManager::Types().At(idParent).cbegin(), TypeManager::Types().At(idParent).cend(), vectorParent.cbegin()));
			Assert::IsTrue(std::equal(TypeManager::Types().At(id).cbegin(), TypeManager::Types().At(id).cend(), vectorTogether.cbegin()));

			//reset memory
			TypeManager::Remove(id);
			TypeManager::Remove(idParent);
		}

		TEST_METHOD(RegisterTypeTemplate)
		{
			Assert::AreEqual(size_t(0), TypeManager::Types().Size());
			RegisterType<ParentFoo>();
			auto signatures = ParentFoo::Signatures();
			Assert::AreEqual(signatures.Size(), TypeManager::GetSignatureByTypeID(ParentFoo::TypeIdClass()).Size());
			Assert::IsTrue(std::equal(signatures.begin(), signatures.end(), TypeManager::GetSignatureByTypeID(ParentFoo::TypeIdClass()).begin()));

			Assert::AreEqual(size_t(1), TypeManager::Types().Size());
			RegisterType<DerivedFoo, ParentFoo>();
			Vector<Signature> signaturesTogether{
				DerivedFoo::Signatures()[0],
				ParentFoo::Signatures()[0]
			};
			Assert::AreEqual(signaturesTogether.Size(), TypeManager::GetSignatureByTypeID(DerivedFoo::TypeIdClass()).Size());
			Assert::IsTrue(std::equal(signaturesTogether.begin(), signaturesTogether.end(), TypeManager::GetSignatureByTypeID(DerivedFoo::TypeIdClass()).begin()));

			//reset memory
			TypeManager::Remove(ParentFoo::TypeIdClass());
			TypeManager::Remove(DerivedFoo::TypeIdClass());
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}