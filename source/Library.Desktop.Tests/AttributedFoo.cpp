#include "pch.h"
#include "AttributedFoo.h"

using namespace FIEAGameEngine;
using namespace std::string_literals;

namespace UnitTests
{
	RTTI_DEFINITIONS(AttributedFoo);

	AttributedFoo::AttributedFoo():
		Attributed(AttributedFoo::TypeIdClass())
	{
	}

	gsl::owner<AttributedFoo*> AttributedFoo::Clone() const {
		return new AttributedFoo(*this);
	}

	Vector<Signature> AttributedFoo::Signatures() {
		return Vector<Signature> {
			{ "externalInteger"s, Datum::DatumTypes::Integer, 1, offsetof(AttributedFoo, externalInteger) },
			{ "externalFloat"s, Datum::DatumTypes::Float, 1, offsetof(AttributedFoo, externalFloat) },
			{ "externalString"s, Datum::DatumTypes::String, 1, offsetof(AttributedFoo, externalString) },
			{ "externalVector"s, Datum::DatumTypes::Vector, 1, offsetof(AttributedFoo, externalVector) },
			{ "externalMatrix"s, Datum::DatumTypes::Matrix, 1, offsetof(AttributedFoo, externalMatrix) },
			{ "externalPointer"s, Datum::DatumTypes::Pointer, 1, offsetof(AttributedFoo, externalPointer) },
			{ "nestedScope"s, Datum::DatumTypes::Table, 1, 0},
			{ "externalIntegerArray"s, Datum::DatumTypes::Integer, arraySize, offsetof(AttributedFoo, externalIntegerArray) },
			{ "externalFloatArray"s, Datum::DatumTypes::Float, arraySize, offsetof(AttributedFoo, externalFloatArray) },
			{ "externalStringArray"s, Datum::DatumTypes::String, arraySize, offsetof(AttributedFoo, externalStringArray) },
			{ "externalVectorArray"s, Datum::DatumTypes::Vector, arraySize, offsetof(AttributedFoo, externalVectorArray) },
			{ "externalMatrixArray"s, Datum::DatumTypes::Matrix, arraySize, offsetof(AttributedFoo, externalMatrixArray) },
			{ "externalPointerArray"s, Datum::DatumTypes::Pointer, arraySize, offsetof(AttributedFoo, externalPointerArray) },
			{ "nestedScopeArray"s, Datum::DatumTypes::Table, arraySize, 0},
		};
	}

	bool AttributedFoo::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const AttributedFoo* other = rhs->As<AttributedFoo>();
		if (other == nullptr) return false;

		bool equalsReturn = (externalInteger == other->externalInteger) &&
			(externalFloat == other->externalFloat) &&
			(externalString == other->externalString) &&
			(externalVector == other->externalVector) &&
			(externalMatrix == other->externalMatrix);
		if (!equalsReturn) return false;

		equalsReturn = std::equal(std::begin(externalIntegerArray), std::end(externalIntegerArray), std::begin(other->externalIntegerArray)) &&
			std::equal(std::begin(externalFloatArray), std::end(externalFloatArray), std::begin(other->externalFloatArray)) &&
			std::equal(std::begin(externalStringArray), std::end(externalStringArray), std::begin(other->externalStringArray)) &&
			std::equal(std::begin(externalVectorArray), std::end(externalVectorArray), std::begin(other->externalVectorArray)) &&
			std::equal(std::begin(externalMatrixArray), std::end(externalMatrixArray), std::begin(other->externalMatrixArray)) &&
			std::equal(std::begin(externalPointerArray), std::end(externalPointerArray), std::begin(other->externalPointerArray), [](Foo* lhs, Foo* rhs) { return (lhs != nullptr) ? (lhs->Equals(rhs)) : (rhs == nullptr); });
		if (!equalsReturn) return false;

		equalsReturn = (externalPointer != nullptr) ? (externalPointer->Equals(other->externalPointer)) : (other->externalPointer == nullptr);
		if (!equalsReturn) return false;

		ForEachAttribute([&equalsReturn, &other, this](const std::string& key, Datum& value) {
			if (key != "this" && !value.IsExternal() || this->IsAuxiliaryAttribute(key)) {
				auto otherDatum = other->Find(key);
				if (otherDatum == nullptr || *otherDatum != value) {
					equalsReturn = false;
					return true;
				}
			}			
			return false;
		});

		return equalsReturn;
	}

	std::string AttributedFoo::ToString() const {
		return Attributed::ToString();
	}

}