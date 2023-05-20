#pragma once

#include <algorithm>
#include <functional>
#include "EventPublisher.h"
#include "GameTime.h"

namespace FIEAGameEngine {
	class EventQueue final {
		/// <summary>
		/// struct for organizing information associated with an event (the event itself and the time at which the event should be delivered).
		/// </summary>
		struct QueueEntry {
			/// <summary>
			/// The event itself.
			/// </summary>
			std::shared_ptr<EventPublisher> event;
			/// <summary>
			/// The time point at which the event should be delivered.
			/// </summary>
			std::chrono::high_resolution_clock::time_point expirationTime;

			/// <summary>
			/// Check if this entry has expired and should be delivered.
			/// </summary>
			/// <param name="currentTime">The current game time.</param>
			/// <returns>A boolean indicating expiry.</returns>
			inline bool IsExpired(const GameTime& currentTime) {
				return currentTime.CurrentTime() >= expirationTime;
			};
		};

	public:
		/// <summary>
		/// Default constructor for a EventQueue.
		/// </summary>
		EventQueue() = default;
		/// <summary>
		/// A copy constructor for a EventQueue that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The EventQueue to copy</param>
		EventQueue(const EventQueue& rhs) = default;
		/// <summary>
		/// The move constructor for EventQueue.
		/// </summary>
		/// <param name="rhs">The EventQueue to move data from</param>
		EventQueue(EventQueue&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an EventQueue that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The EventQueue to copy</param>
		/// <returns>A reference to the updated EventQueue</returns>
		EventQueue& operator=(const EventQueue& rhs) = default;
		/// <summary>
		/// The move assignment operator for a EventQueue that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The EventQueue to copy</param>
		/// <returns>A reference to the updated EventQueue</returns>
		EventQueue& operator=(EventQueue&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a EventQueue.
		/// </summary>
		~EventQueue() = default;

		/// <summary>
		/// Run through all queued events and deliver those that have expired.
		/// </summary>
		void Update(const GameTime& gameTime);
		/// <summary>
		/// Enqueue a new event.
		/// </summary>
		/// <param name="event">The event to enqueue</param>
		/// <param name="delay">The delay in milliseconds from now for when this event should be delivered</param>
		void Enqueue(std::shared_ptr<EventPublisher> event, const GameTime& gameTime, std::chrono::milliseconds delay = std::chrono::milliseconds(0));
		/// <summary>
		/// Dequeue a currently queued event - no effect if the specified event is not in the queue.
		/// </summary>
		/// <param name="event">The event to dequeue.</param>
		void Dequeue(std::shared_ptr<EventPublisher> event);
		/// <summary>
		/// Clear all queues, including pending ones.
		/// </summary>
		void Clear();
		/// <summary>
		/// Check if the queue is empty.
		/// </summary>
		/// <returns>A boolean indicating if the queue is empty.</returns>
		inline bool IsEmpty() const { return _queue.IsEmpty(); };
		/// <summary>
		/// Get the current size of the queue.
		/// </summary>
		/// <returns>The size of the queue.</returns>
		inline std::size_t Size() const { return _queue.Size(); };

	private:
		/// <summary>
		/// Helper function to process pending adds or removals from the queue.
		/// </summary>
		void HandlePendingEntries();

		/// <summary>
		/// The event queue.
		/// </summary>
		Vector<QueueEntry> _queue;
		/// <summary>
		/// A list of events pending addition to the event queue.
		/// </summary>
		Vector<QueueEntry> _pendingAddQueue;
		/// <summary>
		/// A list of events pending removal from the event queue.
		/// </summary>
		Vector<Vector<QueueEntry>::Iterator> _pendingRemoveQueue;
		/// <summary>
		/// A flag for if the event queue is actively processing events.
		/// </summary>
		volatile bool _isUpdating = false;
		/// <summary>
		/// boolean flag for if a clear has been requested while in the middle of an update.
		/// </summary>
		bool _flagForClear = false;
	};
}


