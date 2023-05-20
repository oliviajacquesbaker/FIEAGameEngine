/// <summary>
/// The declaration of the TypeManager and its relevant structure Signature.
/// </summary>

#pragma once
#include "HashMap.h"
#include "Vector.h"
#include "Datum.h"
#include "RTTI.h"

namespace FIEAGameEngine {
	struct Signature final {
		/// <summary>
		/// The name of the Signature.
		/// </summary>
		std::string _name;
		/// <summary>
		/// The type of the Signature.
		/// </summary>
		Datum::DatumTypes _type;
		/// <summary>
		/// The size of the Signature.
		/// </summary>
		std::size_t _size;
		/// <summary>
		/// The offset of the Signature.
		/// </summary>
		std::size_t _offset;

		/// <summary>
		/// The main constructor for a Signature.
		/// </summary>
		/// <param name="name">The name of the Signature.</param>
		/// <param name="type">The type of the Signature.</param>
		/// <param name="size">The size of the Signature.</param>
		/// <param name="offset">The offset of the Signature.</param>
		Signature(const std::string& name, Datum::DatumTypes type, std::size_t size, std::size_t offset);
		/// <summary>
		/// The default constructor for a Signature has been deleted.
		/// </summary>
		Signature() = delete;
		/// <summary>
		/// The copy constructor of a Signature. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="rhs">The Signature to copy.</param>
		Signature(const Signature& rhs) = default;
		/// <summary>
		/// The move constructor of a Signature. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="rhs">The Signature to move.</param>
		Signature(Signature&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator of a Signature. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="rhs">The Signature to copy.</param>
		Signature& operator=(const Signature& rhs) = default;
		/// <summary>
		/// The move assignment operator of a Signature. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="rhs">The Signature to move.</param>
		Signature& operator=(Signature&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a Signature. Using the compiler provided default implementation.
		/// </summary>
		~Signature() = default;

		/// <summary>
		/// Friend function that allows comparison between two Signatures.
		/// </summary>
		/// <param name="lhs">The left hand side Signature to compare.</param>
		/// <param name="rhs">The right hand side Signature to compare.</param>
		/// <returns>A boolean indicating if the two Signatures are equivilant.</returns>
		[[nodiscard]] friend bool operator==(const Signature& lhs, const Signature& rhs) {
			return (lhs._name == rhs._name && lhs._type == rhs._type, lhs._size == rhs._size, lhs._offset == rhs._offset);
		};
		/// <summary>
		/// Friend function that allows comparison between two Signatures.
		/// </summary>
		/// <param name="lhs">The left hand side Signature to compare.</param>
		/// <param name="rhs">The right hand side Signature to compare.</param>
		/// <returns>A boolean indicating if the two Signatures are not equivilant.</returns>
		[[nodiscard]] friend bool operator!=(const Signature& lhs, const Signature& rhs){
			return !(operator==(lhs, rhs));
		};
	};

	class TypeManager final{
	public:
		/// <summary>
		/// Add a type's signatures to the TypeManager. If the given typeID already exists in the TypeManager, it will return False.
		/// </summary>
		/// <param name="typeID">The typeID to use as a key.</param>
		/// <param name="signatures">The signatures to associate to this typeID</param>
		/// <returns>A boolean indicating if something was actually added.</returns>
		static bool Add(RTTI::IdType typeID, const Vector<Signature>& signatures);
		/// <summary>
		/// Add a type's signatures to the TypeManager using move semantics. If the given typeID already exists in the TypeManager, it will return False.
		/// </summary>
		/// <param name="typeID">The typeID to use as a key.</param>
		/// <param name="signatures">The signatures to associate to this typeID</param>
		/// <returns>A boolean indicating if something was actually added.</returns>
		static bool Add(RTTI::IdType typeID, Vector<Signature>&& signatures);
		/// <summary>
		/// Add a child type's signatures to the TypeManager. The parent's signatures will be added automatically based on the parentID given. If the given typeID already exists in the TypeManager, it will return False.
		/// </summary>
		/// <param name="typeID">The typeID to use as a key.</param>
		/// <param name="parentID">The typeID of this type's parent, to use to look up the parent's signatures.</param>
		/// <param name="signatures">The signatures to associate to this typeID</param>
		/// <returns>A boolean indicating if something was actually added.</returns>
		static bool Add(RTTI::IdType typeID, RTTI::IdType parentID, const Vector<Signature>& signatures);
		/// <summary>
		/// Add a child type's signatures to the TypeManager using move semantics. The parent's signatures will be added automatically based on the parentID given. If the given typeID already exists in the TypeManager, it will return False.
		/// </summary>
		/// <param name="typeID">The typeID to use as a key.</param>
		/// <param name="parentID">The typeID of this type's parent, to use to look up the parent's signatures.</param>
		/// <param name="signatures">The signatures to associate to this typeID</param>
		/// <returns>A boolean indicating if something was actually added.</returns>
		static bool Add(RTTI::IdType typeID, RTTI::IdType parentID, Vector<Signature>&& signatures);
		/// <summary>
		/// Remove a signature from the TypeManager.
		/// </summary>
		/// <param name="typeID">The typeID to remove on.</param>
		static void Remove(RTTI::IdType typeID);
		/// <summary>
		/// Clear the TypeManager of all signatures.
		/// </summary>
		static void Clear();

		/// <summary>
		/// Check if a typeID maps to a set of signatures in the TypeManager.
		/// </summary>
		/// <param name="typeID">The typeID to look up.</param>
		/// <returns>A boolean indicating whether this typeID is in the TypeManager.</returns>
		[[nodiscard]] static bool ContainsSignature(RTTI::IdType typeID);
		/// <summary>
		/// Get a set of signatures from the TypeManager.
		/// </summary>
		/// <param name="typeID">The typeID to look up.</param>
		/// <returns>The set of signatures associated with this typeID.</returns>
		[[nodiscard]] static const Vector<Signature>& GetSignatureByTypeID(RTTI::IdType typeID);
		/// <summary>
		/// Get all types and their signatures from the TypeManager.
		/// </summary>
		/// <returns>The contents of the TypeManager.</returns>
		[[nodiscard]] static const HashMap<RTTI::IdType, Vector<Signature>>& Types();
	private:
		/// <summary>
		/// The mapping of IDs to sets of signatures.
		/// </summary>
		inline static HashMap<RTTI::IdType, Vector<Signature>> _signatures;

		/// <summary>
		/// A helper function to add all inherited signatures from a parent type to the child.
		/// </summary>
		/// <param name="childID">The typeID of the child.</param>
		/// <param name="parentID">The typeID of the parent.</param>
		static void AddInheritedSignatures(RTTI::IdType childID, RTTI::IdType parentID);
	};

	/// <summary>
	/// A template function that allows for users to add to the TypeManager without directly calling it.
	/// </summary>
	template <typename Type>
	void RegisterType() {
		TypeManager::Add(Type::TypeIdClass(), Type::Signatures());
	}

	/// <summary>
	/// A template function that allows for users to add an inherited type to the TypeManager without directly calling it.
	/// </summary>
	template <typename Type, typename ParentType>
	void RegisterType() {
		TypeManager::Add(Type::TypeIdClass(), ParentType::TypeIdClass(), Type::Signatures());
	}
}
