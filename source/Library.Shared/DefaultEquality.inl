/// <summary>
/// The fully specified implementation of the tempated struct DefaultEquality and some of its specializations.
/// </summary>

#pragma once
#include "DefaultEquality.h"

namespace FIEAGameEngine{
	template<typename T>
	inline bool DefaultEquality<T>::operator()(const T& lhs, const T& rhs) const {
		return lhs == rhs;
	}

	inline bool DefaultEquality<char*>::operator()(const char* lhs, const char* rhs) const {
		return strcmp(lhs, rhs) == 0;
	}

	inline bool DefaultEquality<const char*>::operator()(const char* lhs, const char* rhs) const {
		return strcmp(lhs, rhs) == 0;
	}

	inline bool DefaultEquality<char* const>::operator()(const char* const lhs, const char* const rhs) const {
		return strcmp(lhs, rhs) == 0;
	}

	inline bool DefaultEquality<const char* const>::operator()(const char* const lhs, const char* const rhs) const {
		return strcmp(lhs, rhs) == 0;
	}
}