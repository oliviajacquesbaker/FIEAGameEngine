/// <summary>
/// The declaration of the abstract interface IJsonParseHelper.
/// </summary>

#pragma once
#include "json/json.h"
#include "RTTI.h"

namespace FIEAGameEngine
{
	class IJsonParseHelper : public RTTI {
		RTTI_DECLARATIONS(IJsonParseHelper, RTTI);

	public:
		/// <summary>
		/// The default constructor, using compiler provided default behavior.
		/// </summary>
		IJsonParseHelper() = default;
		/// <summary>
		/// The copy constructor, using compiler provided default behavior.
		/// </summary>
		/// <param name="rhs">The object to copy</param>
		IJsonParseHelper(const IJsonParseHelper& rhs) = default;
		/// <summary>
		/// The move constructor, using compiler provided default behavior.
		/// </summary>
		/// <param name="rhs">The object to move</param>
		IJsonParseHelper(IJsonParseHelper&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator, using compiler provided default behavior.
		/// </summary>
		/// <param name="rhs">The object to copy</param>
		/// <returns>A reference to the modified this object</returns>
		IJsonParseHelper& operator=(const IJsonParseHelper& rhs) = default;
		/// <summary>
		/// The move assignment operator, using compiler provided default behavior.
		/// </summary>
		/// <param name="rhs">The object to move</param>
		/// <returns>A reference to the modified this object</returns>
		IJsonParseHelper& operator=(IJsonParseHelper&& rhs) noexcept = default;
		/// <summary>
		/// The destructor, using the compiler provided default behavior.
		/// </summary>
		virtual ~IJsonParseHelper() = default;

		/// <summary>
		/// Derived classes should implement this to return a new JsonParseHelper of the same type but in a default constructed state.
		/// </summary>
		/// <returns>A pointer to the new object.</returns>
		virtual std::shared_ptr<IJsonParseHelper> Create() const = 0;
		/// <summary>
		/// Complete any initialization work required for this helper.
		/// </summary>
		virtual void Initialize();
		/// <summary>
		/// Complete any cleanup work required for this helper.
		/// </summary>
		virtual void Cleanup();

		/// <summary>
		/// Respond to the request to handle this json object given the wrapper.
		/// </summary>
		/// <param name="wrapper">The wrapper of the parse coordinator</param>
		/// <param name="jsonKey">The key of the json object</param>
		/// <param name="jsonObject">The json object</param>
		/// <returns>A boolean indicating if this helper handled the request or not</returns>
		virtual bool StartHandler(class Wrapper& wrapper, const std::string& jsonKey, const Json::Value& jsonObject) = 0;
		/// <summary>
		/// Respond to the request to handle this json object given the wrapper.
		/// </summary>
		/// <param name="wrapper">The wrapper of the parse coordinator</param>
		/// <param name="jsonKey">The key of the json object</param>
		/// <returns>A boolean indicating if this helper handled the request or not</returns>
		virtual bool EndHandler(Wrapper& wrapper, const std::string& jsonKey) = 0;
	};
}
