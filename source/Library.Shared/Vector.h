/// <summary>
/// The declaration of the templated class Vector, as well as its enveloped classes Iterator and ConstIterator.
/// </summary>

#pragma once

#include <cstddef>
#include <memory>
#include <cassert>
#include <algorithm>
#include <limits>
#include <functional>
#include <stdexcept>
#include <DefaultEquality.h>
#include "DefaultGrowth.h"

namespace FIEAGameEngine {
	/// <summary>
	/// A templated container class for a vector. 
	/// Vectors have Iterators and ConstIterators and move semantics are implemented in assignments, copy constructors, and pushbacks. 
	/// Capacity of a Vector can only be decreased with a call to ShrinkToFit.
	/// </summary>
	/// <typeparam name="T">The type of object the Vector contains.</typeparam>
	template <typename T>
	class Vector final {
	public:
		using size_type = std::size_t;
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using rvalue_reference = T&&;

		/// <summary>
		/// Iterator class allows traversal of Vector objects. Iterators function as Random Access Iterators and have all related functionaliy.
		/// Dereferencing an Iterator with an index out of bounds of the size of its Vector or trying to increment an Iterator that is not assigned to an Vector will result in a runtime error.
		/// </summary>
		class Iterator final{
			friend Vector;
			friend class ConstIterator;

		public:
			//difference_type, pointer, and iterator_category choices are taken from information on https://en.cppreference.com/w/cpp/iterator/iterator_traits
			//This allows usage of std:: functions requiring iterator types with this custom Iterator, i.e. std::equal
			using size_type = Vector::size_type;
			using value_type = Vector::value_type;
			using reference = Vector::reference;
			using const_reference = Vector::const_reference;
			using rvalue_reference = Vector::rvalue_reference;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using iterator_category = std::random_access_iterator_tag;

			/// <summary>
			/// Default constructor for an Iterator. Its index will be set to zero and its Vector* container will be set to nullptr.
			/// </summary>
			Iterator() = default;
			/// <summary>
			/// Copy constructor for an Iterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The Iterator to copy</param>
			Iterator(const Iterator& rhs) = default;
			/// <summary>
			/// Move constructor for an Iterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The Iterator to move data from</param>
			Iterator(Iterator&& rhs) noexcept = default;
			/// <summary>
			/// The copy assignment operator for an Iterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The Iterator to copy</param>
			/// <returns>An Iterator reference to the copy</returns>
			Iterator& operator=(const Iterator& rhs) = default;
			/// <summary>
			/// The move assignment operator for an Iterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The Iterator to move data from</param>
			/// <returns>An Iterator reference to the copy</returns>
			Iterator& operator=(Iterator&& rhs) noexcept = default;
			/// <summary>
			/// The destructor for an Iterator. Using the compiler provided default behavior.
			/// </summary>
			~Iterator() = default;

