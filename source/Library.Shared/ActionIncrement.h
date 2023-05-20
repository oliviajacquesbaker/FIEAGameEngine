/// <summary>
/// Declarations for an ActionIncrement.
/// </summary>

#pragma once
#include "Action.h"

namespace FIEAGameEngine {
    class ActionIncrement : public Action {
		RTTI_DECLARATIONS(ActionIncrement, Action);

	public:
		/// <summary>
		/// Default constructor for a ActionIncrement. If no name is given, it will default to an empty string.
		/// </summary>
		ActionIncrement(const std::string& name = "");
		/// <summary>
		/// A copy constructor for a ActionIncrement that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The ActionIncrement to copy</param>
		ActionIncrement(const ActionIncrement& rhs) = default;
		/// <summary>
		/// The move constructor for ActionIncrement.
		/// </summary>
		/// <param name="rhs">The ActionIncrement to move data from</param>
		ActionIncrement(ActionIncrement&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an ActionIncrement that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The ActionIncrement to copy</param>
		/// <returns>A reference to the updated ActionIncrement</returns>
		ActionIncrement& operator=(const ActionIncrement& rhs) = default;
		/// <summary>
		/// The move assignment operator for a ActionIncrement that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The ActionIncrement to copy</param>
		/// <returns>A reference to the updated ActionIncrement</returns>
		ActionIncrement& operator=(ActionIncrement&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a ActionIncrement.
		/// </summary>
		virtual ~ActionIncrement() = default;

		/// <summary>
		/// The cloneable function of an ActionIncrement.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new ActionIncrement.</returns>
		[[nodiscard]] gsl::owner<ActionIncrement*> Clone() const override;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		/// <summary>
		/// Update this ActionIncrement.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime);

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this ActionList with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;

		/// <summary>
		/// Getter function for the incrementAmount of this action.
		/// </summary>
		/// <returns>A const reference to the incrementAmount of this action.</returns>
		inline int IncrementAmount() const { return incrementAmount; };
		/// <summary>
		/// A setter for the incrementAmount of this action.
		/// </summary>
		/// <param name="newName">The new incrementAmount.</param>
		inline void SetIncrementAmount(int newIncrement) { incrementAmount = newIncrement; };
		/// <summary>
		/// Getter function for the target of this action.
		/// </summary>
		/// <returns>A const reference to the target of this action.</returns>
		inline const std::string& Target() const { return target; };
		/// <summary>
		/// A setter for the target of this action.
		/// </summary>
		/// <param name="newName">The new target.</param>
		inline void SetTarget(const std::string& newTarget) { target = newTarget; };
		/// <summary>
		/// Getter function for the targetIndex of this action.
		/// </summary>
		/// <returns>A const reference to the targetIndex of this action.</returns>
		inline int TargetIndex() const { return targetIndex; };
		/// <summary>
		/// A setter for the targetIndex of this action.
		/// </summary>
		/// <param name="newName">The new targetIndex.</param>
		inline void SetTargetIndex(int newIndex) { targetIndex = newIndex; };

	protected:
		/// <summary>
		/// Protected constructor for a ActionIncrement that also takes a RTTI type. If no name is given, it will default to an empty string.
		/// </summary>
		ActionIncrement(RTTI::IdType typeID, const std::string& name = "");

		/// <summary>
		/// The value by which the target should be incremented by.
		/// </summary>
		int incrementAmount = 1;
		/// <summary>
		/// The name of the variable in scope to increment.
		/// </summary>
		std::string target = "";
		/// <summary>
		/// The index of the target datum to update
		/// </summary>
		int targetIndex = 0;
	};

	/// <summary>
	/// The generated concrete factory class that produces ActionLists.
	/// </summary>
	CONCRETE_FACTORY(ActionIncrement, FIEAGameEngine::Scope)
}


