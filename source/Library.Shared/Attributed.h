/// <summary>
/// The declaration of the class Attributed, which is an object that is a Scope but can generate attributes from class data.
/// </summary>

#pragma once

#include <cstddef>
#include <string>
#include <cassert>
#include <stdexcept>
#include "gsl/gsl"
#include "Scope.h"
#include "TypeManager.h"

namespace FIEAGameEngine {
	class Attributed : public Scope {
		RTTI_DECLARATIONS(Attributed, Scope);

	public:
		/// <summary>
		/// The default constructor for an Attributed has been deleted.
		/// </summary>
		Attributed() = delete;
		/// <summary>
		/// A copy constructor for an Attributed that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The Attributed to copy</param>
		Attributed(const Attributed& rhs);
		/// <summary>
		/// The move constructor for Attributed.
		/// </summary>
		/// <param name="rhs">The Attributed to move data from</param>
		Attributed(Attributed&& rhs) noexcept;
		/// <summary>
		/// The copy assignment operator for an Attributed that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Attributed to copy</param>
		/// <returns>A reference to the updated Attributed</returns>
		Attributed& operator=(const Attributed& rhs);
		/// <summary>
		/// The move assignment operator for an Attributed that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The Attributed to copy</param>
		/// <returns>A reference to the updated Attributed</returns>
		Attributed& operator=(Attributed&& rhs) noexcept;
		/// <summary>
		/// The destructor for an Attributed. Using the compiler provided default behavior.
		/// </summary>
		virtual ~Attributed() = default;
		/// <summary>
		/// The cloneable function of an Attributed, made pure virtual because you cannot have a concreate Attributed.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new Attributed.</returns>
		virtual [[nodiscard]] gsl::owner<Attributed*> Clone() const override = 0;

		/// <summary>
		/// Check if the given key is the name of an attribute.
		/// </summary>
		/// <param name="key">The name to look up.</param>
		/// <returns>A boolean indicating if the given key is the name of an attribute.</returns>
		[[nodiscard]] bool IsAttribute(const std::string& key) const;
		/// <summary>
		/// Check if the given key is the name of a prescribed attribute.
		/// </summary>
		/// <param name="key">The name to look up.</param>
		/// <returns>A boolean indicating if the given key is the name of a prescribed attribute.</returns>
		[[nodiscard]] bool IsPrescribedAttribute(const std::string& key) const;
		/// <summary>
		/// Check if the given key is the name of an auxiliary attribute.
		/// </summary>
		/// <param name="key">The name to look up.</param>
		/// <returns>A boolean indicating if the given key is the name of an auxiliary attribute.</returns>
		[[nodiscard]] bool IsAuxiliaryAttribute(const std::string& key) const;

		/// <summary>
		/// A thin wrapper for Scope's Append. You cannot append something with the same name as a prescribed attribute.
		/// </summary>
		/// <param name="key">The string name to append on.</param>
		/// <returns>The Datum, either newly created or preexisting, associated with the key.</returns>
		Datum& AppendAuxiliaryAttribute(const std::string& key);

		using AttributeFunction = std::function<bool(const std::string&, Datum&)>;
		/// <summary>
		/// A helper function that iterates through every attribute for this Attributed object's typeID and calls the passed in functor on it.
		/// </summary>
		/// <param name="func">The functor to call.</param>
		void ForEachAttribute(AttributeFunction func) const;
		/// <summary>
		/// A helper function that iterates through every prescribed attribute for this Attributed object's typeID and calls the passed in functor on it.
		/// </summary>
		/// <param name="func">The functor to call.</param>
		void ForEachPrescribedAttribute(AttributeFunction func) const;
		/// <summary>
		/// A helper function that iterates through every auxiliary attribute for this Attributed object's typeID and calls the passed in functor on it.
		/// </summary>
		/// <param name="func">The functor to call.</param>
		void ForEachAuxiliaryAttribute(AttributeFunction func) const;

		/// <summary>
		/// Overriden RTTI functionality - Get a string representation of this Attributed.
		/// </summary>
		/// <returns>A string representing this Scope.</returns>
		std::string ToString() const override;

	protected:
		/// <summary>
		/// The constructor for an Attributed, hich takes in the type ID of the kind of object that is truly being instantiated.
		/// </summary>
		/// <param name="size">The type ID of the derived final class that is actually being instantiated.</param>
		explicit Attributed(RTTI::IdType typeID);

	private:
		/// <summary>
		/// Helper function to append all attributes from the signature of the given type.
		/// </summary>
		/// <param name="typeID">The type to look up the signature to append on.</param>
		void Populate(RTTI::IdType typeID);
		/// <summary>
		/// Helper function to redirect pointers to external storage from a copied/moved object to this one.
		/// </summary>
		/// <param name="typeID">The type to look up the signatures for.</param>
		void UpdateExternalStorage(RTTI::IdType typeID);

		/// <summary>
		/// A helper function that iterates through every Signature for this Attributed object's typeID and calls the passed in functor on it.
		/// </summary>
		/// <param name="func">The functor to call.</param>
		using SignatureFunction = std::function<bool(const Signature&)>;
		void ForEachSignature(RTTI::IdType signatureID, SignatureFunction func) const;
	};
}

