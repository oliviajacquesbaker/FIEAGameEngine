#pragma once

#include <cstdint>
#include <stdexcept>
#include "Factory.h"
#include "RTTI.h"

namespace UnitTests
{
	class Bar final : public FIEAGameEngine::RTTI {
		RTTI_DECLARATIONS(Bar, FIEAGameEngine::RTTI);

	public:
		explicit Bar(std::int32_t data = 0);
		Bar(const Bar& rhs);
		Bar(Bar&& rhs) noexcept;
		Bar& operator=(const Bar& rhs);
		Bar& operator=(Bar&& rhs) noexcept;
		virtual ~Bar();

		bool Equals(const RTTI* rhs) const;
		std::string ToString() const;

		std::int32_t Data() const;
		void SetData(std::int32_t data);
	private:
		std::int32_t* _data;
	};

	CONCRETE_FACTORY(Bar, FIEAGameEngine::RTTI)
}

