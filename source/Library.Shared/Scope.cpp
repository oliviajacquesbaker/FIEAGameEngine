/// <summary>
/// The definitions of the class Scope, which is an object that stores string key - datum pairs.
/// </summary>

#include "pch.h"
#include "Scope.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(Scope);

	Scope::Scope(std::size_t size) :
		_orderVector{size}
	{
	}
	
	Scope::Scope(const Scope& rhs) {
		DeepCopy(rhs);
	}
	
	Scope::Scope(Scope&& rhs) noexcept :
		_attributes{ std::move(rhs._attributes) }, _orderVector{ std::move(rhs._orderVector) }
	{
		Reparent(rhs);
	}
	
	Scope& Scope::operator=(const Scope& rhs) {
		if (this != &rhs) {
			Clear();
			DeepCopy(rhs);
		}
		return *this;
	}
	
	Scope& Scope::operator=(Scope&& rhs) noexcept {
		if (this != &rhs) {
			Clear();

			_attributes = std::move(rhs._attributes);
			_orderVector = std::move(rhs._orderVector);
			Reparent(rhs);
		}
		return *this;
	}
	
	Scope::~Scope() {
		auto catchSelfPointer = Orphan();
		UNREFERENCED_LOCAL(catchSelfPointer);
		Clear();
	}

	Datum* Scope::Find(const std::string& key) {
		auto findResult = _attributes.Find(key);
		if (findResult == _attributes.end()) return nullptr;
		else return &findResult->second;
	}

	const Datum* Scope::Find(const std::string& key) const {
		auto findResult = _attributes.Find(key);
		if (findResult == _attributes.end()) return nullptr;
		else return &findResult->second;
	}
	
	std::pair<Datum*, std::size_t> Scope::FindContainedScope(Scope* const address) const {
		Datum* foundDatum = nullptr;
		std::size_t foundIndex;
		ForEachNestedScope([address, &foundDatum, &foundIndex](Datum& datum, std::size_t index) {
			UNREFERENCED_LOCAL(index);
			foundIndex = datum.FindIndex(*address);
			if (foundIndex != datum.Size()) {
				foundDatum = &datum;
				return true;
			}
			return false;
		});

		return std::pair<Datum*, std::size_t>(foundDatum, foundIndex);
	}
	
	Datum* Scope::Search(const std::string& key, Scope*& foundScope) {
		foundScope = nullptr;
		Datum* foundDatum = Find(key);
		if (foundDatum == nullptr && _parent != nullptr) return _parent->Search(key, foundScope);
		else {
			if(foundDatum != nullptr) foundScope = this;
			return foundDatum;
		}
	}
	
	Datum& Scope::Append(const std::string& key) {
#ifdef USE_EXCEPTIONS
		if (key.empty()) throw std::runtime_error("Key cannot be empty.");
#endif // USE_EXCEPTIONS

		auto [foundIter, inserted] = _attributes.Insert(std::pair<const std::string, Datum>(key, Datum()));
		if (inserted) _orderVector.PushBack(&*foundIter);
		return foundIter->second;
	}
	
	Scope& Scope::AppendScope(const std::string& key) {
		Datum& tableDatum = Append(key);
		 
#ifdef USE_EXCEPTIONS
		if (tableDatum.Type() != Datum::DatumTypes::Unknown && tableDatum.Type() != Datum::DatumTypes::Table) throw std::runtime_error("Datum already exists of non table type.");
#endif // USE_EXCEPTIONS

		Scope* newScope = new Scope();
		tableDatum.PushBack(*newScope);
		newScope->_parent = this;
		return *newScope;
	}
	
	gsl::owner<Scope*> Scope::Orphan() {
		if (_parent != nullptr) {
			auto [containingDatum, index] = _parent->FindContainedScope(this);
			containingDatum->RemoveAt(index);	
			_parent = nullptr;
		}
		return this;
	}
	
	void Scope::Adopt(Scope& newChild, const std::string& childName) {
#ifdef USE_EXCEPTIONS
		if (this == &newChild) throw std::runtime_error("Cannot adopt self.");
		if (IsDescendantOf(newChild)) throw std::runtime_error("Cannot adopt an ancestor Scope.");
#endif // USE_EXCEPTIONS

		Datum& datum = Append(childName);

#ifdef USE_EXCEPTIONS
		if (datum.Type() != Datum::DatumTypes::Unknown && datum.Type() != Datum::DatumTypes::Table) throw std::runtime_error("Datum already exists of non table type.");
#endif // USE_EXCEPTIONS

		Scope* orphanedScope = newChild.Orphan();
		orphanedScope->_parent = this;
		datum.PushBack(*orphanedScope);
	}

	bool Scope::IsAncestorOf(const Scope& otherScope) const {
		if (otherScope._parent == nullptr) return false;
		if (otherScope._parent == this) return true;
		return IsAncestorOf(*otherScope._parent);
	}
	
	bool Scope::operator==(const Scope& rhs) const {
		if (_orderVector.Size() != rhs._orderVector.Size()) return false;
		for (auto item : rhs._orderVector) {
			if (item->first == "this") continue;

			auto findResult = _attributes.Find(item->first);
			if (findResult == _attributes.end() || item->second != findResult->second) return false;
		}
		return true;
	}

	bool Scope::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const Scope* other = rhs->As<Scope>();
		return (other != nullptr ? *this == *other : false);
	}

	std::string Scope::ToString() const {
		std::string strValue = ""s;
		for (std::size_t i = 0; i < Size(); ++i) {
			if (i > 0) strValue += "\n";
			strValue += _orderVector[i]->first + ":"s;
			for (std::size_t j = 0; j < _orderVector[i]->second.Size(); ++j) {
				strValue += " ("s + _orderVector[i]->second.ToString(j) + ") "s;
			}
		}
		return strValue;
	}
	
	void Scope::Clear() {
		if (Size() > 0) {
			ForEachNestedScope([](Datum& datum, std::size_t index) {
				Scope* _scope = &datum.GetAsTable(index);
				_scope->_parent = nullptr;
				delete _scope;

				return false;
			});
			_attributes.Clear();
			_orderVector.Clear();
		}		
	}
	
	void Scope::DeepCopy(const Scope& rhs) {
		_orderVector.Reserve(rhs.Size());
		for (auto item : rhs._orderVector) {
			Datum& appendedDatum = Append(item->first);
			if (item->second.Type() != Datum::DatumTypes::Table) {
				appendedDatum = item->second;
			}
			else {
				appendedDatum.SetType(Datum::DatumTypes::Table);
				appendedDatum.Reserve(item->second.Size());
				for (std::size_t i = 0; i < item->second.Size(); ++i) {
					Scope* clone = item->second.GetAsTable(i).Clone();
					clone->_parent = this;
					appendedDatum.PushBack(*clone);
				}
			}
		}
	}
	
	void Scope::Reparent(Scope& oldScope) {
		if (oldScope._parent != nullptr) {
			auto catchReturn = oldScope.Orphan();
			delete catchReturn;
		}
		ForEachNestedScope([this](Datum& datum, std::size_t index) {
			datum.GetAsTable(index)._parent = this;
			return false;
		});
	}
	
	void Scope::ForEachNestedScope(NestedScopeFunction func) const {
		for (auto item : _orderVector) {
			if (item->second.Type() == Datum::DatumTypes::Table) {
				for (std::size_t i = 0; i < item->second.Size(); ++i) {
					bool breakNeeded = func(item->second, i);
					if (breakNeeded) return;
				}
			}
		}
	}
}
