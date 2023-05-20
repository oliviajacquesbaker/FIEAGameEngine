#pragma once

#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include "Attributed.h"
#include "Factory.h"
#include "Foo.h"

namespace UnitTests
{
	class AttributedFoo final : public FIEAGameEngine::Attributed
	{
		RTTI_DECLARATIONS(AttributedFoo, FIEAGameEngine::Attributed);

	public:
		static const std::size_t arraySize = 5;

		AttributedFoo();
		AttributedFoo(const AttributedFoo& rhs) = default;
		AttributedFoo(AttributedFoo&& rhs) noexcept = default;
		AttributedFoo& operator=(const AttributedFoo& rhs) = default;
		AttributedFoo& operator=(AttributedFoo&& rhs) noexcept = default;
		~AttributedFoo() = default;
		[[nodiscard]] gsl::owner<AttributedFoo*> Clone() const override;

		int externalInteger = 0;
		float externalFloat = 0;
		std::string externalString;
		glm::vec4 externalVector;
		glm::mat4 externalMatrix;
		Foo* externalPointer;

		int externalIntegerArray[arraySize];
		float externalFloatArray[arraySize];
		std::string externalStringArray[arraySize];
		glm::vec4 externalVectorArray[arraySize];
		glm::mat4 externalMatrixArray[arraySize];
		Foo* externalPointerArray[arraySize];

		bool Equals(const RTTI* rhs) const override;
		std::string ToString() const override;

		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
	};

	CONCRETE_FACTORY(AttributedFoo, FIEAGameEngine::Scope)
}

