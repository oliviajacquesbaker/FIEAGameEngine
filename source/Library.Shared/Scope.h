/// <summary>
/// The declaration of the class Scope, which is an object that stores string key-datum pairs.
/// </summary>

#pragma once

#include <cstddef>
#include <string>
#include <cassert>
#include <stdexcept>
#include "gsl/gsl"
#include "RTTI.h"
#include "Datum.h"
#include "Vector.h"
#include "HashMap.h"

namespace FIEAGameEngine {
	class Scope : public RTTI {
		RTTI_DECLARATIONS(Scope, RTTI);

	public:
		/// <summary>
		/// Default constructor for a Scope. If no size is given, it will default to zero.
		/// </summary>
		/// <param name="size">The initial capacity of the scope.</param>
		explicit Scope(std::size_t size = 0);
		/// <summary>
		/// A copy constructor for a Scope that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The Scope to copy</param>
		Scope(const Scope& rhs);
		/// <summary>
		/// The move constructor for Scope.
		/// </summary>
		/// <param name="rhs">The Scope to move data from</param>
		Scope(Scope&& rhs) noexcept;
		/// <summary>
		/// The copy assignment operator for an Scope that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Scope to copy</param>
		/// <returns>A reference to the updated Scope</returns>
		Scope& operator=(const Scope& rhs);
		/// <summary>
		/// The move assignment operator for a Scope that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The Scope to copy</param>
		/// <returns>A reference to the updated Scope</returns>
		Scope& operator=(Scope&& rhs) noexcept;
		/// <summary>
		/// The destructor for a Scope.
		/// </summary>
		virtual ~Scope();
		/// <summary>
		/// The cloneable function of a Scope, which creates a new Scope on the heap as a copy of this one and returns the pointer to it.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new Scope.</returns>
		virtual [[nodiscard]] gsl::owner<Scope*> Clone() const;

		/// <summary>
		/// Find the Datum associated with the given key within this Scope. Returns nullptr if not found.
		/// </summary>
		/// <param name="key">The string key to look for.</param>
		/// <returns>The address of the Datum associated with the key.</returns>
		[[nodiscard]] Datum* Find(const std::string& key);
		/// <summary>
		/// Find the Datum associated with the given key within this Scope. Returns nullptr if not found.
		/// </summary>
		/// <param name="key">The string key to look for.</param>
		/// <returns>The address of the constant Datum associated with the key.</returns>
		[[nodiscard]] const Datum* Find(const std::string& key) const;
		/// <summary>
		/// Find the Datum associated with the given key within the closest related Scope, starting with this one and moving up the hierarchy. Returns nullptr if not found.
		/// </summary>
		/// <param name="key">The string key to look for.</param>
		/// <returns>The address of the Datum associated with the key.</returns>
		[[nodiscard]] Datum* Search(const std::string& key);
		/// <summary>
		/// Find the Datum associated with the given key within the closest related Scope, starting with this one and moving up the hierarchy. Returns nullptr if not found.
		/// </summary>
		/// <param name="key">The string key to look for.</param>
		/// <param name="foundScope">Output parameter indicating the Scope in which the Datum was found.</param>
		/// <returns>The address of the Datum associated with the key.</returns>
		[[nodiscard]] Datum* Search(const std::string& key, Scope*& foundScope);

		/// <summary>
		/// Get the current size of the Scope.
		/// </summary>
		/// <returns>The current size of the Scope.</returns>
		[[nodiscard]] std::size_t Size() const;
		/// <summary>
		/// Check if the Scope is currently empty.
		/// </summary>
		/// <returns>A boolean indicating if the Scope is currently empty.</returns>
		[[nodiscard]] bool IsEmpty() const;
		/// <summary>
		/// Clear the Scope of all string-datum pairs.
		/// </summary>
		void Clear();

		/// <summary>
		/// Append an empty Datum with the given name to the Scope. If the given name already exists as an entry in the Scope, it will return the existing Datum and perform no insert.
		/// </summary>
		/// <param name="key">The string name to append on.</param>
		/// <returns>The Datum, either newly created or preexisting, associated with the key.</returns>
		Datum& Append(const std::string& key);
		/// <summary>
		/// Create a new nested Scope and append it to the Scope, associating it with the given key. The created Scope is owned by the parent Scope.
		/// </summary>
		/// <param name="key">The string key to append on.</param>
		/// <returns>A reference to the newly created Scope.</returns>
		Scope& AppendScope(const std::string& key);
		/// <summary>
		/// Transfer ownership of a heap allocated Scope to belong to this Scope.
		/// </summary>
		/// <param name="newChild">The heap allocated Scope.</param>
		/// <param name="childName">The string key to associate with the new child Scope.</param>
		void Adopt(Scope& newChild, const std::string& childName);
		/// <summary>
		/// Release ownership of this Scope, removing the reference to this scope from its parent if it has one.
		/// </summary>
		/// <returns>A gsl::owner wrapping a pointer to this Scope - the context that calls this function is responsible for catching this and managing its memory.</returns>
		[[nodiscard]] gsl::owner<Scope*> Orphan(); 
		/// <summary>
		/// Get the parent of this Scope.
		/// </summary>
		/// <returns>A pointer to the parent of this Scope.</returns>
		[[nodiscard]] Scope* GetParent() const;
		/// <summary>
		/// Check if the given scope is an ancestor of this one.
		/// </summary>
		/// <param name="otherScope">The Scope to check against.</param>
		/// <returns>A boolean indicating if this Scope is an ancestor of the given one.</returns>
		[[nodiscard]] bool IsAncestorOf(const Scope& otherScope) const;
		/// <summary>
		/// Check if the given scope is a descendant of this one.
		/// </summary>
		/// <param name="otherScope">The Scope to check against.</param>
		/// <returns>A boolean indicating if this Scope is a descendant of the given one.</returns>
		[[nodiscard]] bool IsDescendantOf(const Scope& otherScope) const;

