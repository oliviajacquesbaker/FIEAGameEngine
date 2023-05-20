#include "pch.h"
#include "EventQueue.h"

namespace FIEAGameEngine {
	void EventQueue::Update(const GameTime& gameTime) {
		if (_isUpdating) return;
		_isUpdating = true;

		HandlePendingEntries();

		auto iter = std::partition(_queue.begin(), _queue.end(), [&gameTime](QueueEntry entry) {return !entry.IsExpired(gameTime); });
		for (auto iterCopy = iter; iterCopy != _queue.end(); ++iterCopy) {
			(*iterCopy).event->Deliver();
		}
		_queue.Remove(iter, _queue.end());
		_isUpdating = false;
	}

	void EventQueue::Enqueue(std::shared_ptr<EventPublisher> event, const GameTime& gameTime, std::chrono::milliseconds delay) {
		if(!_isUpdating) _queue.PushBack({ event, gameTime.CurrentTime() + delay });
		else _pendingAddQueue.PushBack({ event, gameTime.CurrentTime() + delay});
	}

	void EventQueue::Dequeue(std::shared_ptr<EventPublisher> _event) {
		for (auto iter = _queue.begin(); iter != _queue.end(); ++iter) {
			if ((*iter).event == _event) {
				if (!_isUpdating) _queue.Remove(iter);
				else _pendingRemoveQueue.PushBack(iter);
				break;
			}
		}		
	}

	void EventQueue::Clear() {
		if (!_isUpdating) {
			_queue.Clear();
			_queue.ShrinkToFit();
			_pendingAddQueue.Clear();
			_pendingAddQueue.ShrinkToFit();
			_pendingRemoveQueue.Clear();
			_pendingRemoveQueue.ShrinkToFit();
		}
		else _flagForClear = true;
	}
	
	void EventQueue::HandlePendingEntries() {
		if (_flagForClear) _queue.Clear();
		else {
			for (auto& item : _pendingRemoveQueue) {
				_queue.Remove(item);
			}
		}
		_pendingRemoveQueue.Clear();
		_flagForClear = false;

		for (auto& item : _pendingAddQueue) { 
			_queue.PushBack(std::move(item));
		}
		_pendingAddQueue.Clear();
	}
}