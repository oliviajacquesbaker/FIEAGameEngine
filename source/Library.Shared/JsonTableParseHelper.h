/// <summary>
/// The declaration of the JsonTableParseHelper, as well as its wrapped class Wrapper and struct ContextFrame.
/// </summary>

#pragma once
#include "IJsonParseHelper.h"
#include "JsonParseCoordinator.h"
#include "Factory.h"
#include "Scope.h"

namespace FIEAGameEngine
{
	class JsonTableParseHelper : public IJsonParseHelper {
		RTTI_DECLARATIONS(JsonTableParseHelper, IJsonParseHelper);

	public:
		class Wrapper final : public FIEAGameEngine::Wrapper {
			RTTI_DECLARATIONS(Wrapper, FIEAGameEngine::Wrapper);

		public:
			/// <summary>
			/// The primary constructor for the Wrapper, which takes in a reference to the root scope
			/// </summary>
			/// <param name="root">The root Scope object</param>
			explicit Wrapper(Scope& root);			
			/// <summary>
			/// The default constructor for the Wrapper has been deleted.
			/// </summary>
			Wrapper() = delete;
			/// <summary>
			/// A copy constructor for a Wrapper that performs a deep copy during construction.
			/// </summary>
			/// <param name="other">The Wrapper to copy</param>
			Wrapper(const Wrapper& rhs) = default;
			/// <summary>
			/// The move constructor for Wrapper.
			/// </summary>
			/// <param name="rhs">The Wrapper to move data from</param>
			Wrapper(Wrapper&& rhs) noexcept = default;
			/// <summary>
			/// The copy assignment operator for an Wrapper has been deleted - references can not be reassigned.
			/// </summary>
			/// <param name="rhs">The Wrapper to copy</param>
			/// <returns>A reference to the updated Wrapper</returns>
			Wrapper& operator=(const Wrapper& rhs) = delete;
			/// <summary>
			/// The move assignment operator for a Wrapper has been deleted - references can not be reassigned.
			/// </summary>
			/// <param name="rhs">The Wrapper to copy</param>
			/// <returns>A reference to the updated Wrapper</returns>
			Wrapper& operator=(Wrapper&& rhs) noexcept = delete;
			/// <summary>
			/// The destructor for a JsonParseCoordinator.
			/// </summary>
			virtual ~Wrapper() = default;

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
			/// The actual data of this wrapper - a root scope object. 
			/// </summary>
			Scope& _root;
		};

		struct ContextFrame final {
			/// <summary>
			/// The default constructor for the ContextFrame has been deleted.
			/// </summary>
			ContextFrame() = delete;
			/// <summary>
			/// The primary constructor for a ContextFrame.
			/// </summary>
			/// <param name="key">The name of the ContextFrame.</param>
			/// <param name="root">The root scope of the ContextFrame.</param>
			ContextFrame(const std::string& key, Scope& root);
			/// <summary>
			/// A copy constructor for a ContextFrame that performs a deep copy during construction.
			/// </summary>
			/// <param name="other">The ContextFrame to copy</param>
			ContextFrame(const ContextFrame& rhs) = default;
			/// <summary>
			/// The move constructor for ContextFrame.
			/// </summary>
			/// <param name="rhs">The ContextFrame to move data from</param>
			ContextFrame(ContextFrame&& rhs) noexcept = default;
			/// <summary>
			/// The copy assignment operator for an ContextFrame has been deleted - references can not be reassigned.
			/// </summary>
			/// <param name="rhs">The ContextFrame to copy</param>
			/// <returns>A reference to the updated ContextFrame</returns>
			ContextFrame& operator=(const ContextFrame& rhs) = delete;
			/// <summary>
			/// The move assignment operator for a ContextFrame has been deleted - references can not be reassigned.
			/// </summary>
			/// <param name="rhs">The ContextFrame to copy</param>
			/// <returns>A reference to the updated ContextFrame</returns>
			ContextFrame& operator=(ContextFrame&& rhs) noexcept = delete;
			/// <summary>
			/// The destructor for a ContextFrame.
			/// </summary>
			~ContextFrame() = default;

			/// <summary>
			/// The name of this context frame
			/// </summary>
			const std::string& name;
			/// <summary>
			/// The class of this context frame
			/// </summary>
			std::string className = "";
			/// <summary>
			/// The type of this context frame
			/// </summary>
			Datum::DatumTypes type;
			/// <summary>
			/// The data of this context frame, stored as references so it can be handled only later in the process
			/// </summary>
			Vector<std::reference_wrapper<const Json::Value>> jsonData;
			/// <summary>
			/// The root scope of this context frame
			/// </summary>
			Scope& root;
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
		/// String key representing the keyword for datum type
		/// </summary>
		const std::string typeKey{ "type" };
		/// <summary>
		/// String key representing the keyword for object class (if present)
		/// </summary>
		const std::string classKey{ "class" };
		/// <summary>
		/// String key representing the keyword for datum value
		/// </summary>
		const std::string valueKey{ "value" };
		/// <summary>
		/// The stack tracking which part of the deserialization process the helper is in.
		/// </summary>
		SList<ContextFrame> _stack;

