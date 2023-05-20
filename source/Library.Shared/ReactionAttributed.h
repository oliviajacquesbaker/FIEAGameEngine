/// <summary>
/// Declarations and inline definitions for ReactionAttributed.
/// </summary>

#pragma once
#include "Reaction.h"
#include "EventMessageAttributed.h"

namespace FIEAGameEngine {
	class ReactionAttributed : public Reaction {
		RTTI_DECLARATIONS(ReactionAttributed, Reaction);

	public:
		/// <summary>
		/// The constructor for a ReactionAttributed
		/// </summary>
		/// <param name="name">The name of this ReactioAttributed</param>
		ReactionAttributed(const std::string& name = "", std::string subtype = "");
		/// <summary>
		/// A copy constructor for a ReactionAttributed that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The ReactionAttributed to copy</param>
		ReactionAttributed(const ReactionAttributed& rhs) = default;
		/// <summary>
		/// The move constructor for ReactionAttributed.
		/// </summary>
		/// <param name="rhs">The ReactionAttributed to move data from</param>
		ReactionAttributed(ReactionAttributed&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an ReactionAttributed that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The ReactionAttributed to copy</param>
		/// <returns>A reference to the updated ReactionAttributed</returns>
		ReactionAttributed& operator=(const ReactionAttributed& rhs) = default;
		/// <summary>
		/// The move assignment operator for a ReactionAttributed that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The ReactionAttributed to copy</param>
		/// <returns>A reference to the updated ReactionAttributed</returns>
		ReactionAttributed& operator=(ReactionAttributed&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a ReactionAttributed.
		/// </summary>
		virtual ~ReactionAttributed();

		/// <summary>
		/// The cloneable function of an ReactionAttributed, made pure virtual because you cannot have a concreate ReactionAttributed.
		/// </summary>
		/// <returns>A gsl::owner indicating ownership of the enclosed pointer to the new ReactionAttributed.</returns>
		[[nodiscard]] gsl::owner<ReactionAttributed*> Clone() const override;
		/// <summary>
		/// Get the signatures of all the prescribed attributes for this Attributed object to auto populate its scope.
		/// </summary>
		/// <returns>A Vector of all the signatures of the prescribed attributes for this object.</returns>
		static FIEAGameEngine::Vector<FIEAGameEngine::Signature> Signatures();
		/// <summary>
		/// Update this ReactionAttributed.
		/// </summary>
		/// <param name="gameTime">The current time of the game</param>
		virtual void Update(const GameTime& gameTime) override;
		/// <summary>
		/// The "notification response" function of this ReactionAttributed;
		/// </summary>
		/// <param name="eventPublisher">The event that occured.</param>
		virtual void Notify(EventPublisher& eventPublisher);

		/// <summary>
		/// Overriden RTTI functionality - Compare the equality of this ReactionAttributed with another object.
		/// </summary>
		/// <returns>A bool representing the equality of these two RTTI.</returns>
		bool Equals(const RTTI* rhs) const override;

		/// <summary>
		/// Getter function for the subtype of this ReactionAttributed.
		/// </summary>
		/// <returns>A const reference to the subtype of this ReactionAttributed.</returns>
		inline const std::string& Subtype() const { return _subtype; };
		/// <summary>
		/// A setter for the gameState of this ReactionAttributed.
		/// </summary>
		/// <param name="newGameState">The new subtype.</param>
		inline void SetSubtype(std::string newSubtype) { _subtype = std::move(newSubtype); };

	protected:
		/// <summary>
		/// Constructor for a ReactionAttributed that takes a derived class RTTI type ID.
		/// </summary>
		/// <param name="typeID">The actual typeID of this instance.</param>
		/// <param name="name">The name of this ReactionAttributed.</param>
		explicit ReactionAttributed(RTTI::IdType typeID, const std::string& name = "", std::string subtype = "");
		/// <summary>
		/// A helper function to copy in the "parameters" of an EventMessage Attributed object.
		/// </summary>
		/// <param name="eventMessage"></param>
		void CopyInParameters(const EventMessageAttributed& eventMessage);

		/// <summary>
		/// The subtype this ReactionAttributed matches to.
		/// </summary>
		std::string _subtype{ "" };
		/// <summary>
		/// The delegate object that will subscribe to events.
		/// </summary>
		Delegate notificationDelegate{ std::bind(&ReactionAttributed::Notify, this, _1) };
	};

	/// <summary>
	/// The generated concrete factory class that produces ReactionAttributeds.
	/// </summary>
	CONCRETE_FACTORY(ReactionAttributed, FIEAGameEngine::Scope)
}


