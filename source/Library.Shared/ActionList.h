#pragma once
#include "Action.h"

namespace FIEAGameEngine {
	class ActionList : public Action {
		RTTI_DECLARATIONS(ActionList, Action);

	public:
		/// <summary>
		/// Default constructor for a ActionList. If no name is given, it will default to an empty string.
		/// </summary>
		ActionList(const std::string& name = "");
		/// <summary>
		/// A copy constructor for a ActionList that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The ActionList to copy</param>
		ActionList(const ActionList& rhs) = default;
		/// <summary>
		/// The move constructor for ActionList.
		/// </summary>
		/// <param name="rhs">The ActionList to move data from</param>
		ActionList(ActionList&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an ActionList that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The ActionList to copy</param>
		/// <returns>A reference to the updated ActionList</returns>
		ActionList& operator=(const ActionList& rhs) = default;
		/// <summary>
		/// The move assignment operator for a ActionList that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The ActionList to copy</param>
		/// <returns>A reference to the updated ActionList</returns>
		ActionList& operator=(ActionList&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a ActionList.
		/// </summary>
		virtual ~ActionList() = default;

		/// <summary>
		/// The cloneable function of an ActionList.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new ActionList.</returns>
		[[nodiscard]] gsl::owner<ActionList*> Clone() const override;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		/// <summary>
		/// Update this ActionList and all of its actions.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime);

		/// <summary>
		/// Convenience function to create a default instance of the given type and adopt it as an action.
		/// </summary>
		/// <param name="childClassName">The class name of the action to create.</param>
		/// <param name="childName">The name of the action to create.</param>
		void CreateAction(const std::string& actionClassName, const std::string& actionName);

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this ActionList with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;

	protected:
		/// <summary>
		/// Protected constructor for a ActionList that also takes a RTTI type. If no name is given, it will default to an empty string.
		/// </summary>
		ActionList(RTTI::IdType typeID, const std::string& name = "");
		/// <summary>
		/// The cached datum for the actions attribute.
		/// </summary>
		Datum* actionsDatum{ nullptr };

	private:
		/// <summary>
		/// String key for the actions attribute.
		/// </summary>
		inline static const std::string actionsKey = { "actions" };
	};

	/// <summary>
	/// The generated concrete factory class that produces ActionLists.
	/// </summary>
	CONCRETE_FACTORY(ActionList, FIEAGameEngine::Scope)
}


