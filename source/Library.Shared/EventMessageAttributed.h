/// <summary>
/// Declarations and inline definitions for EventMessageAttributed.
/// </summary>

#pragma once
#include "Attributed.h"
#include "Factory.h"
#include "GameState.h"

namespace FIEAGameEngine {
	class EventMessageAttributed : public Attributed {
		RTTI_DECLARATIONS(EventMessageAttributed, Attributed);

	public:
		/// <summary>
		/// The constructor for an EventMessageAttributed.S
		/// </summary>
		/// <param name="gameState">A reference to the game state.</param>
		/// <param name="subtype">The subtype specifier for this EventMessageAttributed to respond to.</param>
		EventMessageAttributed(std::string subtype = "");
		/// <summary>
		/// A copy constructor for a EventMessageAttributed that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The EventMessageAttributed to copy</param>
		EventMessageAttributed(const EventMessageAttributed& rhs) = default;
		/// <summary>
		/// The move constructor for EventMessageAttributed.
		/// </summary>
		/// <param name="rhs">The EventMessageAttributed to move data from</param>
		EventMessageAttributed(EventMessageAttributed&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an EventMessageAttributed that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The EventMessageAttributed to copy</param>
		/// <returns>A reference to the updated EventMessageAttributed</returns>
		EventMessageAttributed& operator=(const EventMessageAttributed& rhs) = default;
		/// <summary>
		/// The move assignment operator for a EventMessageAttributed that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The EventMessageAttributed to copy</param>
		/// <returns>A reference to the updated EventMessageAttributed</returns>
		EventMessageAttributed& operator=(EventMessageAttributed&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a EventMessageAttributed.
		/// </summary>
		virtual ~EventMessageAttributed() = default;

		/// <summary>
		/// The cloneable function of an EventMessageAttributed, made pure virtual because you cannot have a concreate EventMessageAttributed.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new EventMessageAttributed.</returns>
		[[nodiscard]] gsl::owner<EventMessageAttributed*> Clone() const override;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this EventMessageAttributed with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;
		/// <summary>
		/// Overriden RTTI functionality - Get a string representation of this EventMessageAttributed.
		/// </summary>
		/// <returns>A string representing this EventMessageAttributed.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Getter function for the subtype of this EventMessageAttributed.
		/// </summary>
		/// <returns>A const reference to the subtype of this EventMessageAttributed.</returns>
		inline const std::string& Subtype() const { return _subtype; };
		/// <summary>
		/// A setter for the gameState of this EventMessageAttributed.
		/// </summary>
		/// <param name="newGameState">The new subtype.</param>
		inline void SetSubtype(std::string newSubtype) { _subtype = std::move(newSubtype); };

	protected:
		/// <summary>
		/// The subtype this EventMessageAttributed matches to.
		/// </summary>
		std::string _subtype{ "" };
	};

	/// <summary>
	/// The generated concrete factory class that produces EventMessageAttributeds.
	/// </summary>
	CONCRETE_FACTORY(EventMessageAttributed, FIEAGameEngine::Scope)
}


