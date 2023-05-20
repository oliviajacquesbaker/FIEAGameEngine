/// <summary>
/// The declaration of the integer parse helper and its wrapper.
/// </summary>

#pragma once
#include "JsonParseCoordinator.h"

namespace UnitTests 
{
	class JsonIntegerParseHelper : public FIEAGameEngine::IJsonParseHelper {
		RTTI_DECLARATIONS(JsonIntegerParseHelper, FIEAGameEngine::IJsonParseHelper);

	public:
		class Wrapper final : public FIEAGameEngine::Wrapper {
			RTTI_DECLARATIONS(Wrapper, FIEAGameEngine::Wrapper);

		public:
			/// <summary>
			/// Create a new object of this same type, but default constructed.
			/// </summary>
			/// <returns>The newly created object</returns>
			std::shared_ptr<FIEAGameEngine::Wrapper> Create() const override;

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
			/// The actual data of this wrapper. 
			/// </summary>
			FIEAGameEngine::Vector<int> data;
		};

		/// <summary>
		/// Create a new object of this same type, but default constructed.
		/// </summary>
		/// <returns>The newly created object</returns>
		std::shared_ptr<FIEAGameEngine::IJsonParseHelper> Create() const override;

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

	private:
		/// <summary>
		/// A state indicator of if this helper is actively parsing something.
		/// </summary>
		bool isParsing{ false };
	};
}



