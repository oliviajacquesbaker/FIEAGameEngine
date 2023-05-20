/// <summary>
/// Declarations for an ActionWhile.
/// </summary>

#pragma once
#include "Action.h"

namespace FIEAGameEngine {
	class ActionWhile : public Action {
		RTTI_DECLARATIONS(ActionWhile, Action);

	public:
		/// <summary>
		/// Default constructor for a ActionListWhile. If no name is given, it will default to an empty string.
		/// </summary>
		ActionWhile(const std::string& name = "");
		/// <summary>
		/// A copy constructor for a ActionListWhile that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The ActionListWhile to copy</param>
		ActionWhile(const ActionWhile& rhs) = default;
		/// <summary>
		/// The move constructor for ActionListWhile.
		/// </summary>
		/// <param name="rhs">The ActionListWhile to move data from</param>
		ActionWhile(ActionWhile&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an ActionListWhile that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The ActionListWhile to copy</param>
		/// <returns>A reference to the updated ActionListWhile</returns>
		ActionWhile& operator=(const ActionWhile& rhs) = default;
		/// <summary>
		/// The move assignment operator for a ActionListWhile that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The ActionListWhile to copy</param>
		/// <returns>A reference to the updated ActionListWhile</returns>
		ActionWhile& operator=(ActionWhile&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a ActionListWhile.
		/// </summary>
		virtual ~ActionWhile() = default;

		/// <summary>
		/// The cloneable function of an ActionListWhile.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new ActionListWhile.</returns>
		[[nodiscard]] gsl::owner<ActionWhile*> Clone() const override;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		/// <summary>
		/// Update this ActionListWhile.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime);

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this ActionList with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;

		/// <summary>
		/// Getter function for the condition of this action.
		/// </summary>
		/// <returns>A const reference to the condition of this action.</returns>
		inline int Condition() const { return condition; };
		/// <summary>
		/// A setter for the condition of this action.
		/// </summary>
		/// <param name="newName">The new condition.</param>
		inline void SetCondition(int newCondition) {condition = newCondition; };

	protected:
		/// <summary>
		/// Protected constructor for a ActionListWhile that also takes a RTTI type. If no name is given, it will default to an empty string.
		/// </summary>
		ActionWhile(RTTI::IdType typeID, const std::string& name = "");
		/// <summary>
		/// Helper function to run the given block of actions.
		/// </summary>
		/// <param name="gameTime">The current game time.</param>
		void RunActionsBlock(Datum* actions, const GameTime& gameTime);

		/// <summary>
		/// The condition indicating if this action should execute.
		/// </summary>
		int condition = 0;

	private:
		/// <summary>
		/// String key for the preamble attribute.
		/// </summary>
		inline static const std::string preambleKey = { "preamble" };
		/// <summary>
		/// String key for the loop attribute.
		/// </summary>
		inline static const std::string loopKey = { "loop" };
		/// <summary>
		/// String key for the increment attribute.
		/// </summary>
		inline static const std::string incrementKey = { "increment" };
		/// <summary>
		/// The cached datum for the preamble attribute.
		/// </summary>
		Datum* preambleDatum{ nullptr };
		/// <summary>
		/// The cached datum for the loop attribute.
		/// </summary>
		Datum* loopDatum{ nullptr };
		/// <summary>
		/// The cached datum for the increment attribute.
		/// </summary>
		Datum* incrementDatum{ nullptr };
	};

	/// <summary>
	/// The generated concrete factory class that produces ActionLists.
	/// </summary>
	CONCRETE_FACTORY(ActionWhile, FIEAGameEngine::Scope)
}


