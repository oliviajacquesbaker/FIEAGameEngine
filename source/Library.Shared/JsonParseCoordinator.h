/// <summary>
/// The declaration of the abstract base class Wrapper and the class JsonParseCoordinator.
/// </summary>

#pragma once
#include <sstream>
#include <fstream>
#include <stack>
#include "Vector.h"
#include "RTTI.h"
#include "IJsonParseHelper.h"

namespace FIEAGameEngine
{
	class Wrapper : public RTTI {
		RTTI_DECLARATIONS(Wrapper, RTTI);

	public:
		/// <summary>
		/// The default constructor for the Wrapper.
		/// </summary>
		Wrapper() = default;
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
		/// The copy assignment operator for an Wrapper that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Wrapper to copy</param>
		/// <returns>A reference to the updated Wrapper</returns>
		Wrapper& operator=(const Wrapper& rhs) = default;
		/// <summary>
		/// The move assignment operator for a Wrapper that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The Wrapper to copy</param>
		/// <returns>A reference to the updated Wrapper</returns>
		Wrapper& operator=(Wrapper&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a JsonParseCoordinator.
		/// </summary>
		virtual ~Wrapper() = default;

		/// <summary>
		/// Derived classes should implement this to return a new object of the same type, but default constructed.
		/// </summary>
		/// <returns>The newly created object.</returns>
		virtual std::shared_ptr<Wrapper> Create() const = 0;
		/// <summary>
		/// Complete any work needed at initialization for this wrapper.
		/// </summary>
		virtual void Initialize();
		/// <summary>
		/// Complete any work needed at cleanup by this wrapper.
		/// </summary>
		virtual void Cleanup();

		/// <summary>
		/// Increment the current nesting depth.
		/// </summary>
		void IncrementDepth();
		/// <summary>
		/// Decrement the current nesting depth.
		/// </summary>
		void DecrementDepth();
		/// <summary>
		/// Get the current nesting depth.
		/// </summary>
		/// <returns>The current nesting depth</returns>
		std::size_t Depth() const;

	private:
		/// <summary>
		/// The current nesting depth.
		/// </summary>
		std::size_t _nestingDepth = 0;
	};

	class JsonParseCoordinator final : public RTTI {
		RTTI_DECLARATIONS(JsonParseCoordinator, RTTI);

