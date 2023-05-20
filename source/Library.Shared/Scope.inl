/// <summary>
/// The inline definitions of the class Scope, which is an object that stores string key-datum pairs.s
/// </summary>

#pragma once
#include "Scope.h"

namespace FIEAGameEngine {
	inline gsl::owner<Scope*> Scope::Clone() const {
		return new Scope(*this);
	}

	inline bool Scope::IsEmpty() const {
		return _orderVector.IsEmpty();
	}

	inline Datum* Scope::Search(const std::string& key) {
		Scope* dummy = nullptr;
		return Search(key, dummy);
	}

	inline std::size_t Scope::Size() const {
		return _orderVector.Size();
	}

	inline Scope* Scope::GetParent() const {
		return _parent;
	}

	inline bool Scope::IsDescendantOf(const Scope& otherScope) const {
		return otherScope.IsAncestorOf(*this);
	}

	inline Datum& Scope::At(const std::string& key) {
		return _attributes.At(key);
	}

	inline const Datum& Scope::At(const std::string& key) const {
		return _attributes.At(key);
	}

	inline Datum& Scope::operator[](const std::string& key) {
		return Append(key);
	}

	inline Datum& Scope::operator[](std::size_t index) {
		return _orderVector[index]->second;
	}

	inline const Datum& Scope::operator[](std::size_t index) const {
		return _orderVector[index]->second;
	}

	inline bool Scope::operator!=(const Scope& rhs) const {
		return !(operator==(rhs));
	}
}