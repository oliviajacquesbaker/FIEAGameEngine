/// <summary>
/// Declarations and inline definitions for the Game State.
/// </summary>

#pragma once
#include "GameClock.h"
#include "GameTime.h"
#include "EventQueue.h"
#include "GameObject.h"

namespace FIEAGameEngine {
	class GameState final {

	public:
		/// <summary>
		/// Update all elements of the current game state.
		/// </summary>
		static void Update();

		/// <summary>
		/// Set the root object of the game.
		/// </summary>
		/// <param name="rootObject">A reference to the root GameObject.</param>
		inline static void SetRootObject(GameObject& rootObject) { _rootObject = &rootObject; };
		/// <summary>
		/// Get the root object of the game.
		/// </summary>
		/// <returns>The root GameObject.</returns>
		inline static GameObject* GetRootObject() { return _rootObject; };

		/// <summary>
		/// Get the game time.
		/// </summary>
		/// <returns>A reference to the GameTime object.</returns>
		inline static GameTime& GetGameTime() { return _gameTime; };
		/// <summary>
		/// Set whether or not the game clock should update automatically each update tick.
		/// </summary>
		/// <param name="enabled">A boolean indicating whether the game clock should update.</param>
		inline static void SetGameClockEnabled(bool enabled) { _gameClockEnabled = enabled; };
		/// <summary>
		/// Get whether or not the game clock is currently enabled.
		/// </summary>
		/// <returns>A boolean indicating whether the game clock is currently enabled.</returns>
		inline static bool IsGameClockEnabled() { return _gameClockEnabled; };

		/// <summary>
		/// Get the game's EventQueue.
		/// </summary>
		/// <returns>A reference to the EventQueue object.</returns>
		inline static EventQueue& GetEventQueue() { return _eventQueue; };
		/// <summary>
		/// Request all members cleaned up, i.e. event queue is cleared and root is set to nullptr..
		/// </summary>
		static void RequestCleanup();

	private:
		/// <summary>
		/// The game clock.
		/// </summary>
		inline static GameClock _gameClock{};
		/// <summary>
		/// The game time object.
		/// </summary>
		inline static GameTime _gameTime{};
		/// <summary>
		/// The event queue object.
		/// </summary>
		inline static EventQueue _eventQueue{};
		/// <summary>
		/// A pointer to the root object of the game.
		/// </summary>
		inline static GameObject* _rootObject{ nullptr };
		/// <summary>
		/// Whether or not the game clock should tick.
		/// </summary>
		inline static bool _gameClockEnabled{ false };
	};
}


