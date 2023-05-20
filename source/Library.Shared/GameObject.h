/// <summary>
/// Declarations for a GameObject.
/// </summary>

#pragma once
#include <cstddef>
#include <cassert>
#include "Factory.h"
#include "Attributed.h"
#include "Action.h"
#include "GameTime.h"

namespace FIEAGameEngine {
	class GameObject : public Attributed {
		RTTI_DECLARATIONS(GameObject, Attributed);

	public:
		/// <summary>
		/// Default constructor for a GameObject. If no name is given, it will default to an empty string.
		/// </summary>
		explicit GameObject(const std::string& name = "");
		/// <summary>
		/// A copy constructor for a GameObject that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The GameObject to copy</param>
		GameObject(const GameObject& rhs) = default;
		/// <summary>
		/// The move constructor for GameObject.
		/// </summary>
		/// <param name="rhs">The GameObject to move data from</param>
		GameObject(GameObject&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an GameObject that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The GameObject to copy</param>
		/// <returns>A reference to the updated GameObject</returns>
		GameObject& operator=(const GameObject& rhs) = default;
		/// <summary>
		/// The move assignment operator for a GameObject that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The GameObject to copy</param>
		/// <returns>A reference to the updated GameObject</returns>
		GameObject& operator=(GameObject&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a GameObject.
		/// </summary>
		virtual ~GameObject() = default;

		/// <summary>
		/// The cloneable function of an GameObject, made pure virtual because you cannot have a concreate GameObject.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new GameObject.</returns>
		[[nodiscard]] gsl::owner<GameObject*> Clone() const override;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		/// <summary>
		/// Update this GameObject and all of its children.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime);
		/// <summary>
		/// Convenience function to create a default instance of the given type and adopt it as a child.
		/// </summary>
		/// <param name="childClassName">The class name of the child to create.</param>
		/// <param name="childName">The name of the child to create.</param>
		void CreateChild(const std::string& childClassName, const std::string& childName);
		/// <summary>
		/// Convenience function to create a default instance of the given type and adopt it as an action.
		/// </summary>
		/// <param name="childClassName">The class name of the action to create.</param>
		/// <param name="childName">The name of the action to create.</param>
		void CreateAction(const std::string& actionClassName, const std::string& actionName);

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this GameObject with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;
		/// <summary>
		/// Overriden RTTI functionality - Get a string representation of this GameObject.
		/// </summary>
		/// <returns>A string representing this GameObject.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Getter function for the name of this game object.
		/// </summary>
		/// <returns>A const reference to the name of this game object.</returns>
		inline const std::string& Name() const { return name; };
		/// <summary>
		/// A setter for the name of this object.
		/// </summary>
		/// <param name="newName">The new name.</param>
		inline void SetName(const std::string& newName) { name = newName; };
		/// <summary>
		/// Getter function for the position of this game object.
		/// </summary>
		/// <returns>A const reference to the position of this game object.</returns>
		inline const glm::vec4& Position() const { return position; };
		/// <summary>
		/// A setter for the position of this object.
		/// </summary>
		/// <param name="newPosition">The new position.</param>
		inline void SetPosition(const glm::vec4& newPosition) { position = newPosition; };
		/// <summary>
		/// Getter function for the rotation of this game object.
		/// </summary>
		/// <returns>A const reference to the rotation of this game object.</returns>
		inline const glm::vec4& Rotation() const { return rotation; };
		/// <summary>
		/// A setter for the rotation of this object.
		/// </summary>
		/// <param name="newRotation">The new rotation.</param>
		inline void SetRotation(const glm::vec4& newRotation) { rotation = newRotation; };
		/// <summary>
		/// Getter function for the scale of this game object.
		/// </summary>
		/// <returns>A const reference to the scale of this game object.</returns>
		inline const glm::vec4& Scale() const { return scale; };
		/// <summary>
		/// A setter for the scale of this object.
		/// </summary>
		/// <param name="newScale">The new scale.</param>
		inline void SetScale(const glm::vec4& newScale) { scale = newScale; };
		/// <summary>
		/// Convenience function to get the actions datum of this game object.
		/// </summary>
		/// <returns>A reference to the actions datum of this object.</returns>
		inline const Datum& Actions() const { return At("actions"); };;

	protected:
		/// <summary>
		/// Protected constructor for a ActionList that also takes a RTTI type. If no name is given, it will default to an empty string.
		/// </summary>
		GameObject(RTTI::IdType typeID, const std::string& name = "");

		/// <summary>
		/// The name of this GameObject.
		/// </summary>
		std::string name;
		/// <summary>
		/// The position of this GameObject.
		/// </summary>
		glm::vec4 position;
		/// <summary>
		/// The rotation of this GameObject.
		/// </summary>
		glm::vec4 rotation;
		/// <summary>
		/// The scale of this GameObject.
		/// </summary>
		glm::vec4 scale;

	private:
		/// <summary>
		/// Helper function to update children.
		/// </summary>
		/// <param name="gameTime">The current game time.</param>
		void UpdateChildren(const GameTime& gameTime);
		/// <summary>
		/// Helper function to update actions.
		/// </summary>
		/// <param name="gameTime">The current game time.</param>
		void UpdateActions(const GameTime& gameTime);

		/// <summary>
		/// String key for the children attribute.
		/// </summary>
		inline static const std::string childrenKey = { "children" };
		/// <summary>
		/// The cached datum for the children attribute.
		/// </summary>
		Datum* childrenDatum{ nullptr };
		/// <summary>
		/// String key for the actions attribute.
		/// </summary>
		inline static const std::string actionsKey = { "actions" };
		/// <summary>
		/// The cached datum for the actions attribute.
		/// </summary>
		Datum* actionsDatum{ nullptr };
	};

	/// <summary>
	/// The generated concrete factory class that produces GameObjects.
	/// </summary>
	CONCRETE_FACTORY(GameObject, FIEAGameEngine::Scope)
}

