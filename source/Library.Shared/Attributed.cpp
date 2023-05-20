/// <summary>
/// The definitions of the class Attributed, which is an object that is a Scope but can generate attributes from class data.
/// </summary>

#include "pch.h"
#include "Attributed.h"

using namespace std::string_literals;

namespace FIEAGameEngine {
	RTTI_DEFINITIONS(Attributed);

	Attributed::Attributed(RTTI::IdType typeID) {
		Populate(typeID);
	}

	Attributed::Attributed(const Attributed& rhs) :
		Scope(rhs)
	{
		UpdateExternalStorage(rhs.TypeIdInstance());
	}

	Attributed::Attributed(Attributed&& rhs) noexcept :
		Scope(std::move(rhs))
	{
		UpdateExternalStorage(rhs.TypeIdInstance());
	}

	Attributed& Attributed::operator=(const Attributed& rhs) {
		if (this != &rhs) {
			Scope::operator=(rhs);
			UpdateExternalStorage(rhs.TypeIdInstance());
		}
		return *this;
	}

	Attributed& Attributed::operator=(Attributed&& rhs) noexcept {
		if (this != &rhs) {
			Scope::operator=(std::move(rhs));
			UpdateExternalStorage(TypeIdInstance());
		}
		return *this;
	}

	void Attributed::Populate(RTTI::IdType typeID) {
		(*this)["this"s] = this;
		ForEachSignature(typeID, [this](const Signature& signature) {
			(*this)[signature._name].SetType(signature._type);
			if (signature._type != Datum::DatumTypes::Table) {
				void* ptr = reinterpret_cast<std::byte*>(this) + signature._offset;
				(*this)[signature._name].SetStorage(ptr, signature._size, signature._type);
			}
			return false;
		});
	}

	void Attributed::UpdateExternalStorage(RTTI::IdType typeID) {
		(*this)["this"s] = this;
		ForEachSignature(typeID, [this](const Signature& signature) {
			if (signature._type != Datum::DatumTypes::Table) { 
				void* ptr = reinterpret_cast<std::byte*>(this) + signature._offset;
				this->At(signature._name).SetStorage(ptr, signature._size, signature._type);
			}
			return false;
		});
	}

	bool Attributed::IsAttribute(const std::string& key) const {
		return (Find(key) != nullptr);
	}
	
	bool Attributed::IsPrescribedAttribute(const std::string& key) const {
		bool isPrescribed = (key == "this");
		if (!isPrescribed) {
			ForEachSignature(TypeIdInstance(), [&key, &isPrescribed](const Signature& signature) {
				if (signature._name == key) {
					isPrescribed = true;
					return true;
				}
				return false;
			});
		}		
		return isPrescribed;
	}
	
	bool Attributed::IsAuxiliaryAttribute(const std::string& key) const {
		return (IsAttribute(key) && !IsPrescribedAttribute(key));
	}

	Datum& Attributed::AppendAuxiliaryAttribute(const std::string& key) {
#ifdef USE_EXCEPTIONS
		if (IsPrescribedAttribute(key)) throw std::runtime_error("Given key name is a prescribed attribute.");
#endif // USE_EXCEPTIONS
		return Append(key);
	}

	void Attributed::ForEachAttribute(AttributeFunction func) const {
		for (auto& attribute : _orderVector) { 
			bool breakNeeded = func(attribute->first, attribute->second);
			if (breakNeeded) return;
		}
	}	

	void Attributed::ForEachPrescribedAttribute(AttributeFunction func) const {
		std::size_t prescribedAttributeCount = TypeManager::GetSignatureByTypeID(TypeIdInstance()).Size();
		for (std::size_t i = 0; i <= prescribedAttributeCount; ++i) {
			bool breakNeeded = func(_orderVector[i]->first, _orderVector[i]->second);
			if (breakNeeded) return;
		}
	}

	void Attributed::ForEachAuxiliaryAttribute(AttributeFunction func) const {
		std::size_t prescribedAttributeCount = TypeManager::GetSignatureByTypeID(TypeIdInstance()).Size();
		for (std::size_t i = prescribedAttributeCount + 1; i < _orderVector.Size(); ++i) {
			bool breakNeeded = func(_orderVector[i]->first, _orderVector[i]->second);
			if (breakNeeded) return;
		}
	}

	void Attributed::ForEachSignature(RTTI::IdType signatureID, SignatureFunction func) const {
		auto& signatures = TypeManager::GetSignatureByTypeID(signatureID);
		for (auto& signature : signatures) {
			bool breakNeeded = func(signature);
			if (breakNeeded) return;
		}
	}

	std::string Attributed::ToString() const {
		std::string strValue = "this: (pointer to this object) ";
		//skip the first "this" attribute in order to avoid an infinite loop of getting the ToString on ourselves.
		for (std::size_t i = 1; i < Size(); ++i) { 
			strValue += "\n" + _orderVector[i]->first + ":"s;
			for (std::size_t j = 0; j < _orderVector[i]->second.Size(); ++j) {
				strValue += " ("s + _orderVector[i]->second.ToString(j) + ") "s;
			}
		}
		return strValue;
	}
}