	public:
		/// <summary>
		/// The default constructor for the JsonParseCoordinator has been deleted.
		/// </summary>
		JsonParseCoordinator() = delete;
		/// <summary>
		/// Default constructor for a JsonParseCoordinator. If no size is given, it will default to zero.
		/// </summary>
		/// <param name="size">The initial capacity of the JsonParseCoordinator.</param>
		explicit JsonParseCoordinator(std::shared_ptr<Wrapper> wrapper);
		/// <summary>
		/// A copy constructor for a JsonParseCoordinator that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The JsonParseCoordinator to copy</param>
		JsonParseCoordinator(const JsonParseCoordinator& rhs) = delete;
		/// <summary>
		/// The move constructor for JsonParseCoordinator.
		/// </summary>
		/// <param name="rhs">The JsonParseCoordinator to move data from</param>
		JsonParseCoordinator(JsonParseCoordinator&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for an JsonParseCoordinator that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The JsonParseCoordinator to copy</param>
		/// <returns>A reference to the updated JsonParseCoordinator</returns>
		JsonParseCoordinator& operator=(const JsonParseCoordinator& rhs) = delete;
		/// <summary>
		/// The move assignment operator for a JsonParseCoordinator that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The JsonParseCoordinator to copy</param>
		/// <returns>A reference to the updated JsonParseCoordinator</returns>
		JsonParseCoordinator& operator=(JsonParseCoordinator&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a JsonParseCoordinator.
		/// </summary>
		~JsonParseCoordinator() = default;
		/// <summary>
		/// Create a new object of the same type and containing the same helpers and wrapper types, but all default constructed.
		/// </summary>
		/// <returns>The newly created object.</returns>
		[[nodiscard]] std::shared_ptr<JsonParseCoordinator> Create() const;

		/// <summary>
		/// Get the list of helpers from this ParseCoordinator.
		/// </summary>
		/// <returns>A vector of shared_ptrs to helpers.</returns>
		Vector<std::shared_ptr<IJsonParseHelper>>& GetHelpers();
		/// <summary>
		/// Get the list of helpers from this ParseCoordinator.
		/// </summary>
		/// <returns>A const vector of shared_ptrs to helpers.</returns>
		const Vector<std::shared_ptr<IJsonParseHelper>>& GetHelpers() const;
		/// <summary>
		/// Get the wrapper associated with this Parse Coordinator.
		/// </summary>
		/// <returns>The wrapper of this Parse Coordinator.</returns>
		std::shared_ptr<Wrapper> GetWrapper();
		/// <summary>
		/// Get the wrapper associated with this Parse Coordinator.
		/// </summary>
		/// <returns>The const wrapper of this Parse Coordinator.</returns>
		const std::shared_ptr<Wrapper> GetWrapper() const;

		/// <summary>
		/// Add a helper.
		/// </summary>
		/// <param name="helper">The helper to add</param>
		/// <returns>A boolean indicating if an add occurred.</returns>
		bool AddHelper(std::shared_ptr<IJsonParseHelper> helper);
		/// <summary>
		/// Remove a helper, given a reference to it.
		/// </summary>
		/// <param name="helper">The helper to remove</param>
		/// <returns>A boolean indicating if a remove occured.</returns>
		bool RemoveHelper(std::shared_ptr<IJsonParseHelper> helper);
		/// <summary>
		/// Remove a helper given the type id of it.
		/// </summary>
		/// <param name="helperClass">The rtti type id class of the helper to remove</param>
		/// <returns>A boolean indicating if there was a successful remove or not.</returns>
		bool RemoveHelper(RTTI::IdType helperClass);
		/// <summary>
		/// Set the wrapper for this Parse Coordinator.
		/// </summary>
		/// <param name="wrapper">The new wrapper for this Parse Coordinator to reference.</param>
		void SetWrapper(std::shared_ptr<Wrapper> wrapper);

		/// <summary>
		/// Deserialize a json object given the raw json string
		/// </summary>
		/// <param name="jsonString">The json string</param>
		void DeserializeObject(const std::string& jsonString);
		/// <summary>
		/// Deserialize a json object given its input stream
		/// </summary>
		/// <param name="inStream">An input stream to deserialize as json</param>
		void DeserializeObject(std::istream& inStream);
		/// <summary>
		/// Deserialize a json object given a file name
		/// </summary>
		/// <param name="fileName">The name of the file containing a json object</param>
		void DeserializeObjectFromFile(const std::string& fileName);

		/// <summary>
		/// Compare this object with another RTTI object
		/// </summary>
		/// <param name="rhs">The object to compare against</param>
		/// <returns>A boolean indicating if the two objects are equivilant</returns>
		bool Equals(const RTTI* rhs) const;
		/// <summary>
		/// Get a string representation of this RTTI object.
		/// </summary>
		/// <returns>A string representation of this object.</returns>
		std::string ToString() const;

	private:
		/// <summary>
		/// The wrapper of this Parse Coordinator
		/// </summary>
		std::shared_ptr<Wrapper> _wrapper;
		/// <summary>
		/// The list of helpers for this Parse Coordinator.
		/// </summary>
		Vector<std::shared_ptr<IJsonParseHelper>> _helpers;
		
		/// <summary>
		/// Initialize this parser and all its data members
		/// </summary>
		void Initialize();
		/// <summary>
		/// Perform any clean up on this parser or its data members as necessary.
		/// </summary>
		void Cleanup();

		/// <summary>
		/// A helper function to parse all values within a json object
		/// </summary>
		/// <param name="jsonObject">The json object to parse</param>
		void ParseMembers(const Json::Value& jsonObject);
		/// <summary>
		/// A helper function to parse a key value pair
		/// </summary>
		/// <param name="jsonKey">The key of the object</param>
		/// <param name="jsonObject">The object</param>
		/// <param name="isArray">Whether or not the object is of type array</param>
		void Parse(const std::string& jsonKey, const Json::Value& jsonObject, bool isArray);

		/// <summary>
		/// A helper function to pass the parse request along.
		/// </summary>
		/// <param name="jsonKey">The object key</param>
		/// <param name="jsonObject">The object</param>
		/// <returns>The helper that responded to the request</returns>
		std::shared_ptr<IJsonParseHelper> StartDelegateWork(const std::string& jsonKey, const Json::Value& jsonObject);
		/// <summary>
		/// A helper function to pass along the request of handling the object
		/// </summary>
		/// <param name="helper">The helper to handle the request</param>
		/// <param name="jsonKey">The key of the object</param>
		void EndDelegateWork(std::shared_ptr<IJsonParseHelper> helper, const std::string& jsonKey);
	};
}