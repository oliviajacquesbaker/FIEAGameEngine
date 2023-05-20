/// <summary>
/// The declaration of the test parse helper and its wrapper.
/// </summary>

#pragma once
#include "JsonParseCoordinator.h"

namespace UnitTests
{
	class JsonTestParseHelper final : public FIEAGameEngine::IJsonParseHelper {
		RTTI_DECLARATIONS(JsonTestParseHelper, FIEAGameEngine::IJsonParseHelper);
	public:
		class Wrapper : public FIEAGameEngine::Wrapper {
			RTTI_DECLARATIONS(JsonTestParseHelper::Wrapper, FIEAGameEngine::Wrapper);

		public:
			/// <summary>
			/// Create a new object of this same type, but default constructed.
			/// </summary>
			/// <returns>The newly created object</returns>
			std::shared_ptr<FIEAGameEngine::Wrapper> Create() const override;
			/// <summary>
			/// Initialize this wrapper for use.
			/// </summary>
			void Initialize() override;
			/// <summary>
			/// Clean up this wrapper from use.
			/// </summary>
			void Cleanup() override;

			/// <summary>
			/// The maximum nesting depth this wrapper has ever reached.
			/// </summary>
			std::size_t maxDepth{ 0 };
			/// <summary>
			/// Whether or not Initialize() has been called on this wrapper.
			/// </summary>
			bool initializeCalled{ false };
			/// <summary>
			/// Whether or not Cleanup() has been called on this wrapper.
			/// </summary>
			bool cleanupCalled{ false };

			/// <summary>
			/// Compare this object to another RTTI object
			/// </summary>
			/// <param name="rhs">The object to compare against</param>
			/// <returns>A boolean indicating equality of the two objects</returns>
			bool Equals(const RTTI* rhs) const;
			/// <summary>
			/// Get a string representation of this object.
			/// </summary>
			/// <returns>A string representation of this object.</returns>
			std::string ToString() const;
		};

		/// <summary>
		/// Create a new object of this same type, but default constructed.
		/// </summary>
		/// <returns>The newly created object</returns>
		std::shared_ptr<FIEAGameEngine::IJsonParseHelper> Create() const override;
		/// <summary>
		/// Initialize this helper for use.
		/// </summary>
		void Initialize() override;
		/// <summary>
		/// Cleanup this helper from use.
		/// </summary>
		void Cleanup() override;

		/// <summary>
		/// Begin processing the parsing request if possible.
		/// </summary>
		/// <param name="wrapper">The wrapper</param>
		/// <param name="jsonKey">The key of the object</param>
		/// <param name="jsonObject">The object</param>
		/// <returns>A boolean indicating if this helper is handling the request.</returns>
		bool StartHandler(FIEAGameEngine::Wrapper& wrapper, const std::string& jsonKey, const Json::Value& jsonObject) override;
		/// <summary>
		/// Finish processing the parsing request if possible.
		/// </summary>
		/// <param name="wrapper">The wrapper</param>
		/// <param name="jsonKey">The key of the object</param>
		/// <returns>A boolean indicating if this helper is handling the request.</returns>
		bool EndHandler(FIEAGameEngine::Wrapper& wrapper, const std::string& jsonKey) override;

		/// <summary>
		/// Compare this object to another RTTI object
		/// </summary>
		/// <param name="rhs">The object to compare against</param>
		/// <returns>A boolean indicating equality of the two objects</returns>
		bool Equals(const RTTI* rhs) const;
		/// <summary>
		/// Get a string representation of this object.
		/// </summary>
		/// <returns>A string representation of this object.</returns>
		std::string ToString() const;

		/// <summary>
		/// Whether or not Initialize() has been called on this helper.
		/// </summary>
		bool initializeCalled{ false };
		/// <summary>
		/// Whether or not Cleanup() has been called on this helper.
		/// </summary>
		bool cleanupCalled{ false };
		/// <summary>
		/// How many times the start handler has been called and responded positively to the request.
		/// </summary>
		size_t startHandlerCount{ 0 };
		/// <summary>
		/// How many times the end handler has been called and responded positively to the request.
		/// </summary>
		size_t endHandlerCount{ 0 };
	};
}