			/// <summary>
			/// Dereference an Iterator.
			/// </summary>
			/// <returns>A reference to the item the Iterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator's data is equal to end() / if the data is equal to nullptr</exception>
			[[nodiscard]] reference operator*() const;
			/// <summary>
			/// Pre-increment an Iterator. If USE_CLAMPS is defined, the Iterator will not increment past the max size of size_type.
			/// </summary>
			/// <returns>A reference to an Iterator, now pointing to the next item in the vector</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator is not associted with an Vector</exception>
			Iterator& operator++();
			/// <summary>
			/// Post-increment an Iterator. Calls the pre-fixed increment as part of it. If USE_CLAMPS is defined, the Iterator will not increment past the max size of size_type.
			/// </summary>
			/// <param name="">Unused parameter to disambiguate between post and pre increment overloads</param>
			/// <returns>An Iterator pointing to the item in the vector that the incremented Iterator used to be pointing to</returns>
			Iterator operator++(int);
			/// <summary>
			/// Pre-decrement an Iterator. If USE_CLAMPS is defined, the Iterator will not decrement past 0.
			/// </summary>
			/// <returns>A reference to an Iterator, now pointing to the previous item in the vector</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator is not associted with an Vector</exception>
			Iterator& operator--();
			/// <summary>
			/// Post-decrement an Iterator. Calls the pre-fixed decrement as part of it. If USE_CLAMPS is defined, the Iterator will not decrement past 0.
			/// </summary>
			/// <param name="">Unused parameter to disambiguate between post and pre decrement overloads</param>
			/// <returns>An Iterator pointing to the item in the vector that the decremented Iterator used to be pointing to</returns>
			Iterator operator--(int);
			/// <summary>
			/// Iterator difference operator. Returns the distance between two Iterators.
			/// </summary>
			/// <param name="rhs">The iterator to compare to.</param>
			/// <returns>A difference_type that represents the difference in indecies between the two Iterators.</returns>
			difference_type operator-(const Iterator& rhs) const;
			/// <summary>
			/// Addition Assignment operator. Increments the iterator by the specified amount.
			/// </summary>
			/// <param name="incrementAmount">A difference_type specifying how much to increment by.</param>
			/// <returns>A reference to the Iterator, now pointing to the item incrementAmount away from what it used to be pointing to.</returns>
			Iterator& operator+=(difference_type incrementAmount);
			/// <summary>
			/// Subtraction Assignment operator. Decrements the iterator by the specified amount.
			/// </summary>
			/// <param name="incrementAmount">A difference_type specifying how much to decrement by.</param>
			/// <returns>A reference to the Iterator, now pointing to the item decrementAmount away from what it used to be pointing to.</returns>
			Iterator& operator-=(difference_type decrementAmount);
			/// <summary>
			/// Bracket access operator. Repositions the Iterator to point to the specified index and returns the item there.
			/// </summary>
			/// <param name="index">The index to point to.</param>
			/// <returns>The item contained at the specified index.</returns>
			reference operator[](difference_type index);

