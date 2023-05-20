/// <summary>
/// The definitions of the TypeManager and its relevant structure Signature.
/// </summary>

#include "pch.h"
#include "TypeManager.h"

namespace FIEAGameEngine {
	Signature::Signature(const std::string& name, Datum::DatumTypes type, std::size_t size, std::size_t offset):
		_name{name}, _type{type}, _size{size}, _offset{offset}
	{
	}

	bool TypeManager::Add(RTTI::IdType typeID, const Vector<Signature>& signatures) {
		return _signatures.Insert({ typeID, signatures }).second;
	}

	bool TypeManager::Add(RTTI::IdType typeID, Vector<Signature>&& signatures) {
		return _signatures.Insert({typeID, std::move(signatures) }).second;
	}

	bool TypeManager::Add(RTTI::IdType typeID, RTTI::IdType parentID, const Vector<Signature>& signatures) {
#ifdef USE_EXCEPTIONS
		if (!ContainsSignature(parentID)) throw std::runtime_error("Given parent ID is not registered in the Type Manager.");
#endif // USE_EXCEPTIONS

		bool added = _signatures.Insert({ typeID, signatures }).second;
		if(added) AddInheritedSignatures(typeID, parentID);
		return added;
	}

	bool TypeManager::Add(RTTI::IdType typeID, RTTI::IdType parentID, Vector<Signature>&& signatures) {
#ifdef USE_EXCEPTIONS
		if (!ContainsSignature(parentID)) throw std::runtime_error("Given parent ID is not registered in the Type Manager.");
#endif // USE_EXCEPTIONS

		bool added = _signatures.Insert({ typeID, std::move(signatures) }).second;
		if(added) AddInheritedSignatures(typeID, parentID);
		return added;
	}

	void TypeManager::Remove(RTTI::IdType typeID) {
		_signatures.Remove(typeID);
	}

	void TypeManager::Clear() {
		_signatures.Clear();
	}

	bool TypeManager::ContainsSignature(RTTI::IdType typeID) {
		return _signatures.ContainsKey(typeID);
	}

	const Vector<Signature>& TypeManager::GetSignatureByTypeID(RTTI::IdType typeID) {
		return _signatures.At(typeID);
	}

	const HashMap<RTTI::IdType, Vector<Signature>>& TypeManager::Types() {
		return _signatures;
	}

	void TypeManager::AddInheritedSignatures(RTTI::IdType childID, RTTI::IdType parentID) {
		auto& signatureVector = _signatures.At(parentID);
		for (auto& signature : signatureVector) {
			_signatures[childID].PushBack(signature);
		}
	}
}


