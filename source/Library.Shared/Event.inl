#pragma once

#include "pch.h"
#include "Event.h"

using namespace std::string_literals;
using namespace std::placeholders;

namespace FIEAGameEngine {
	template<typename T>
	RTTI_DEFINITIONS(Event<T>);

	template<typename T>
	inline Event<T>::Event(T eventPayload) :
		_payload{ std::move(eventPayload) }
	{
	}
	 
	template<typename T>
	inline std::string Event<T>::ToString() const {
		return "Event with " + std::to_string(Event<T>::_subscribers.Size()) + " subscribers."s;
	}

	template<typename T>
	inline void Event<T>::Deliver() {
		if (_isDelivering) return;
		_isDelivering = true;

		HandlePendingEntries();
		for (auto& subscriber : Event<T>::_subscribers) {
			if(subscriber && subscriber->subscriber) subscriber->subscriber(*this);
		}

		_isDelivering = false;
	}

	template<typename T>
	inline const T& Event<T>::Message() const {
		return _payload;
	}
	
	template<typename T>
	inline void Event<T>::Subscribe(Delegate& subscriber) {
		if (!_isDelivering) Event<T>::_subscribers.PushBack(&subscriber);
		else Event<T>::_pendingAddList.PushBack(&subscriber);
	}
	
	template<typename T>
	inline void Event<T>::Unsubscribe(Delegate& subscriber) {
		if (!_isDelivering) Event<T>::_subscribers.Remove(&subscriber);
		else Event<T>::_pendingRemoveList.PushBack(&subscriber);
	}
	
	template<typename T>
	inline void Event<T>::ClearSubscribers() {
		Event<T>::_subscribers.Clear();
		Event<T>::_subscribers.ShrinkToFit();
		Event<T>::_pendingAddList.Clear();
		Event<T>::_pendingAddList.ShrinkToFit();
		Event<T>::_pendingRemoveList.Clear();
		Event<T>::_pendingRemoveList.ShrinkToFit();
	}
	
	template<typename T>
	inline void Event<T>::RequestClearSubscribers() {
		if (_isDelivering) _flagForClear = true;
		else ClearSubscribers();
	}

	template<typename T>
	inline void Event<T>::HandlePendingEntries() {
		if (_flagForClear) ClearSubscribers();
		else {
			for (auto& item : _pendingRemoveList) {
				Event<T>::_subscribers.Remove(item);
			}
		}
		_pendingRemoveList.Clear();
		_flagForClear = false;

		for (auto& item : _pendingAddList) {
			_subscribers.PushBack(item);
		}
		_pendingAddList.Clear();
	}
}