		/// <summary>
		/// A helper function that sets a datum's data after decoding it from the json value according to the datum's type.
		/// </summary>
		/// <param name="datum">The datum to update</param>
		/// <param name="val">The json data to decode</param>
		/// <param name="index">The index of the datum to update</param>
		void SetIntData(Datum& datum, const Json::Value& val, std::size_t index) const;
		/// <summary>
		/// A helper function that sets a datum's data after decoding it from the json value according to the datum's type.
		/// </summary>
		/// <param name="datum">The datum to update</param>
		/// <param name="val">The json data to decode</param>
		/// <param name="index">The index of the datum to update</param>
		void SetFloatData(Datum& datum, const Json::Value& val, std::size_t index) const;
		/// <summary>
		/// A helper function that sets a datum's data after decoding it from the json value according to the datum's type.
		/// </summary>
		/// <param name="datum">The datum to update</param>
		/// <param name="val">The json data to decode</param>
		/// <param name="index">The index of the datum to update</param>
		void SetStringData(Datum& datum, const Json::Value& val, std::size_t index) const;
		/// <summary>
		/// A helper function that pushes back a datum's data after decoding it from the json value according to the datum's type.
		/// </summary>
		/// <param name="datum">The datum to update</param>
		/// <param name="val">The json data to decode</param>
		/// <param name="index">The index of the datum to update</param>
		void PushIntData(Datum& datum, const Json::Value& val, std::size_t index) const;
		/// <summary>
		/// A helper function that pushes back a datum's data after decoding it from the json value according to the datum's type.
		/// </summary>
		/// <param name="datum">The datum to update</param>
		/// <param name="val">The json data to decode</param>
		/// <param name="index">The index of the datum to update</param>
		void PushFloatData(Datum& datum, const Json::Value& val, std::size_t index) const;
		/// <summary>
		/// A helper function that pushes back a datum's data after decoding it from the json value according to the datum's type.
		/// </summary>
		/// <param name="datum">The datum to update</param>
		/// <param name="val">The json data to decode</param>
		/// <param name="index">The index of the datum to update</param>
		void PushStringData(Datum& datum, const Json::Value& val, std::size_t index) const;

		/// <summary>
		/// Create and adopt a scope made from a factory.
		/// </summary>
		/// <param name="className">The name of the class of the scope.</param>
		void AppendFromFactory(const std::string& className);

		using SetDataFunction = void(JsonTableParseHelper::*)(Datum&, const Json::Value&, std::size_t) const;
		/// <summary>
		/// A look up table to retrieve the correct data handling method according to what type this datum is.
		/// </summary>
		inline static const SetDataFunction _SetFunction[static_cast<std::int32_t>(Datum::DatumTypes::End) + 1] = {
			nullptr,								//Datum::Types::Unknown
			&JsonTableParseHelper::SetIntData,		//Datum::Types::Integer
			&JsonTableParseHelper::SetFloatData,	//Datum::Types::Float			
			&JsonTableParseHelper::SetStringData,	//Datum::Types::String
			&JsonTableParseHelper::SetStringData,	//Datum::Types::Vector
			&JsonTableParseHelper::SetStringData,	//Datum::Types::Matrix
			nullptr,								//Datum::Types::Pointer
			nullptr									//Datum::Types::Table
		};
		/// <summary>
		/// A look up table to retrieve the correct data handling method according to what type this datum is.
		/// </summary>
		inline static const SetDataFunction _PushFunction[static_cast<std::int32_t>(Datum::DatumTypes::End) + 1] = {
			nullptr,								//Datum::Types::Unknown
			&JsonTableParseHelper::PushIntData,		//Datum::Types::Integer
			&JsonTableParseHelper::PushFloatData,	//Datum::Types::Float			
			&JsonTableParseHelper::PushStringData,	//Datum::Types::String
			&JsonTableParseHelper::PushStringData,	//Datum::Types::Vector
			&JsonTableParseHelper::PushStringData,	//Datum::Types::Matrix
			nullptr,								//Datum::Types::Pointer
			nullptr									//Datum::Types::Table
		};
	};
}

#include "JsonTableParseHelper.inl"
