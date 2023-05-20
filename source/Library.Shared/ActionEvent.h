/// <summary>
/// Declarations and inline definitions for ActionEvent.
/// </summary>

#pragma once
#include "Action.h"
#include "Event.h"
#include "EventQueue.h"
#include "EventMessageAttributed.h"

namespace FIEAGameEngine {
	class ActionEvent : public Action {
		RTTI_DECLARATIONS(ActionEvent, Action);

	public:
		/// <summary>
		/// Default constructor for a ActionEvent. If no name is given, it will default to an empty string.
		/// </summary>
		ActionEvent(const std::string& name = "", std::string subtype = "", int delay = 0);
		/// <summary>
		/// A copy constructor for a ActionEvent that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The ActionEvent to copy</param>
		ActionEvent(const ActionEvent& rhs) = default;
		/// <summary>
		/// The move constructor for ActionEvent.
		/// </summary>
		/// <param name="rhs">The ActionEvent to move data from</param>
		ActionEvent(ActionEvent&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an ActionEvent that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The ActionEvent to copy</param>
		/// <returns>A reference to the updated ActionEvent</returns>
		ActionEvent& operator=(const ActionEvent& rhs) = default;
		/// <summary>
		/// The move assignment operator for a ActionEvent that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The ActionEvent to copy</param>
		/// <returns>A reference to the updated ActionEvent</returns>
		ActionEvent& operator=(ActionEvent&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a ActionEvent.
		/// </summary>
		virtual ~ActionEvent() = default;

		/// <summary>
		/// The cloneable function of an ActionEvent.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new ActionEvent.</returns>
		[[nodiscard]] gsl::owner<ActionEvent*> Clone() const override;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		/// <summary>
		/// Update this ActionEvent.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime);

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this ActionEvent with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;

		/// <summary>
		/// Getter function for the subtype of this ActionEvent.
		/// </summary>
		/// <returns>A const reference to the subtype of this ActionEvent.</returns>
		inline const std::string& Subtype() const { return _subtype; };
		/// <summary>
		/// A setter for the gameState of this ActionEvent.
		/// </summary>
		/// <param name="newGameState">The new subtype.</param>
		inline void SetSubtype(std::string newSubtype) { _subtype = std::move(newSubtype); };
		/// <summary>
		/// Getter function for the delay of this ActionEvent.
		/// </summary>
		/// <returns>The delay of this ActionEvent.</returns>
		inline int Delay() const { return _delay; };
		/// <summary>
		/// A setter for the gameState of this ActionEvent.
		/// </summary>
		/// <param name="newDelay">The new delay.</param>
		inline void SetDelay(int newDelay) { _delay = std::move(newDelay); };

	protected:
		/// <summary>
		/// Protected constructor for a ActionEvent that also takes a RTTI type. If no name is given, it will default to an empty string.
		/// </summary>
		ActionEvent(RTTI::IdType typeID, const std::string& name = "", std::string subtype = "", int delay = 0);

		/// <summary>
		/// The subtype this ActionEvent matches to.
		/// </summary>
		std::string _subtype{ "" };
		/// <summary>
		/// The delay for the events this ActionEvent enqueues.
		/// </summary>
		int _delay{ 0 };
	};

	/// <summary>
	/// The generated concrete factory class that produces ActionLists.
	/// </summary>
	CONCRETE_FACTORY(ActionEvent, FIEAGameEngine::Scope)
}


