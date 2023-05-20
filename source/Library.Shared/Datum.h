/// <summary>
/// The declaration of the class Datum, which is an object that stores values of a single type selected from one of several supported types.
/// </summary>

#pragma once

#include <cstddef>
#include <string>
#include <cassert>
#include <stdexcept>
#include <functional>
#include "glm/glm.hpp"
#include "RTTI.h"
#include "HashMap.h"
#include "DefaultGrowth.h"

#pragma warning(push)
#pragma warning(disable:4201)
#include "glm/gtx/string_cast.hpp"
#pragma warning(pop)


namespace FIEAGameEngine {
	class Scope;
	/// <summary>
	/// The Datum class, representing data of some type. The data can be owned internally or externally, but externally owned data cannot have its memory changed (individual
	/// values may be set to different values, though). 
	/// </summary>
	class Datum final {
		friend class Scope;
		friend class Attributed;
	public:
		using IncrementFunctor = std::function<std::size_t(std::size_t)>;

		/// <summary>
		/// The allowed types of data that can be stored in a Datum. End is not a supported type; it acts only as a sentinel within the enum.
		/// </summary>
		enum class DatumTypes {
			Unknown = 0,
			Integer,
			Float,
			String,
			Vector,
			Matrix,
			Pointer, 
			Table,

			End = Table
		};
		/// <summary>
		/// A lookup table to convert between a Datum type and a string representation of that type.
		/// </summary>
		inline static const HashMap<DatumTypes, std::string> typeToString{
			{{DatumTypes::Unknown, "Unknown"},
			{DatumTypes::Integer, "Integer"},
			{DatumTypes::Float, "Float"},
			{DatumTypes::String, "String"},
			{DatumTypes::Vector, "Vector"},
			{DatumTypes::Matrix, "Matrix"},
			{DatumTypes::Pointer, "Pointer"},
			{DatumTypes::Table, "Table"}},
			std::size_t(17)
		}; 
		/// <summary>
		/// A lookup table to convert between a Datum type and a string representation of that type.
		/// </summary>
		inline static const HashMap<std::string, DatumTypes> stringToType{
			{{"Unknown", DatumTypes::Unknown},
			{"Integer", DatumTypes::Integer},
			{"Float", DatumTypes::Float},
			{"String", DatumTypes::String},
			{"Vector", DatumTypes::Vector},
			{"Matrix", DatumTypes::Matrix},
			{"Pointer", DatumTypes::Pointer},
			{"Table", DatumTypes::Table}},
			std::size_t(17)
		};

