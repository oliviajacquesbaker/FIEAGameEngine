#pragma once
#include "ActionList.h"

namespace FIEAGameEngine {
	class ActionListWhile : public ActionList {
		RTTI_DECLARATIONS(ActionListWhile, Action);

	public:
		/// <summary>
		/// Default constructor for a ActionListWhile. If no name is given, it will default to an empty string.
		/// </summary>
		ActionListWhile(const std::string& name = "");
		/// <summary>
		/// A copy constructor for a ActionListWhile that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The ActionListWhile to copy</param>
		ActionListWhile(const ActionListWhile& rhs) = default;
		/// <summary>
		/// The move constructor for ActionListWhile.
		/// </summary>
		/// <param name="rhs">The ActionListWhile to move data from</param>
		ActionListWhile(ActionListWhile&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an ActionListWhile that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The ActionListWhile to copy</param>
		/// <returns>A reference to the updated ActionListWhile</returns>
		ActionListWhile& operator=(const ActionListWhile& rhs) = default;
		/// <summary>
		/// The move assignment operator for a ActionListWhile that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The ActionListWhile to copy</param>
		/// <returns>A reference to the updated ActionListWhile</returns>
		ActionListWhile& operator=(ActionListWhile&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a ActionListWhile.
		/// </summary>
		virtual ~ActionListWhile() = default;

		/// <summary>
		/// The cloneable function of an ActionListWhile.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new ActionListWhile.</returns>
		[[nodiscard]] gsl::owner<ActionListWhile*> Clone() const override;
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
		/// The condition indicating if this action should execute.
		/// </summary>
		int condition = 0;

	protected:
		/// <summary>
		/// Protected constructor for a ActionListWhile that also takes a RTTI type. If no name is given, it will default to an empty string.
		/// </summary>
		ActionListWhile(RTTI::IdType typeID, const std::string& name = "");

		/// <summary>
		/// Helper function to run the given block of actions.
		/// </summary>
		/// <param name="gameTime">The current game time.</param>
		void RunActionsBlock(const std::string& tableName, const GameTime& gameTime);
	};

	/// <summary>
	/// The generated concrete factory class that produces ActionLists.
	/// </summary>
	CONCRETE_FACTORY(ActionListWhile, FIEAGameEngine::Scope)
}


