/// <summary>
/// The declaration of the templated type DefaultHash and some of its specializations.
/// </summary>

#pragma once

#include <cstdint>
#include <cassert>
#include <stdlib.h>
#include <string>

namespace FIEAGameEngine {
	/// <summary>
	/// The default implementation of the hash function.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename TKey>
	struct DefaultHash final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of any type.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const TKey& key) const;
	};

	template<>
	struct DefaultHash<std::string> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of strings.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const std::string& key) const;
	};

	template<>
	struct DefaultHash<const std::string> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of const strings
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const std::string& key) const;
	};

	template<>
	struct DefaultHash<std::wstring> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of wide strings.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const std::wstring& key) const;
	};

	template<>
	struct DefaultHash<const std::wstring> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of const wide strings.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const std::wstring& key) const;
	};

	template<>
	struct DefaultHash<char*> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of char*.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const char* key) const;
	};

	template<>
	struct DefaultHash<const char*> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of const char*.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const char* key) const;
	};

	template<>
	struct DefaultHash<char* const> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of char* const.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const char* const key) const;
	};

	template<>
	struct DefaultHash<const char* const> final {
		/// <summary>
		/// Template for a functor that produces a hash value from an input of const char* const.
		/// </summary>
		/// <param name="lhs">The object to hash</param>
		/// <returns>The hashed value</returns>
		std::size_t operator()(const char* const key) const;
	};
}

#include "DefaultHash.inl"