		/// <summary>
		/// Default constructor for an empty Datum.
		/// </summary>
		explicit Datum(IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// Constructor for a Datum that allows specification of Datum type and initial capacity
		/// </summary>
		explicit Datum(DatumTypes type, std::size_t size = std::size_t(0), IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// An Initializer list constructor for Datum. The items will be pushed back to the constructed Datum in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an Datum as it's being constructed</param>
		Datum(std::initializer_list<std::int32_t> list, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// An Initializer list constructor for Datum. The items will be pushed back to the constructed Datum in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an Datum as it's being constructed</param>
		Datum(std::initializer_list<float> list, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// An Initializer list constructor for Datum. The items will be pushed back to the constructed Datum in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an Datum as it's being constructed</param>
		Datum(std::initializer_list<std::string> list, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// An Initializer list constructor for Datum. The items will be pushed back to the constructed Datum in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an Datum as it's being constructed</param>
		Datum(std::initializer_list<glm::vec4> list, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// An Initializer list constructor for Datum. The items will be pushed back to the constructed Datum in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an Datum as it's being constructed</param>
		Datum(std::initializer_list<glm::mat4> list, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// An Initializer list constructor for Datum. The items will be pushed back to the constructed Datum in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an Datum as it's being constructed</param>
		Datum(std::initializer_list<RTTI*> list, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// A copy constructor for an Datum that performs a deep copy during construction if the rhs is internally stored, or shallow copy otherwise.
		/// </summary>
		/// <param name="other">The Datum to copy</param>
		Datum(const Datum& rhs);
		/// <summary>
		/// A copy constructor for an Datum that assigns the RHS scalar as a size of 1 datum.
		/// </summary>
		/// <param name="other">The scalar to copy</param>
		Datum(std::int32_t rhs, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// A copy constructor for an Datum that assigns the RHS scalar as a size of 1 datum.
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		Datum(float rhs, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// A copy constructor for an Datum that assigns the RHS scalar as a size of 1 datum.
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		Datum(const std::string& rhs, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// A copy constructor for an Datum that assigns the RHS scalar as a size of 1 datum.
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		Datum(const glm::vec4& rhs, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// A copy constructor for an Datum that assigns the RHS scalar as a size of 1 datum.
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		Datum(const glm::mat4& rhs, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// A copy constructor for an Datum that assigns the RHS scalar as a size of 1 datum.
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		Datum(RTTI* rhs, IncrementFunctor incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// The move constructor for Datum, "stealing" the data from the RHS Datum.
		/// </summary>
		/// <param name="rhs">The Datum to move data from</param>
		Datum(Datum&& rhs) noexcept;
		/// <summary>
		/// The copy assignment operator for an Datum that first clears the existing elements and then performs a copy.
		/// </summary>
		/// <param name="rhs">The Datum to copy</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(const Datum& rhs);
		/// <summary>
		/// The assignment operator for an Datum that first clears the existing elements and then assigns the RHS scalar as a size of 1 datum
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(std::int32_t rhs);
		/// <summary>
		/// The assignment operator for an Datum that first clears the existing elements and then assigns the RHS scalar as a size of 1 datum
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(float rhs);
		/// <summary>
		/// The assignment operator for an Datum that first clears the existing elements and then assigns the RHS scalar as a size of 1 datum
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(const std::string& rhs);
		/// <summary>
		/// The assignment operator for an Datum that first clears the existing elements and then assigns the RHS scalar as a size of 1 datum
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(const glm::vec4& rhs);
		/// <summary>
		/// The assignment operator for an Datum that first clears the existing elements and then assigns the RHS scalar as a size of 1 datum
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(const glm::mat4& rhs);
		/// <summary>
		/// The assignment operator for an Datum that first clears the existing elements and then assigns the RHS scalar as a size of 1 datum
		/// </summary>
		/// <param name="rhs">The scalar to copy</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(RTTI* rhs);
		/// <summary>
		/// A copy assignment operator for an Datum that takes an initializer list. The datum will be cleared/reset before all list items are pushed back into it.
		/// </summary>
		/// <param name="list">The initializer list to use for pushbacks</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(std::initializer_list<std::int32_t> list);
		/// <summary>
		/// A copy assignment operator for an Datum that takes an initializer list. The datum will be cleared/reset before all list items are pushed back into it.
		/// </summary>
		/// <param name="list">The initializer list to use for pushbacks</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(std::initializer_list<float> list);
		/// <summary>
		/// A copy assignment operator for an Datum that takes an initializer list. The datum will be cleared/reset before all list items are pushed back into it.
		/// </summary>
		/// <param name="list">The initializer list to use for pushbacks</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(std::initializer_list<std::string> list);
		/// <summary>
		/// A copy assignment operator for an Datum that takes an initializer list. The datum will be cleared/reset before all list items are pushed back into it.
		/// </summary>
		/// <param name="list">The initializer list to use for pushbacks</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(std::initializer_list<glm::vec4> list);
		/// <summary>
		/// A copy assignment operator for an Datum that takes an initializer list. The datum will be cleared/reset before all list items are pushed back into it.
		/// </summary>
		/// <param name="list">The initializer list to use for pushbacks</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(std::initializer_list<glm::mat4> list);
		/// <summary>
		/// A copy assignment operator for an Datum that takes an initializer list. The datum will be cleared/reset before all list items are pushed back into it.
		/// </summary>
		/// <param name="list">The initializer list to use for pushbacks</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(std::initializer_list<RTTI*> list);
		/// <summary>
		/// The move assignment operator for Datum, which "steals" data from the RHS Datum.
		/// </summary>
		/// <param name="rhs">The Datum to move data from</param>
		/// <returns>A reference to the updated Datum</returns>
		Datum& operator=(Datum&& rhs) noexcept;
		/// <summary>
		/// The destructor for Datum, which destructs its data if it is internally owned.
		/// </summary>
		~Datum();

		/// <summary>
		/// Retrieves the current size of the Datum.
		/// </summary>
		/// <returns>The current size of the Datum.</returns>
		[[nodiscard]] std::size_t Size() const;
		/// <summary>
		/// Retrieves the current capacity of the Datum.
		/// </summary>
		/// <returns>The current capacity of the Datum.</returns>
		[[nodiscard]] std::size_t Capacity() const;
		/// <summary>
		/// Retrieves the current type of the Datum.
		/// </summary>
		/// <returns>The current type of the Datum.</returns>
		[[nodiscard]] DatumTypes Type() const;
		/// <summary>
		/// Retrieves the current status of the Datum's data ownership.
		/// </summary>
		/// <returns>A boolean representing if the Datum is not in charge of the data's memory.</returns>
		[[nodiscard]] bool IsExternal() const;
		/// <summary>
		/// Retrieves whether or not the Datum's size is currently equal to zero.
		/// </summary>
		/// <returns>A boolean indicating whether the Datum is currently empty.</returns>
		[[nodiscard]] bool IsEmpty() const;
		/// <summary>
		/// Retrieves the function used to decide how much to increment the size of the Datum by when performing a PushBack when size == capacity.
		/// </summary>
		/// <returns>A function returning a size_t based on an input size_t</returns>
		[[nodiscard]] IncrementFunctor GetIncrementFunction() const;

		/// <summary>
		/// Sets the type of the Datum. The type cannot be set to 'Unknown' nor can it be changed after being initially changed from 'Unknown.'
		/// </summary>
		/// <param name="type">The new type of the Datum.</param>
		void SetType(DatumTypes type);
		/// <summary>
		/// Set the IncrementFunctor, used to determine how much to increment capacity by when it becomes necessary on a PushBack call.
		/// </summary>
		/// <param name="incrementFunctor">The new functor to be used as an increment strategy.</param>
		void SetIncrementFunction(IncrementFunctor incrementFunctor);
		/// <summary>
		/// Clear the contents of the Datum, setting size to zero. Capacity does not change. Cannot be performed on Datums that are marked external.
		/// </summary>
		void Clear();
		/// <summary>
		/// Shrink the capacity to match the size, if it is originally greater than the current size. Cannot be performed if the Datum is marked as external.
		/// </summary>
		void ShrinkToFit();
		/// <summary>
		/// Reserve capacity for the Datum. Cannot be performed on a Datum that is marked as external.
		/// </summary>
		/// <param name="capacity">The new capacity for the Datum.</param>
		void Reserve(std::size_t capacity);
		/// <summary>
		/// Resize the Datum, either growing or shrinking to match the new capacity. When growing, the Datum will fill "extra" spots with default constructed data. 
		/// Cannot be performed on a Datum marked as external.
		/// </summary>
		/// <param name="newSize">The new size.</param>
		void Resize(std::size_t newSize);

		/// <summary>
		/// Set the Datum's data to refer to externally owned data.
		/// </summary>
		/// <param name="intArray">The external data to point to.</param>
		/// <param name="arraySize">The size of the exernal data.</param>
		void SetStorage(std::int32_t* intArray, std::size_t arraySize);
		/// <summary>
		/// Set the Datum's data to refer to externally owned data.
		/// </summary>
		/// <param name="floatArray">The external data to point to.</param>
		/// <param name="arraySize">The size of the exernal data.</param>
		void SetStorage(float* floatArray, std::size_t arraySize);
		/// <summary>
		/// Set the Datum's data to refer to externally owned data.
		/// </summary>
		/// <param name="stringArray">The external data to point to.</param>
		/// <param name="arraySize">The size of the exernal data.</param>
		void SetStorage(std::string* stringArray, std::size_t arraySize);
		/// <summary>
		/// Set the Datum's data to refer to externally owned data.
		/// </summary>
		/// <param name="vectorArray">The external data to point to.</param>
		/// <param name="arraySize">The size of the exernal data.</param>
		void SetStorage(glm::vec4* vectorArray, std::size_t arraySize);
		/// <summary>
		/// Set the Datum's data to refer to externally owned data.
		/// </summary>
		/// <param name="matrixArray">The external data to point to.</param>
		/// <param name="arraySize">The size of the exernal data.</param>
		void SetStorage(glm::mat4* matrixArray, std::size_t arraySize);
		/// <summary>
		/// Set the Datum's data to refer to externally owned data.
		/// </summary>
		/// <param name="pointerArray">The external data to point to.</param>
		/// <param name="arraySize">The size of the exernal data.</param>
		void SetStorage(RTTI** pointerArray, std::size_t arraySize);

		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the front of Datum.</returns>
		[[nodiscard]] std::int32_t& FrontAsInt();
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the front of Datum.</returns>
		[[nodiscard]] std::int32_t FrontAsInt() const;
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the front of Datum.</returns>
		[[nodiscard]] float& FrontAsFloat();
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the front of Datum.</returns>
		[[nodiscard]] float FrontAsFloat() const;
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the front of Datum.</returns>
		[[nodiscard]] std::string& FrontAsString();
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the front of Datum.</returns>
		[[nodiscard]] const std::string& FrontAsString() const;
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the front of Datum.</returns>
		[[nodiscard]] glm::vec4& FrontAsVector();
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the front of Datum.</returns>
		[[nodiscard]] const glm::vec4& FrontAsVector() const;
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the front of Datum.</returns>
		[[nodiscard]] glm::mat4& FrontAsMatrix();
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the front of Datum.</returns>
		[[nodiscard]] const glm::mat4& FrontAsMatrix() const;
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the front of Datum.</returns>
		[[nodiscard]] RTTI*& FrontAsPointer();
		/// <summary>
		/// Retrieve the value at the front of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the front of Datum.</returns>
		[[nodiscard]] RTTI* FrontAsPointer() const;

		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the back of Datum.</returns>
		[[nodiscard]] std::int32_t& BackAsInt();
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the back of Datum.</returns>
		[[nodiscard]] std::int32_t BackAsInt() const;
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the back of Datum.</returns>
		[[nodiscard]] float& BackAsFloat();
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the back of Datum.</returns>
		[[nodiscard]] float BackAsFloat() const;
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the back of Datum.</returns>
		[[nodiscard]] std::string& BackAsString();
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the back of Datum.</returns>
		[[nodiscard]] const std::string& BackAsString() const;
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the back of Datum.</returns>
		[[nodiscard]] glm::vec4& BackAsVector();
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the back of Datum.</returns>
		[[nodiscard]] const glm::vec4& BackAsVector() const;
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the back of Datum.</returns>
		[[nodiscard]] glm::mat4& BackAsMatrix();
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the back of Datum.</returns>
		[[nodiscard]] const glm::mat4& BackAsMatrix() const;
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the back of Datum.</returns>
		[[nodiscard]] RTTI*& BackAsPointer();
		/// <summary>
		/// Retrieve the value at the back of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the back of Datum.</returns>
		[[nodiscard]] RTTI* BackAsPointer() const;
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] Scope& BackAsTable();
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A constant reference to the value at the specified index.</returns>
		[[nodiscard]] const Scope& BackAsTable() const;

		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] std::int32_t& GetAsInt(std::size_t index = std::size_t(0));
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the specified index.</returns>
		[[nodiscard]] std::int32_t GetAsInt(std::size_t index = std::size_t(0)) const;
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] float& GetAsFloat(std::size_t index = std::size_t(0));
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the specified index.</returns>
		[[nodiscard]] float GetAsFloat(std::size_t index = std::size_t(0)) const;
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] std::string& GetAsString(std::size_t index = std::size_t(0));
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the specified index.</returns>
		[[nodiscard]] const std::string& GetAsString(std::size_t index = std::size_t(0)) const;
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] glm::vec4& GetAsVector(std::size_t index = std::size_t(0));
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the specified index.</returns>
		[[nodiscard]] const glm::vec4& GetAsVector(std::size_t index = std::size_t(0)) const;
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] glm::mat4& GetAsMatrix(std::size_t index = std::size_t(0));
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A const reference to the value at the specified index.</returns>
		[[nodiscard]] const glm::mat4& GetAsMatrix(std::size_t index = std::size_t(0)) const;
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] RTTI*& GetAsPointer(std::size_t index = std::size_t(0));
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A copy of the value at the specified index.</returns>
		[[nodiscard]] RTTI* GetAsPointer(std::size_t index = std::size_t(0)) const;
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A reference to the value at the specified index.</returns>
		[[nodiscard]] Scope& GetAsTable(std::size_t index = std::size_t(0));
		/// <summary>
		/// Retrieve the value at the specified index of the Datum's data array.
		/// </summary>
		/// <returns>A constant reference to the value at the specified index.</returns>
		[[nodiscard]] const Scope& GetAsTable(std::size_t index = std::size_t(0)) const;

		/// <summary>
		/// Set the value at the specified index of the Datum's data array to the given value.
		/// </summary>
		/// <param name="value">The value to set the data to.</param>
		/// <param name="index">The index at which to set the data, defaulted to zero.</param>
		void Set(std::int32_t value, std::size_t index = std::size_t(0));
		/// <summary>
		/// Set the value at the specified index of the Datum's data array to the given value.
		/// </summary>
		/// <param name="value">The value to set the data to.</param>
		/// <param name="index">The index at which to set the data, defaulted to zero.</param>
		void Set(float value, std::size_t index = std::size_t(0));
		/// <summary>
		/// Set the value at the specified index of the Datum's data array to the given value.
		/// </summary>
		/// <param name="value">The value to set the data to.</param>
		/// <param name="index">The index at which to set the data, defaulted to zero.</param>
		void Set(std::string value, std::size_t index = std::size_t(0));
		/// <summary>
		/// Set the value at the specified index of the Datum's data array to the given value.
		/// </summary>
		/// <param name="value">The value to set the data to.</param>
		/// <param name="index">The index at which to set the data, defaulted to zero.</param>
		void Set(glm::vec4 value, std::size_t index = std::size_t(0));
		/// <summary>
		/// Set the value at the specified index of the Datum's data array to the given value.
		/// </summary>
		/// <param name="value">The value to set the data to.</param>
		/// <param name="index">The index at which to set the data, defaulted to zero.</param>
		void Set(glm::mat4 value, std::size_t index = std::size_t(0));
		/// <summary>
		/// Set the value at the specified index of the Datum's data array to the given value.
		/// </summary>
		/// <param name="value">The value to set the data to.</param>
		/// <param name="index">The index at which to set the data, defaulted to zero.</param>
		void Set(RTTI* value, std::size_t index = std::size_t(0));

		/// <summary>
		/// Add the given value to the end of the Datum.
		/// </summary>
		/// <param name="data">The value to push onto Datum's data array.</param>
		void PushBack(std::int32_t data);
		/// <summary>
		/// Add the given value to the end of the Datum.
		/// </summary>
		/// <param name="data">The value to push onto Datum's data array.</param>
		void PushBack(float data);
		/// <summary>
		/// Add the given value to the end of the Datum.
		/// </summary>
		/// <param name="data">The value to push onto Datum's data array.</param>
		void PushBack(const std::string& data);
		/// <summary>
		/// Add the given value to the end of the Datum.
		/// </summary>
		/// <param name="data">The value to push onto Datum's data array.</param>
		void PushBack(const glm::vec4& data);
		/// <summary>
		/// Add the given value to the end of the Datum.
		/// </summary>
		/// <param name="data">The value to push onto Datum's data array.</param>
		void PushBack(const glm::mat4& data);
		/// <summary>
		/// Add the given value to the end of the Datum.
		/// </summary>
		/// <param name="data">The value to push onto Datum's data array.</param>
		void PushBack(RTTI* data);

		/// <summary>
		/// Remove the back value from the Datum. Does not reduce the Datum's capacity. Cannot be performed on a Datum marked as external.
		/// </summary>
		void PopBack();
		/// <summary>
		/// Look for an item in the Datum and remove the first instance of it if it's found. Does not decrease the Datum's capacity.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		bool Remove(std::int32_t data);
		/// <summary>
		/// Look for an item in the Datum and remove the first instance of it if it's found. Does not decrease the Datum's capacity.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		bool Remove(float data);
		/// <summary>
		/// Look for an item in the Datum and remove the first instance of it if it's found. Does not decrease the Datum's capacity.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		bool Remove(const std::string& data);
		/// <summary>
		/// Look for an item in the Datum and remove the first instance of it if it's found. Does not decrease the Datum's capacity.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		bool Remove(const glm::vec4& data);
		/// <summary>
		/// Look for an item in the Datum and remove the first instance of it if it's found. Does not decrease the Datum's capacity.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		bool Remove(const glm::mat4& data);
		/// <summary>
		/// Look for an item in the Datum and remove the first instance of it if it's found. Does not decrease the Datum's capacity.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		bool Remove(RTTI* data);
		/// <summary>
		/// Remove the item at the given index. Does not decrease the Datum's capacity.
		/// </summary>
		/// <param name="index">The index to remove</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		bool RemoveAt(std::size_t index);
		
		/// <summary>
		/// Look for the given value in the Datum, and return the index of that value if it's present. Otherwise, return the size of the Datum.
		/// </summary>
		/// <param name="value">The value to look for.</param>
		/// <returns>The index the value exists at.</returns>
		std::size_t FindIndex(std::int32_t value) const;
		/// <summary>
		/// Look for the given value in the Datum, and return the index of that value if it's present. Otherwise, return the size of the Datum.
		/// </summary>
		/// <param name="value">The value to look for.</param>
		/// <returns>The index the value exists at.</returns>
		std::size_t FindIndex(float value) const;
		/// <summary>
		/// Look for the given value in the Datum, and return the index of that value if it's present. Otherwise, return the size of the Datum.
		/// </summary>
		/// <param name="value">The value to look for.</param>
		/// <returns>The index the value exists at.</returns>
		std::size_t FindIndex(const std::string& value) const;
		/// <summary>
		/// Look for the given value in the Datum, and return the index of that value if it's present. Otherwise, return the size of the Datum.
		/// </summary>
		/// <param name="value">The value to look for.</param>
		/// <returns>The index the value exists at.</returns>
		std::size_t FindIndex(const glm::vec4& value) const;
		/// <summary>
		/// Look for the given value in the Datum, and return the index of that value if it's present. Otherwise, return the size of the Datum.
		/// </summary>
		/// <param name="value">The value to look for.</param>
		/// <returns>The index the value exists at.</returns>
		std::size_t FindIndex(const glm::mat4& value) const;
		/// <summary>
		/// Look for the given value in the Datum, and return the index of that value if it's present. Otherwise, return the size of the Datum.
		/// </summary>
		/// <param name="value">The value to look for.</param>
		/// <returns>The index the value exists at.</returns>
		std::size_t FindIndex(RTTI* value) const;
		/// <summary>
		/// Look for the given value in the Datum, and return the index of that value if it's present. Otherwise, return the size of the Datum.
		/// </summary>
		/// <param name="value">The value to look for.</param>
		/// <returns>The index the value exists at.</returns>
		std::size_t FindIndex(Scope& value) const;

		/// <summary>
		/// Push back a value based on the string passed in. The string should conform to standardized format for each type.
		/// </summary>
		/// <param name="stringValue">The string representing a value.</param>
		void PushBackFromString(const std::string& stringValue);
		/// <summary>
		/// Set the value at the given index based on the string passed in. The string should conform to standardized format for each type.
		/// </summary>
		/// <param name="stringValue">The string representing a value.</param>
		/// <param name="index">The index to set, defaulted to zero.</param>
		void SetFromString(const std::string& stringValue, std::size_t index = std::size_t(0));
		/// <summary>
		/// Return the string representation of the value at the given index.
		/// </summary>
		/// <param name="index">The index to convert, defaulted to zero.</param>
		/// <returns>The string representation of the specified value.</returns>
		std::string ToString(std::size_t index = std::size_t(0)) const;

		/// <summary>
		/// Test the equality of the Datum to another Datum.
		/// </summary>
		/// <param name="rhs">The other Datum to compare against.</param>
		/// <returns>Whether the two Datums are equivilant.</returns>
		[[nodiscard]] bool operator==(const Datum& rhs) const;
		/// <summary>
		/// Test the equality of the Datum against a scalar value. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator==(const std::int32_t rhs) const;
		/// <summary>
		/// Test the equality of the Datum against a scalar value. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator==(const float rhs) const;
		/// <summary>
		/// Test the equality of the Datum against a scalar value. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator==(const std::string& rhs) const;
		/// <summary>
		/// Test the equality of the Datum against a scalar value. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator==(const glm::vec4& rhs) const;
		/// <summary>
		/// Test the equality of the Datum against a scalar value. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator==(const glm::mat4& rhs) const;
		/// <summary>
		/// Test the equality of the Datum against a scalar value. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator==(const RTTI* rhs) const;

		/// <summary>
		/// Test the equality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator==(const std::int32_t lhs, const Datum& rhs) { return rhs.operator==(lhs); }
		/// <summary>
		/// Test the equality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator==(const float lhs, const Datum& rhs) { return rhs.operator==(lhs); }
		/// <summary>
		/// Test the equality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator==(const std::string& lhs, const Datum& rhs) { return rhs.operator==(lhs); }
		/// <summary>
		/// Test the equality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator==(const glm::vec4& lhs, const Datum& rhs) { return rhs.operator==(lhs); }
		/// <summary>
		/// Test the equality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator==(const glm::mat4& lhs, const Datum& rhs) { return rhs.operator==(lhs); }
		/// <summary>
		/// Test the equality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons. Datum must be size of 1 for this to ever be true.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator==(const RTTI* lhs, const Datum& rhs) { return rhs.operator==(lhs); }

		/// <summary>
		/// Test the inequality of the Datum to another Datum.
		/// </summary>
		/// <param name="rhs">The other Datum to compare against.</param>
		/// <returns>Whether the two Datums are not equivilant.</returns>
		[[nodiscard]] bool operator!=(const Datum& rhs) const;
		/// <summary>
		/// Test the inequality of the Datum against a scalar value.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator!=(const std::int32_t rhs) const;
		/// <summary>
		/// Test the inequality of the Datum against a scalar value.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator!=(const float rhs) const;
		/// <summary>
		/// Test the inequality of the Datum against a scalar value.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator!=(const std::string& rhs) const;
		/// <summary>
		/// Test the inequality of the Datum against a scalar value.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator!=(const glm::vec4& rhs) const;
		/// <summary>
		/// Test the inequality of the Datum against a scalar value.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator!=(const glm::mat4& rhs) const;
		/// <summary>
		/// Test the inequality of the Datum against a scalar value.
		/// </summary>
		/// <param name="rhs">The scalar value to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to this scalar value.</returns>
		[[nodiscard]] bool operator!=(const RTTI* rhs) const;

		/// <summary>
		/// Test the unequality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator!=(const std::int32_t lhs, const Datum& rhs) { return rhs.operator!=(lhs); }
		/// <summary>
		/// Test the unequality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator!=(const float lhs, const Datum& rhs) { return rhs.operator!=(lhs); }
		/// <summary>
		/// Test the unequality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator!=(const std::string& lhs, const Datum& rhs) { return rhs.operator!=(lhs); }
		/// <summary>
		/// Test the unequality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator!=(const glm::vec4& lhs, const Datum& rhs) { return rhs.operator!=(lhs); }
		/// <summary>
		/// Test the unequality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator!=(const glm::mat4& lhs, const Datum& rhs) { return rhs.operator!=(lhs); }
		/// <summary>
		/// Test the unequality of the Datum against a scalar value, set as a friend function to allow for reverse ordered comparisons.
		/// </summary>
		/// <param name="lhs">The scalar value to compare against.</param>
		/// <param name="rhs">The Datum to compare against.</param>
		/// <returns>Whether the Datum is not equivilant to the scalar value.</returns>
		[[nodiscard]] friend bool operator!=(const RTTI* lhs, const Datum& rhs) { return rhs.operator!=(lhs); }


	private:
		/// <summary>
		/// The union type specifying the sorts of lenses we can look at the Datum's data through.
		/// </summary>
		union DatumValues final {
			std::int32_t* i;
			float* f;
			std::string* s;
			glm::vec4* v;
			glm::mat4* m;
			RTTI** p;
			Scope** t;
			void* vp;
		};

		/// <summary>
		/// The data of the Datum.
		/// </summary>
		DatumValues _data{ nullptr };
		/// <summary>
		/// The size of the Datum.
		/// </summary>
		std::size_t _size{ 0 };
		/// <summary>
		/// The capacity of the Datum.
		/// </summary>
		std::size_t _capacity{ 0 };
		/// <summary>
		/// The type of the Datum.
		/// </summary>
		DatumTypes _type{ DatumTypes::Unknown };
		/// <summary>
		/// Whether the Datum has been marked external and therefore does not own its memory.
		/// </summary>
		bool _isExternal{ false };

		/// <summary>
		/// A function that will return how much to increment the capacity of the Datum by when called on a certain current capacity.
		/// </summary>
		IncrementFunctor _incrementFunctor;
		/// <summary>
		/// A lookup table to retrieve the size of any of the possible Datum Types.
		/// </summary>
		static constexpr std::size_t _typeSizes[static_cast<std::int32_t>(DatumTypes::End) + 1] = {
			0,						//Datum::Types::Unknown
			sizeof(std::int32_t),	//Datum::Types::Integer
			sizeof(float),			//Datum::Types::Float			
			sizeof(std::string),	//Datum::Types::String
			sizeof(glm::vec4),		//Datum::Types::Vector
			sizeof(glm::mat4),		//Datum::Types::Matrix
			sizeof(RTTI*),			//Datum::Types::Pointer
			sizeof(Scope*),			//Datum::Types::Table
		};

		/// <summary>
		/// A helper function to wipe any internal storage the Datum currently has, both size and capacity.
		/// </summary>
		void ResetInternalStorage();
		/// <summary>
		/// A helper function to copy RHS into this, behaving as either a shallow or deep copy based on whether RHS is marked external or not.
		/// </summary>
		/// <param name="rhs">The Datum to copy.</param>
		void CopyHelper(const Datum& rhs);
		/// <summary>
		/// A helper function to steal RHS Datum's data through a pointer swap.
		/// </summary>
		/// <param name="rhs">The Datum to copy / steal from.</param>
		void CopySwapHelper(Datum& rhs);
		/// <summary>
		/// A helper function to set the Datum's data to refer to externally owned data regardless of data type.
		/// </summary>
		/// <param name="array">The external data to point to.</param>
		/// <param name="arraySize">The size of the external data.</param>
		/// <param name="type">The type of data.</param>
		void SetStorage(void* array, std::size_t arraySize, DatumTypes type);
		/// <summary>
		/// A helper function that prepares for a pushback, checking if the current size == capacity and incrementing the capacity if needed.
		/// </summary>
		/// <param name="type">The type of data the Datum is.</param>
		void PushBackPrep(DatumTypes type);

		/// <summary>
		/// Add the given value to the end of the Datum.
		/// </summary>
		/// <param name="data">The value to push onto Datum's data array.</param>
		void PushBack(Scope& data);

		/// <summary>
		/// Helper function that PushBacks a default constructed value. 
		/// </summary>
		void PushDefaultInt();
		/// <summary>
		/// Helper function that PushBacks a default constructed value. 
		/// </summary>
		void PushDefaultFloat();
		/// <summary>
		/// Helper function that PushBacks a default constructed value. 
		/// </summary>
		void PushDefaultString();
		/// <summary>
		/// Helper function that PushBacks a default constructed value. 
		/// </summary>
		void PushDefaultVector();
		/// <summary>
		/// Helper function that PushBacks a default constructed value. 
		/// </summary>
		void PushDefaultMatrix();
		/// <summary>
		/// Helper function that PushBacks a default constructed value. 
		/// </summary>
		void PushDefaultPointer();

		/// <summary>
		/// Retrieve the string representation of the value at the current index.
		/// </summary>
		/// <param name="index">The index to convert the value at.</param>
		/// <returns>A string representation of the specified value.</returns>
		std::string StringFromInt(size_t index) const;
		/// <summary>
		/// Retrieve the string representation of the value at the current index.
		/// </summary>
		/// <param name="index">The index to convert the value at.</param>
		/// <returns>A string representation of the specified value.</returns>
		std::string StringFromFloat(size_t index) const;
		/// <summary>
		/// Retrieve the string representation of the value at the current index.
		/// </summary>
		/// <param name="index">The index to convert the value at.</param>
		/// <returns>A string representation of the specified value.</returns>
		std::string StringFromString(size_t index) const;
		/// <summary>
		/// Retrieve the string representation of the value at the current index.
		/// </summary>
		/// <param name="index">The index to convert the value at.</param>
		/// <returns>A string representation of the specified value.</returns>
		std::string StringFromVector(size_t index) const;
		/// <summary>
		/// Retrieve the string representation of the value at the current index.
		/// </summary>
		/// <param name="index">The index to convert the value at.</param>
		/// <returns>A string representation of the specified value.</returns>
		std::string StringFromMatrix(size_t index) const;
		/// <summary>
		/// Retrieve the string representation of the value at the current index.
		/// </summary>
		/// <param name="index">The index to convert the value at.</param>
		/// <returns>A string representation of the specified value.</returns>
		std::string StringFromPointer(size_t index) const;
		/// <summary>
		/// Retrieve the string representation of the value at the current index.
		/// </summary>
		/// <param name="index">The index to convert the value at.</param>
		/// <returns>A string representation of the specified value.</returns>
		std::string StringFromTable(size_t index) const;

		/// <summary>
		/// Set the value at the given index to a value specified via the given string.
		/// </summary>
		/// <param name="strValue">The string representing which value to set the data to.</param>
		/// <param name="index">The index at which to set the data.</param>
		void SetIntFromString(const std::string& strValue, size_t index);
		/// <summary>
		/// Set the value at the given index to a value specified via the given string.
		/// </summary>
		/// <param name="strValue">The string representing which value to set the data to.</param>
		/// <param name="index">The index at which to set the data.</param>
		void SetFloatFromString(const std::string& strValue, size_t index);
		/// <summary>
		/// Set the value at the given index to a value specified via the given string.
		/// </summary>
		/// <param name="strValue">The string representing which value to set the data to.</param>
		/// <param name="index">The index at which to set the data.</param>
		void SetStringFromString(const std::string& strValue, size_t index);
		/// <summary>
		/// Set the value at the given index to a value specified via the given string.
		/// </summary>
		/// <param name="strValue">The string representing which value to set the data to.</param>
		/// <param name="index">The index at which to set the data.</param>
		void SetVectorFromString(const std::string& strValue, size_t index);
		/// <summary>
		/// Set the value at the given index to a value specified via the given string.
		/// </summary>
		/// <param name="strValue">The string representing which value to set the data to.</param>
		/// <param name="index">The index at which to set the data.</param>
		void SetMatrixFromString(const std::string& strValue, size_t index);

		/// <summary>
		/// Helper function for Set and Get calls that checks if the type and index are valid.
		/// </summary>
		/// <param name="type">The type of the given value</param>
		/// <param name="index">The index to set at</param>
		void GetSetChecker(DatumTypes type, size_t index) const;
		/// <summary>
		/// Helper function for Front and Back calls that checks if the type and size are valid.
		/// </summary>
		/// <param name="type">The type of the given value</param>
		void FrontBackChecker(DatumTypes type) const;

		/// <summary>
		/// A function lookup table for pushing back a default constructed value based on the Datum's type.
		/// </summary>
		using PushDefaultFunction = void(Datum::*)();
		inline static const PushDefaultFunction _defaultPlaceFunction [static_cast<std::int32_t>(DatumTypes::End) + 1] = {
			nullptr,					//Datum::Types::Unknown
			&Datum::PushDefaultInt,		//Datum::Types::Integer
			&Datum::PushDefaultFloat,	//Datum::Types::Float			
			&Datum::PushDefaultString,	//Datum::Types::String
			&Datum::PushDefaultVector,	//Datum::Types::Vector
			&Datum::PushDefaultMatrix,	//Datum::Types::Matrix
			&Datum::PushDefaultPointer, //Datum::Types::Pointer
			nullptr						//Datum::Types::Table
		};

		/// <summary>
		/// A function lookup table for retrieving the string representation of a given value based on the Datum's type.
		/// </summary>
		using ToStringFunction = std::string(Datum::*)(std::size_t) const;
		inline static const ToStringFunction _toStringFunctions[static_cast<std::int32_t>(DatumTypes::End) + 1] = {
			nullptr,					//Datum::Types::Unknown
			&Datum::StringFromInt,		//Datum::Types::Integer
			&Datum::StringFromFloat,	//Datum::Types::Float			
			&Datum::StringFromString,	//Datum::Types::String
			&Datum::StringFromVector,	//Datum::Types::Vector
			&Datum::StringFromMatrix,	//Datum::Types::Matrix
			&Datum::StringFromPointer,	//Datum::Types::Pointer
			&Datum::StringFromTable,	//Datum::Types::Table
		};

		/// <summary>
		/// A function lookup table for setting the specified value to data represented in string format based on the Datum's type.
		/// </summary>
		using SetFromStringFunction = void(Datum::*)(const std::string&, std::size_t);
		inline static const SetFromStringFunction _setFromStringFunctions[static_cast<std::int32_t>(DatumTypes::End) + 1] = {
			nullptr,						//Datum::Types::Unknown
			&Datum::SetIntFromString,		//Datum::Types::Integer
			&Datum::SetFloatFromString,		//Datum::Types::Float			
			&Datum::SetStringFromString,	//Datum::Types::String
			&Datum::SetVectorFromString,	//Datum::Types::Vector
			&Datum::SetMatrixFromString,	//Datum::Types::Matrix
			nullptr,						//Datum::Types::Pointer
			nullptr,						//Datum::Types::Table
		};
	};
}

#include "Datum.inl"