		/// <summary>
		/// Accesses the Datum at the given key.
		/// </summary>
		/// <param name="key">The string name to look up.</param>
		/// <returns>The Datum associated with the key.</returns>
		[[nodiscard]] Datum& At(const std::string& key);
		/// <summary>
		/// Accesses the Datum at the given key.
		/// </summary>
		/// <param name="key">The string name to look up.</param>
		/// <returns>The const Datum associated with the key.</returns>
		[[nodiscard]] const Datum& At(const std::string& key) const;
		/// <summary>
		/// A wrapper for Append - Append an empty Datum with the given name to the Scope. If the given name already exists as an entry in the Scope, it will return the existing Datum and perform no insert.
		/// </summary>
		/// <param name="key">The string name to append on</param>
		/// <returns>The Datum, either newly created or preexisting, associated with the key.</returns>
		[[nodiscard]] Datum& operator[](const std::string& key);
		/// <summary>
		/// Retrieve a Datum given its position in the order vector.
		/// </summary>
		/// <param name="index">The index to retrieve on from the order vector.</param>
		/// <returns>A reference to the Datum that was a part of the indicated entry.</returns>
		[[nodiscard]] Datum& operator[](std::size_t index);
		/// <summary>
		/// Retrieve a Datum given its position in the order vector.
		/// </summary>
		/// <param name="index">The index to retrieve on from the order vector.</param>
		/// <returns>A const reference to the Datum that was a part of the indicated entry.</returns>
		[[nodiscard]] const Datum& operator[](std::size_t index) const;
		/// <summary>
		/// Compare the equality of this Scope to another.
		/// </summary>
		/// <param name="rhs">The Scope to compare against.</param>
		/// <returns>A boolean indicating the equality of this Scope and the other.</returns>
		[[nodiscard]] bool operator==(const Scope& rhs) const;
		/// <summary>
		/// Compare the inequality of this Scope to another.
		/// </summary>
		/// <param name="rhs">The Scope to compare against.</param>
		/// <returns>A boolean indicating the inequality of this Scope and the other.</returns>
		[[nodiscard]] bool operator!=(const Scope& rhs) const;

		/// <summary>
		/// Overriden RTTI equality functionality - compare the equality of this Scope against another RTTI object.
		/// </summary>
		/// <param name="rhs">A pointer to the RTTI object to compare against.</param>
		/// <returns>A boolean indicating equality.</returns>
		[[nodiscard]] bool Equals(const RTTI* rhs) const override;
		/// <summary>
		/// Overriden RTTI functionality - Get a string representation of this Scope.
		/// </summary>
		/// <returns>A string representing this Scope.</returns>
		[[nodiscard]] std::string ToString() const override;
	protected:
		/// <summary>
		/// A vector of pointers to the attributes of this Scope, maintaining the order in which they were appended.
		/// </summary>
		Vector<std::pair<const std::string, Datum>*> _orderVector;
	private:
		/// <summary>
		/// A pointer to this Scope's parent.
		/// </summary>
		Scope* _parent{ nullptr };
		/// <summary>
		/// The map of Datums to their string names.
		/// </summary>
		HashMap<std::string, Datum> _attributes;

		/// <summary>
		/// A helper function to deep copy a Scope.
		/// </summary>
		/// <param name="rhs">The Scope to copy.</param>
		void DeepCopy(const Scope& rhs);
		/// <summary>
		/// A helper function for move semantics that reparents all nested scopes to refer to the newly moved to object.
		/// </summary>
		/// <param name="rhs">The Scope being moved from.</param>
		void Reparent(Scope& rhs); 
		/// <summary>
		/// Find the Datum the indicated Scope is a part of.
		/// </summary>
		/// <param name="address">The address of the Scope to be looking for.</param>
		/// <returns>A pair containing the Datum and index at which the Scope was found in.</returns>
		[[nodiscard]] std::pair<Datum*, std::size_t> FindContainedScope(Scope* const address) const;

		/// <summary>
		/// A helper function that iterates through every nested Scope within this one and calls the passed in functor on it.
		/// </summary>
		/// <param name="func">The functor to call.</param>
		using NestedScopeFunction = std::function<bool(Datum&, std::size_t)>;
		void ForEachNestedScope(NestedScopeFunction func) const;
	};
}

#include "Scope.inl"