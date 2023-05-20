/// <summary>
/// Declarations for an Action.
/// </summary>

#pragma once
#include <cstddef>
#include <cassert>
#include "Factory.h"
#include "Attributed.h"
#include "GameTime.h"

namespace FIEAGameEngine {
	class Action : public Attributed {
		RTTI_DECLARATIONS(Action, Attributed);

	public:
		/// <summary>
		/// A copy constructor for a Action that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The Action to copy</param>
		Action(const Action& rhs) = default;
		/// <summary>
		/// The move constructor for Action.
		/// </summary>
		/// <param name="rhs">The Action to move data from</param>
		Action(Action&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an Action that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Action to copy</param>
		/// <returns>A reference to the updated Action</returns>
		Action& operator=(const Action& rhs) = default;
		/// <summary>
		/// The move assignment operator for a Action that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The Action to copy</param>
		/// <returns>A reference to the updated Action</returns>
		Action& operator=(Action&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a Action.
		/// </summary>
		virtual ~Action() = default;

		/// <summary>
		/// The cloneable function of an Action, made pure virtual because you cannot have a concreate Action.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new Action.</returns>
		[[nodiscard]] gsl::owner<Action*> Clone() const override = 0;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		/// <summary>
		/// Update this Action.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime) = 0;

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this Action with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;
		/// <summary>
		/// Overriden RTTI functionality - Get a string representation of this Action.
		/// </summary>
		/// <returns>A string representing this Action.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Getter function for the name of this action.
		/// </summary>
		/// <returns>A const reference to the name of this action.</returns>
		inline const std::string& Name() const { return name; };
		/// <summary>
		/// A setter for the name of this action.
		/// </summary>
		/// <param name="newName">The new name.</param>
		inline void SetName(const std::string& newName) { name = newName; };

	protected:
		/// <summary>
		/// Default constructor for a Action. If no name is given, it will default to an empty string.
		/// </summary>
		explicit Action(RTTI::IdType typeID, const std::string& name = "");

		/// <summary>
		/// The name of this GameObject.
		/// </summary>
		std::string name; 
	};
}

