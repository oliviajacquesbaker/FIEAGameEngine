/// <summary>
/// Declarations for Reaction.
/// </summary>

#pragma once
#include "ActionList.h"
#include "Event.h"

namespace FIEAGameEngine {
	class Reaction : public ActionList {
		RTTI_DECLARATIONS(Reaction, ActionList);

	public:
		/// <summary>
		/// The default constructor for a Reaction has been deleted.
		/// </summary>
		Reaction() = delete;
		/// <summary>
		/// A copy constructor for a Reaction that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The Reaction to copy</param>
		Reaction(const Reaction& rhs) = default;
		/// <summary>
		/// The move constructor for Reaction.
		/// </summary>
		/// <param name="rhs">The Reaction to move data from</param>
		Reaction(Reaction&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an Reaction that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Reaction to copy</param>
		/// <returns>A reference to the updated Reaction</returns>
		Reaction& operator=(const Reaction& rhs) = default;
		/// <summary>
		/// The move assignment operator for a Reaction that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The Reaction to copy</param>
		/// <returns>A reference to the updated Reaction</returns>
		Reaction& operator=(Reaction&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a Reaction.
		/// </summary>
		virtual ~Reaction() = default;

		/// <summary>
		/// Update this Reaction.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime) override = 0;

	protected:
		/// <summary>
		/// Default constructor for a Reaction. If no name is given, it will default to an empty string.
		/// </summary>
		explicit Reaction(RTTI::IdType typeID, const std::string& name = "");
	};
}


