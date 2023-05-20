/// <summary>
/// The declaration of the templated struct DefaultEquality and some of its specializations.
/// </summary>

#pragma once

#include <cstdint>
#include <string.h>

namespace FIEAGameEngine {
	/// <summary>
	/// The default implementation of an equality comparison.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	struct DefaultEquality final {
		/// <summary>
		/// Template for a functor that tests the quality of two objects.
		/// </summary>
		/// <param name="lhs">The first object to compare</param>
		/// <param name="rhs">The second object to compare against the first</param>
		/// <returns>A boolean indicating if the two objects are equavilant</returns>
		bool operator()(const T& lhs, const T& rhs) const;
	};

	/// <summary>
	/// The equality comparison specified for char*.
	/// </summary>
	template<>
	struct DefaultEquality<char*> final {
		/// <summary>
		/// Template specialization for a functor that tests the quality of two char*.
		/// </summary>
		/// <param name="lhs">The first char* to compare</param>
		/// <param name="rhs">The second char* to compare against the first</param>
		/// <returns>A boolean indicating if the two char* are equavilant</returns>
		bool operator()(const char* lhs, const char* rhs) const;
	};

	/// <summary>
	/// The equality comparison specified for const char*.
	/// </summary>
	template<>
	struct DefaultEquality<const char*> final {
		/// <summary>
		/// Template specialization for a functor that tests the quality of two const char*.
		/// </summary>
		/// <param name="lhs">The first const char* to compare</param>
		/// <param name="rhs">The second const char* to compare against the first</param>
		/// <returns>A boolean indicating if the two const char* are equavilant</returns>
		bool operator()(const char* lhs, const char* rhs) const;
	};

	/// <summary>
	/// The equality comparison specified for char* const.
	/// </summary>
	template<>
	struct DefaultEquality<char* const> final {
		/// <summary>
		/// Template specialization for a functor that tests the quality of two char* const.
		/// </summary>
		/// <param name="lhs">The first char* const to compare</param>
		/// <param name="rhs">The second char* const to compare against the first</param>
		/// <returns>A boolean indicating if the two char* const are equavilant</returns>
		bool operator()(const char* const lhs, const char* const rhs) const;
	};

	/// <summary>
	/// The equality comparison specified for const char* const.
	/// </summary>
	template<>
	struct DefaultEquality<const char* const> final {
		/// <summary>
		/// Template specialization for a functor that tests the quality of two const char* const.
		/// </summary>
		/// <param name="lhs">The first const char* const to compare</param>
		/// <param name="rhs">The second const char* const to compare against the first</param>
		/// <returns>A boolean indicating if the two const char* const are equavilant</returns>
		bool operator()(const char* const lhs, const char* const rhs) const;
	};
}

#include "DefaultEquality.inl"

