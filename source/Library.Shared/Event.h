#pragma once
#include <cstddef>
#include <stdexcept>
#include "EventPublisher.h"
#include "HashMap.h"

namespace FIEAGameEngine {
	/// <summary>
	/// Empty struct that can be derived from to provide some structure to an event argument type.
	/// </summary>
	struct EventArgs {
		/// <summary>
		/// Constructor for a Event, taking copy of the event payload.
		/// </summary>
		EventArgs() = default;
		/// <summary>
		/// A copy constructor for a EventArgs that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The EventArgs to copy</param>
		EventArgs(const EventArgs& rhs) = default;
		/// <summary>
		/// The move constructor for EventArgs.
		/// </summary>
		/// <param name="rhs">The EventArgs to move data from</param>
		EventArgs(EventArgs&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an EventArgs that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The EventArgs to copy</param>
		/// <returns>A reference to the updated EventArgs</returns>
		EventArgs& operator=(const EventArgs& rhs) = default;
		/// <summary>
		/// The move assignment operator for a EventArgs that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The EventArgs to copy</param>
		/// <returns>A reference to the updated EventArgs</returns>
		EventArgs& operator=(EventArgs&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a EventArgs.
		/// </summary>
		virtual ~EventArgs() = default;
	};

	/// <summary>
	/// Wrapper for std::functions so they can be comaprable as objects.
	/// </summary>
	struct Delegate final {
		using SubscriberFunctor = std::function<void(EventPublisher&)>;
		SubscriberFunctor subscriber;
	};

	template <typename T>
	class Event final : public EventPublisher {
		RTTI_DECLARATIONS(Event, EventPublisher);

	public:
		/// <summary>
		/// Constructor for a Event, taking copy of the event payload.
		/// </summary>
		explicit Event(T eventPayload);
		/// <summary>
		/// A copy constructor for a Event that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The Event to copy</param>
		Event(const Event& rhs) = default;
		/// <summary>
		/// The move constructor for Event.
		/// </summary>
		/// <param name="rhs">The Event to move data from</param>
		Event(Event&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an Event that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Event to copy</param>
		/// <returns>A reference to the updated Event</returns>
		Event& operator=(const Event& rhs) = default;
		/// <summary>
		/// The move assignment operator for a Event that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The Event to copy</param>
		/// <returns>A reference to the updated Event</returns>
		Event& operator=(Event&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a Event.
		/// </summary>
		~Event() = default;

		/// <summary>
		/// Overriden RTTI functionality - Get a string representation of this Event.
		/// </summary>
		/// <returns>A string representing this EventPublisher.</returns>
		std::string ToString() const override;

		/// <summary>
		/// Notify all subscribers of this event.
		/// </summary>
		virtual void Deliver() override;
		/// <summary>
		/// Get the payload of this event
		/// </summary>
		/// <returns>A message of type T</returns>
		const T& Message() const;

		/// <summary>
		/// Add a subscribed functor to the list of subscribers for this event type
		/// </summary>
		/// <param name="subscriber">The new functor to add</param>
		static void Subscribe(Delegate& subscriber);
		/// <summary>
		/// Remove a subscribed functor from the list of subscribers for this event type
		/// </summary>
		/// <param name="subscriber">The functor to remove</param>
		static void Unsubscribe(Delegate& subscriber);
		/// <summary>
		/// Remove all subscribers from this event type.
		/// </summary>
		static void RequestClearSubscribers();

	protected:
		/// <summary>
		/// Helper function to process pending adds or deletes.
		/// </summary>
		static void HandlePendingEntries();
		/// <summary>
		/// Helper function to clear all queues (and shink them to fit) when a requested clear is processed.
		/// </summary>
		static void ClearSubscribers();

		/// <summary>
		/// The list of subscribers, static to this class.
		/// </summary>
		inline static Vector<const Delegate*> _subscribers = Vector<const Delegate*>();
		/// <summary>
		/// List of subscribers that are pending add to the subscribers list.
		/// </summary>
		inline static Vector<const Delegate*> _pendingAddList = Vector<const Delegate*>();
		/// <summary>
		/// List of subscribers that are pending remove from the subscribers list.
		/// </summary>
		inline static Vector<const Delegate*> _pendingRemoveList = Vector<const Delegate*>();
		/// <summary>
		/// boolean flag for if the event is in the middle of delivering itself to subscribers.
		/// </summary>
		inline static volatile bool _isDelivering = false;
		/// <summary>
		/// boolean flag for if a clear has been requested while in the middle of a delivery.
		/// </summary>
		inline static bool _flagForClear = false;

		/// <summary>
		/// The actual information associated with this event.
		/// </summary>
		T _payload; 
	};
}

#include "Event.inl"
