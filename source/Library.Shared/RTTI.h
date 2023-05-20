#pragma once

#include <cstddef>
#include <string>

namespace FIEAGameEngine
{
	class RTTI
	{
	public:
		using IdType = std::size_t;

		RTTI() = default;
		RTTI(const RTTI&) = default;
		RTTI& operator=(const RTTI&) = default;
		RTTI(RTTI&&) noexcept = default;
		RTTI& operator=(RTTI&&) noexcept = default;
		virtual ~RTTI() = default;

		virtual IdType TypeIdInstance() const = 0;
		virtual bool Is(IdType) const;

		template <typename T>
		T* As();

		template <typename T>
		const T* As() const;

		virtual std::string ToString() const;
		virtual bool Equals(const RTTI* rhs) const;
	};
}

#define RTTI_DECLARATIONS(Type, ParentType)																					\
	public:																													\
		static FIEAGameEngine::RTTI::IdType TypeIdClass() { return _typeId; }												\
		FIEAGameEngine::RTTI::IdType TypeIdInstance() const override { return TypeIdClass(); }								\
		bool Is(FIEAGameEngine::RTTI::IdType id) const override { return (id == _typeId ? true : ParentType::Is(id)); }		\
	private:																												\
		static const FIEAGameEngine::RTTI::IdType _typeId

#define RTTI_DEFINITIONS(Type)																								\
	const FIEAGameEngine::RTTI::IdType Type::_typeId = reinterpret_cast<FIEAGameEngine::RTTI::IdType>(&Type::_typeId)
	

#include "RTTI.inl"