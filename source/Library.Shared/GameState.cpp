/// <summary>
/// Definitions for the Game State.
/// </summary>

#include "pch.h"
#include "GameState.h"

namespace FIEAGameEngine {
	void GameState::Update() {
		assert(_rootObject);

		if(_gameClockEnabled) _gameClock.UpdateGameTime(_gameTime);

		_eventQueue.Update(_gameTime);
		_rootObject->Update(_gameTime);
	}
	
	void GameState::RequestCleanup() {
		_gameClock = GameClock();
		_gameTime = GameTime();
		_eventQueue.Clear();
		_rootObject = nullptr;
		_gameClockEnabled = false;
	}
}