			/// <summary>
			/// A friend function of Iterator that returns a new Iterator pointing a specified distance away from the original Iterator.
			/// </summary>
			/// <param name="iter">The Iterator to use as a base.</param>
			/// <param name="incrementAmount">The offset to the new Iterator.</param>
			/// <returns>A new Iterator pointing incrementAmount away (additively) from the original Iterator.</returns>
			friend Iterator operator+(const Iterator& iter, difference_type incrementAmount) {
#ifdef USE_EXCEPTIONS
				if (iter._container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

				return Iterator(*iter._container, iter._index + incrementAmount);
			}
			/// <summary>
			/// A friend function of Iterator that returns a new Iterator pointing a specified distance away from the original Iterator.
			/// </summary>
			/// <param name="iter">The Iterator to use as a base.</param>
			/// <param name="incrementAmount">The offset to the new Iterator.</param>
			/// <returns>A new Iterator pointing decrementAmount away (subtractively) from the original Iterator.</returns>
			friend Iterator operator-(const Iterator& iter, difference_type decrementAmount) {
#ifdef USE_EXCEPTIONS
				if (iter._container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

				return Iterator(*iter._container, iter._index - decrementAmount);
			}

			/// <summary>
			/// Test if two Iterators are not equal to each other. Iterators are not equal to each other if they either do not belong to the same Vector or if they do not point to the same index.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterators are not equal to each other</returns>
			[[nodiscard]] bool operator!=(const Iterator& rhs) const;
			/// <summary>
			/// Test if two Iterators are equal to each other. Iterators are equal to each other if they both belong to the same Vector and point to the same index.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterators are equal to each other</returns>
			[[nodiscard]] bool operator==(const Iterator& rhs) const;
			/// <summary>
			/// Test if an Iterator is less than or equal to the other. An Iterator is less than or equal to another if they belong to the same Vector and its index is less than or equal to the other.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterator is less than or equal to the other.</returns>
			[[nodiscard]] bool operator<=(const Iterator& rhs) const;
			/// <summary>
			/// Test if an Iterator is greater than or equal to the other. An Iterator is greater than or equal to another if they belong to the same Vector and its index is greater than or equal to the other.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterator is greater than or equal to the other.</returns>
			[[nodiscard]] bool operator>=(const Iterator& rhs) const;
			/// <summary>
			/// Test if an Iterator is less than the other. An Iterator is less than another if they belong to the same Vector and its index is less than the other.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterator is less than the other.</returns>
			[[nodiscard]] bool operator<(const Iterator& rhs) const;
			/// <summary>
			/// Test if an Iterator is greater than the other. An Iterator is greater than another if they belong to the same Vector and its index is greater than the other.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterator is greater than the other.</returns>
			[[nodiscard]] bool operator>(const Iterator& rhs) const;
		private:
			/// <summary>
			/// Private constructor for an Iterator that sets its container and index information with parameterized input.
			/// </summary>
			/// <param name="container">The Vector this Iterator belongs to</param>
			/// <param name="index">The index this Iterator points at, defaulted to nullptr</param>
			Iterator(Vector& container, size_type index = 0);

			/// <summary>
			/// The index this Iterator points to.
			/// </summary>
			size_type _index{ 0 };
			/// <summary>
			/// The Vector this Iterator belongs to.
			/// </summary>
			Vector* _container{ nullptr };
		};

		/// <summary>
		/// ConstIterator class allows traversal of Vector objects. Functionally almost identical to Iterator class, except that dereferencing a ConstIterator returns a const T reference.
		/// Dereferencing an Iterator with an index out of bounds of the size of its Vector or trying to increment a ConstIterator that is not assigned to an Vector will result in a runtime error.
		/// </summary>
		class ConstIterator final {
			friend Vector;

		public:
			//difference_type, pointer, and iterator_category choices are taken from information on https://en.cppreference.com/w/cpp/iterator/iterator_traits
			//This allows usage of std:: functions requiring iterator types with this custom Iterator, i.e. std::equal
			using size_type = Vector::size_type;
			using value_type = Vector::value_type;
			using reference = Vector::reference;
			using const_reference = Vector::const_reference;
			using rvalue_reference = Vector::rvalue_reference;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using iterator_category = std::random_access_iterator_tag;

			/// <summary>
			/// Default constructor for a ConstIterator. Its index and Vector* container will both be set to nullptr.
			/// </summary>
			ConstIterator() = default;
			/// <summary>
			/// A type-cast constructor for a ConstIterator that takes in a ConstIterator and copies its information to create a ConstIterator pointing at the same index in the same Vector.
			/// </summary>
			/// <param name="other">The Iterator to copy information from</param>
			ConstIterator(const Iterator& other);
			/// <summary>
			/// Copy constructor for a ConstIterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The ConstIterator to copy</param>
			ConstIterator(const ConstIterator& rhs) = default;
			/// <summary>
			/// Move constructor for a ConstIterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The ConstIterator to move data from</param>
			ConstIterator(ConstIterator&& rhs) noexcept = default;
			/// <summary>
			/// The copy assignment operator for an ConstIterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The ConstIterator to copy</param>
			/// <returns>A ConstIterator reference to the copy</returns>
			ConstIterator& operator=(const ConstIterator& rhs) = default;
			/// <summary>
			/// The move assignment operator for a ConstIterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The ConstIterator to move data from</param>
			/// <returns>A ConstIterator reference to the copy</returns>
			ConstIterator& operator=(ConstIterator&& rhs) noexcept = default;
			/// <summary>
			/// The destructor for a ConstIterator. Using the compiler provided default behavior.
			/// </summary>
			~ConstIterator() = default;

			/// <summary>
			/// Dereference a ConstIterator.
			/// </summary>
			/// <returns>A reference to the item the ConstIterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the ConstIterator's data is equal to end() / if the data is equal to nullptr</exception>
			[[nodiscard]] const_reference operator*() const;
			/// <summary>
			/// Pre-increment an ConstIterator.
			/// </summary>
			/// <returns>A reference to a ConstIterator, now pointing to the next item in the vector</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the ConstIterator is not associted with an Vector</exception>
			ConstIterator& operator++();
			/// <summary>
			/// Post-increment an ConstIterator. Calls the pre-fixed increment as part of it.
			/// </summary>
			/// <param name="">Unused parameter to disambiguate between post and pre increment overloads</param>
			/// <returns>A ConstIterator pointing to the item in the vector that the incremented ConstIterator used to be pointing to</returns>
			ConstIterator operator++(int);
			/// <summary>
			/// Pre-decrement an ConstIterator.
			/// </summary>
			/// <returns>A reference to an ConstIterator, now pointing to the previous item in the vector</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the ConstIterator is not associted with an Vector</exception>
			ConstIterator& operator--();
			/// <summary>
			/// Post-decrement an ConstIterator. Calls the pre-fixed decrement as part of it.
			/// </summary>
			/// <param name="">Unused parameter to disambiguate between post and pre decrement overloads</param>
			/// <returns>A ConstIterator pointing to the item in the vector that the decremented ConstIterator used to be pointing to</returns>
			ConstIterator operator--(int);
			/// <summary>
			/// ConstIterator difference operator. Returns the distance between two ConstIterators.
			/// </summary>
			/// <param name="rhs">The const iterator to compare to.</param>
			/// <returns>A difference_type that represents the difference in indecies between the two ConstIterators.</returns>
			difference_type operator-(const ConstIterator& rhs) const;
			/// <summary>
			/// Addition Assignment operator. Increments the const iterator by the specified amount.
			/// </summary>
			/// <param name="incrementAmount">A difference_type specifying how much to increment by.</param>
			/// <returns>A reference to the ConstIterator, now pointing to the item incrementAmount away from what it used to be pointing to.</returns>
			ConstIterator& operator+=(difference_type incrementAmount);
			/// <summary>
			/// Subtraction Assignment operator. Decrements the const iterator by the specified amount.
			/// </summary>
			/// <param name="incrementAmount">A difference_type specifying how much to decrement by.</param>
			/// <returns>A reference to the ConstIterator, now pointing to the item decrementAmount away from what it used to be pointing to.</returns>
			ConstIterator& operator-=(difference_type decrementAmount);
			/// <summary>
			/// Bracket access operator. Repositions the ConstIterator to point to the specified index and returns the item there.
			/// </summary>
			/// <param name="index">The index to point to.</param>
			/// <returns>The item contained at the specified index.</returns>
			const_reference operator[](difference_type index);

			/// <summary>
			/// A friend function of ConstIterator that returns a new ConstIterator pointing a specified distance away from the original ConstIterator.
			/// </summary>
			/// <param name="iter">The ConstIterator to use as a base.</param>
			/// <param name="incrementAmount">The offset to the new ConstIterator.</param>
			/// <returns>A new ConstIterator pointing incrementAmount away (additively) from the original ConstIterator.</returns>
			friend ConstIterator operator+(const ConstIterator& iter, difference_type incrementAmount) {
#ifdef USE_EXCEPTIONS
				if (iter._container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

				return ConstIterator(*iter._container, iter._index + incrementAmount);
			}
			/// <summary>
			/// A friend function of ConstIterator that returns a new ConstIterator pointing a specified distance away from the original ConstIterator.
			/// </summary>
			/// <param name="iter">The ConstIterator to use as a base.</param>
			/// <param name="incrementAmount">The offset to the new ConstIterator.</param>
			/// <returns>A new ConstIterator pointing decrementAmount away (subtractively) from the original ConstIterator.</returns>
			friend ConstIterator operator-(const ConstIterator& iter, difference_type decrementAmount) {
#ifdef USE_EXCEPTIONS
				if (iter._container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

				return ConstIterator(*iter._container, iter._index - decrementAmount);
			}

			/// <summary>
			/// Test if two ConstIterators are not equal to each other. ConstIterators are not equal to each other if they either do not belong to the same Vector or if they do not point to the same index.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterators are not equal to each other</returns>
			[[nodiscard]] bool operator!=(const ConstIterator& rhs) const;
			/// <summary>
			/// Test if two ConstIterators are equal to each other. ConstIterators are equal to each other if they both belong to the same Vector and point to the same index.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterators are equal to each other</returns>
			[[nodiscard]] bool operator==(const ConstIterator& rhs) const;
			/// <summary>
			/// Test if an ConstIterator is less than or equal to the other. An ConstIterator is less than or equal to another if they belong to the same Vector and its index is less than or equal to the other.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterator is less than or equal to the other.</returns>
			[[nodiscard]] bool operator<=(const ConstIterator& rhs) const;
			/// <summary>
			/// Test if an ConstIterator is greater than or equal to the other. An ConstIterator is greater than or equal to another if they belong to the same Vector and its index is greater than or equal to the other.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterator is greater than or equal to the other.</returns>
			[[nodiscard]] bool operator>=(const ConstIterator& rhs) const;
			/// <summary>
			/// Test if an ConstIterator is less than the other. An ConstIterator is less than another if they belong to the same Vector and its index is less than the other.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterator is less than the other.</returns>
			[[nodiscard]] bool operator<(const ConstIterator& rhs) const;
			/// <summary>
			/// Test if an ConstIterator is greater than the other. An ConstIterator is greater than another if they belong to the same Vector and its index is greater than the other.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterator is greater than the other.</returns>
			[[nodiscard]] bool operator>(const ConstIterator& rhs) const;
		private:
			/// <summary>
			/// Private constructor for an ConstIterator that sets its container and index information with parameterized input.
			/// </summary>
			/// <param name="container">The Vector this ConstIterator belongs to</param>
			/// <param name="index">The index this ConstIterator points at, defaulted to nullptr</param>
			ConstIterator(const Vector& container, size_type index = 0);

			/// <summary>
			/// The index this ConstIterator points to.
			/// </summary>
			size_type _index{ 0 };
			/// <summary>
			/// The Vector this ConstIterator belongs to.
			/// </summary>
			const Vector* _container{ nullptr };
		};

		/// <summary>
		/// Default constructor for an Vector, using the compiler provided default behavior. Its front and back pointers will both be set to nullptr, and its size to zero.
		/// </summary>
		explicit Vector(size_type capacity = size_type(0), std::function<size_type(size_type)> incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// An Initializer list constructor for Vector. The items will be pushed back to the constructor Vector in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an Vector as it's being constructed</param>
		Vector(std::initializer_list<value_type> list, std::function<size_type(size_type)> incrementFunctor = DefaultGrowth{});
		/// <summary>
		/// A copy constructor for an Vector that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The Vector to copy</param>
		Vector(const Vector& rhs);
		/// <summary>
		/// The move constructor for Vector is currently not implemented and has been deleted.
		/// </summary>
		/// <param name="rhs">The Vector to move data from</param>
		Vector(Vector&& rhs) noexcept;
		/// <summary>
		/// The copy assignment operator for an Vector that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Vector to copy</param>
		/// <returns>A reference to the updated Vector</returns>
		Vector& operator=(const Vector& rhs);
		/// <summary>
		/// The move assignment operator for Vector is currently not implemented and has been deleted.
		/// </summary>
		/// <param name="rhs">The Vector to move data from</param>
		/// <returns></returns>
		Vector& operator=(Vector&& rhs) noexcept;
		/// <summary>
		/// The destructor for Vector deletes each element of the vector to free memory.
		/// </summary>
		~Vector();

		/// <summary>
		/// Bracket Access Operator that returns a reference to the item at the specified index. 
		/// </summary>
		/// <param name="index">The index to retrieve.</param>
		/// <returns>A reference to the item at the specified index.</returns>
		/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the index is out of bounds of the size of the Vector.</exception>
		[[nodiscard]] reference operator[](size_type index);
		/// <summary>
		/// Bracket Access Operator that returns a constant reference to the item at the specified index. 
		/// </summary>
		/// <param name="index">The index to retrieve.</param>
		/// <returns>A constant reference to the item at the specified index.</returns>
		/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the index is out of bounds of the size of the Vector.</exception>
		[[nodiscard]] const_reference operator[](size_type index) const;
		/// <summary>
		/// At Access Operator that returns a reference to the item at the specified index. 
		/// </summary>
		/// <param name="index">The index to retrieve.</param>
		/// <returns>A reference to the item at the specified index.</returns>
		/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the index is out of bounds of the size of the Vector.</exception>
		[[nodiscard]] reference At(size_type index);
		/// <summary>
		/// At Access Operator that returns a constant reference to the item at the specified index. 
		/// </summary>
		/// <param name="index">The index to retrieve.</param>
		/// <returns>A constant reference to the item at the specified index.</returns>
		/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the index is out of bounds of the size of the Vector.</exception>
		[[nodiscard]] const_reference At(size_type index) const;

		/// <summary>
		/// Retrieve the size of the Vector.
		/// </summary>
		/// <returns>A size_type representing the current size of the Vector.</returns>
		[[nodiscard]] size_type Size() const;
		/// <summary>
		/// Retrieve the capacity of the Vector.
		/// </summary>
		/// <returns>A size_type representing the current capacity of the Vector.</returns>
		[[nodiscard]] size_type Capacity() const;
		/// <summary>
		/// Determine if the Vector is currently empty.
		/// </summary>
		/// <returns>A boolean indicating whether this Vector is currently empty or not.</returns>
		[[nodiscard]] bool IsEmpty() const;

		/// <summary>
		/// Increase the capacity of the Vector up to the specified amount. If the specified amount is not greater than the current capacity, no work is done.
		/// </summary>
		/// <param name="capacity">The increased capacity for the Vector.</param>
		void Reserve(size_type capacity);
		/// <summary>
		/// Decrease the capacity of the Vector to match its size. If the capacity is not greater than the size, no work is done.
		/// </summary>
		void ShrinkToFit();
		/// <summary>
		/// Remove all elements from the Vector. Does not reduce the Vector's capacity.
		/// </summary>
		void Clear();

		/// <summary>
		/// Get an Iterator pointing to the start of the Vector.
		/// </summary>
		/// <returns>An Iterator pointing to the first element in the Vector</returns>
		[[nodiscard]] Iterator begin();
		/// <summary>
		/// Get an Iterator pointing past the end of the Vector.
		/// </summary>
		/// <returns>An Iterator pointing to nullptr, representing the element past the last element in the Vector</returns>
		[[nodiscard]] Iterator end();
		/// <summary>
		/// Get a ConstIterator pointing to the start of the Vector.
		/// </summary>
		/// <returns>A ConstIterator pointing to the first element in the Vector</returns>
		[[nodiscard]] ConstIterator begin() const;
		/// <summary>
		/// Get a ConstIterator pointing past the end of the Vector.
		/// </summary>
		/// <returns>A ConstIterator pointing to nullptr, representing the element past the last element in the Vector</returns>
		[[nodiscard]] ConstIterator end() const;
		/// <summary>
		/// Get a ConstIterator pointing to the start of the Vector. Can be used to force retrieval of a ConstIterator from non-const Vectors.
		/// </summary>
		/// <returns>A ConstIterator pointing to the first element in the Vector</returns>
		[[nodiscard]] ConstIterator cbegin() const;
		/// <summary>
		/// Get a ConstIterator pointing past the end of the Vector. Can be used to force retrieval of a ConstIterator from non-const Vectors.
		/// </summary>
		/// <returns>A ConstIterator pointing to nullptr, representing the element past the last element in the Vector</returns>
		[[nodiscard]] ConstIterator cend() const;

		/// <summary>
		/// Gets a reference to the data stored in the first element of the Vector.
		/// </summary>
		/// <returns>A reference to the first item in the Vector</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the vector is empty</exception>
		[[nodiscard]] reference Front();
		/// <summary>
		/// Gets a constant reference to the data stored in the first element of the Vector.
		/// </summary>
		/// <returns>A constant reference to the first item in the Vector</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the vector is empty</exception>
		[[nodiscard]] const_reference Front() const;
		/// <summary>
		/// Gets a reference to the data stored in the last element of the Vector.
		/// </summary>
		/// <returns>A reference to the last item in the Vector</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the vector is empty</exception>
		[[nodiscard]] reference Back();
		/// <summary>
		/// Gets a constant reference to the data stored in the last element of the Vector.
		/// </summary>
		/// <returns>A constant reference to the last item in the Vector</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the vector is empty</exception>
		[[nodiscard]] const_reference Back() const;
		/// <summary>
		/// Look for an item in the Vector and return a ConstIterator to the first instance of it if it's found, or pointing to end() otherwise.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <param name="equalityFunctor">A functor with a () operator that returns whether two objects of the Vector type are equivilant.</param>
		/// <returns>A ConstIterator pointing to the found item</returns>
		template <typename EqualityFunctor = DefaultEquality<T>>
		[[nodiscard]] ConstIterator Find(const_reference data, EqualityFunctor equalityFunctor = EqualityFunctor{}) const;
		/// <summary>
		/// Look for an item in the Vector and return an Iterator to the first instance of it if it's found, or pointing to end() otherwise.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <param name="equalityFunctor">A functor with a () operator that returns whether two objects of the Vector type are equivilant.</param>
		/// <returns>An Iterator pointing to the found item</returns>
		template <typename EqualityFunctor = DefaultEquality<T>>
		[[nodiscard]] Iterator Find(const_reference data, EqualityFunctor equalityFunctor = EqualityFunctor{});
		/// <summary>
		/// Add an item to the back of the Vector. If the Vector does not have enough capacity to add this item, it will first reserve extra space according to the Vector's increment functor strategy.
		/// </summary>
		/// <param name="data">The data to add to the Vector.</param>
		/// <returns></returns>
		Iterator PushBack(const_reference data);
		/// <summary>
		/// Add an item to the back of the Vector using move semantics. If the Vector does not have enough capacity to add this item, it will first reserve extra space according to the Vector's increment functor strategy.
		/// </summary>
		/// <param name="data">The data to add to the Vector.</param>
		/// <returns></returns>
		Iterator PushBack(rvalue_reference data);
		/// <summary>
		/// Look for an item in the Vector and remove the first instance of it if it's found. Does not decrease the Vector's capacity.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <param name="equalityFunctor">A functor with a () operator that returns whether two objects of the Vector type are equivilant.</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		template <typename EqualityFunctor = DefaultEquality<T>>
		bool Remove(const_reference data, EqualityFunctor equalityFunctor = EqualityFunctor{});
		/// <summary>
		/// Remove the element in the Vector pointed to by a given Iterator. Does not decrease the Vector's capacity.
		/// </summary>
		/// <param name="iter">An Iterator pointing at which element to remove</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Vector the Iterator belngs to is not valid or not the same as the Vector being modified</exception>
		bool Remove(const Iterator& iter);
		/// <summary>
		/// Remove all elements in a Vector within a given range. Does not decrease the Vector's capacity.
		/// </summary>
		/// <param name="iterFirst">An Iterator pointing at the first element to remove</param>
		/// <param name="iterLast">An Iterator pointing at the element after the last element to remove</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Vector the Iterator belngs to is not valid or not the same as the Vector being modified</exception>
		bool Remove(const Iterator& iterFirst, const Iterator& iterLast);
		/// <summary>
		/// Remove the last item from the Vector. Does not decrease the Vector's capacity.
		/// </summary>
		void PopBack();

	private:
		/// <summary>
		/// The head of the Vector.
		/// </summary>
		T* _data{ nullptr };
		/// <summary>
		/// The current capacity of the Vector.
		/// </summary>
		size_type _capacity{ 0 };
		/// <summary>
		/// The current size of the Vector.
		/// </summary>
		size_type _size{ 0 };
		/// <summary>
		/// A function that will return how much to increment the capacity of the Vector by when called on a certain current capacity.
		/// </summary>
		std::function<size_type(size_type)> _incrementFunctor;

		/// <summary>
		/// A helper function that performs a deep copy of the given Vector into this.
		/// </summary>
		/// <param name="rhs">The Vector to copy.</param>
		void DeepCopy(const Vector& rhs);
	};
}

#include "Vector.inl"