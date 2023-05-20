#pragma once
#include "RTTI.h"
#include "Vector.h"
#include <functional>

namespace FIEAGameEngine {
	class EventPublisher : public RTTI {
		RTTI_DECLARATIONS(EventPublisher, RTTI);

	public:
		/// <summary>
		/// A copy constructor for a EventPublisher that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The EventPublisher to copy</param>
		EventPublisher(const EventPublisher& rhs) = default;
		/// <summary>
		/// The move constructor for EventPublisher.
		/// </summary>
		/// <param name="rhs">The EventPublisher to move data from</param>
		EventPublisher(EventPublisher&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an EventPublisher that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The EventPublisher to copy</param>
		/// <returns>A reference to the updated EventPublisher</returns>
		EventPublisher& operator=(const EventPublisher& rhs) = default;
		/// <summary>
		/// The move assignment operator for a EventPublisher that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The EventPublisher to copy</param>
		/// <returns>A reference to the updated EventPublisher</returns>
		EventPublisher& operator=(EventPublisher&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a EventPublisher.
		/// </summary>
		virtual ~EventPublisher() = default;

		/// <summary>
		/// Notify all subscribers of this event.
		/// </summary>
		virtual void Deliver() = 0;

	protected:
		/// <summary>
		/// Default constructor for a EventPublisher.
		/// </summary>
		EventPublisher() = default;
	};
}


