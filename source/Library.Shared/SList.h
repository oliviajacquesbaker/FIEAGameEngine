/// <summary>
/// The declaration of the templated class SList, as well as its enveloped classes Iterator, ConstIterator, and its struct Node.
/// </summary>

#pragma once

#include <cstddef>
#include <stdexcept>
#include <initializer_list>
#include "DefaultEquality.h"

namespace FIEAGameEngine {
	/// <summary>
	/// A templated container class for a singly linked list. Currently, Move Semantics are not implemented. What can be done is: constuct SLists, use copy semantics 
	/// with SLists, traverse SLists with iterators, add and remove elements from the front and back of SLists, look at the front and back
	/// elements of SLists, and clear all elements from an SList. Removing an element from the front or back of an SList does not return the removed element.
	/// Additionally, trying to look at or remove the front or back elements of an empty list will result in a runtime error.
	/// </summary>
	/// <typeparam name="T"></typeparam>
	template <typename T>
	class SList final {
	private:
		/// <summary>
		/// A Node holds information on a piece of type T data and that data's connection to the SList it's a part of.
		/// </summary>
		struct Node final {
			/// <summary>
			/// Data is the actual information stored in this node.
			/// </summary>
			T Data;
			/// <summary>
			/// Next is a pointer to the Node that comes next in the list this Node is a part of.
			/// </summary>
			Node* Next{ nullptr };

			/// <summary>
			/// Node's constructor, assigning the Node information and a relative location in an SList.
			/// </summary>
			/// <param name="data">The information to be stored in this Node</param>
			/// <param name="next">The Node that comes next in the list that this Node is a part of, defaulted to nullptr</param>
			template<typename... Args>
			Node(Node* next, Args&&... args);

			Node(const Node&) = delete;
			Node(Node&&) noexcept = delete;
			Node& operator=(const Node&) = delete;
			Node& operator=(Node&&) noexcept = delete;
			~Node() = default;
		};

	public:
		using size_type = std::size_t;
		using value_type = T;
		using reference = value_type&;
		using const_reference = const value_type&;
		using rvalue_reference = T&&;

		/// <summary>
		/// Iterator class allows public traversal of SList objects. Iterators can be dereferenced, incremented, and compared for equality.
		/// Dereferencing an Iterator with a Node equal to nullptr or trying to increment an Iterator that is not assigned to an SList will result in a runtime error.
		/// </summary>
		class Iterator final {
			friend SList;
			friend class ConstIterator;

		public:
			//difference_type, pointer, and iterator_category choices are taken from information on https://en.cppreference.com/w/cpp/iterator/iterator_traits
			//This allows usage of std:: functions requiring iterator types with this custom Iterator, i.e. std::equal
			using size_type = SList::size_type;
			using value_type = SList::value_type;
			using reference = SList::reference;
			using const_reference = SList::const_reference;
			using rvalue_reference = SList::rvalue_reference;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using iterator_category = std::forward_iterator_tag;

			/// <summary>
			/// Default constructor for an Iterator. Its Node* and SList* container will both be set to nullptr.
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
			/// <returns>A reference to the item contained by the Node the Iterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator's data is equal to end() / if the data is equal to nullptr</exception>
			reference operator*() const;
			/// <summary>
			/// Pre-increment an Iterator.
			/// </summary>
			/// <returns>A reference to an Iterator, now pointing to the next item in the list</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator is not associted with an SList</exception>
			Iterator& operator++();
			/// <summary>
			/// Post-increment an Iterator. Calls the pre-fixed increment as part of it.
			/// </summary>
			/// <param name="">Unused parameter to disambiguate between post and pre increment overloads</param>
			/// <returns>An Iterator pointing to the item in the list that the incremented Iterator used to be pointing to</returns>
			Iterator operator++(int);
			/// <summary>
			/// Test if two Iterators are not equal to each other. Iterators are not equal to each other if they either do not belong to the same SList or if they do not point to the same Node.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterators are not equal to each other</returns>
			bool operator!=(const Iterator& rhs) const;
			/// <summary>
			/// Test if two Iterators are equal to each other. Iterators are equal to each other if they both belong to the same SList and point to the same Node.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterators are equal to each other</returns>
			bool operator==(const Iterator& rhs) const;
		private:
			/// <summary>
			/// Private constructor for an Iterator that sets its container and node information with parameterized input.
			/// </summary>
			/// <param name="container">The SList this Iterator belongs to</param>
			/// <param name="node">The Node this Iterator points at, defaulted to nullptr</param>
			Iterator(const SList& container, Node* node = nullptr);

			/// <summary>
			/// The Node this Iterator points to.
			/// </summary>
			Node* _node{ nullptr };
			/// <summary>
			/// The SList this Iterator belongs to.
			/// </summary>
			const SList* _container{ nullptr };
		};

		/// <summary>
		/// ConstIterator class allows public traversal of SList objects. Functionally almost identical to Iterator class, except that dereferencing a ConstIterator returns a const T reference.
		/// Dereferencing a ConstIterator with a Node equal to nullptr or trying to increment a ConstIterator that is not assigned to an SList will result in a runtime error.
		/// </summary>
		class ConstIterator final {
			friend SList;
		public:
			//difference_type, pointer, and iterator_category choices are taken from information on https://en.cppreference.com/w/cpp/iterator/iterator_traits
			//This allows usage of std:: functions requiring iterator types with this custom ConstIterator, i.e. std::equal
			using size_type = SList::size_type;
			using value_type = SList::value_type;
			using reference = SList::reference;
			using const_reference = SList::const_reference;
			using rvalue_reference = SList::rvalue_reference;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using iterator_category = std::forward_iterator_tag;

			/// <summary>
			/// Default constructor for a ConstIterator. Its Node* and SList* container will both be set to nullptr.
			/// </summary>
			ConstIterator() = default;
			/// <summary>
			/// A type-cast constructor for a ConstIterator that takes in a ConstIterator and copies its information to create a ConstIterator pointing at the same Node in the same SList.
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
			/// The copy assignment operator for a ConstIterator. Using the compiler provided default behavior.
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
			/// <returns>A constant reference to the item contained by the Node the ConstIterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the ConstIterator's data is equal to end()</exception>
			const_reference operator*() const;
			/// <summary>
			/// Pre-increment a ConstIterator.
			/// </summary>
			/// <returns>A reference to a ConstIterator, now pointing to the next item in the list</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the ConstIterator is not associted with an SList</exception>
			ConstIterator& operator++();
			/// <summary>
			/// Post-increment a ConstIterator. Calls the pre-fixed increment as part of it.
			/// </summary>
			/// <param name="">Unused param to disambiguate between post and pre increment overloads</param>
			/// <returns>a ConstIterator pointing to the item in the list that the incremented ConstIterator used to be pointing to</returns>
			ConstIterator operator++(int);
			/// <summary>
			/// Test if two ConstIterators are not equal to each other. ConstIterators are not equal to each other if they either do not belong to the same SList or if they do not point to the same Node.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterators are not equal to each other</returns>
			bool operator!=(const ConstIterator& rhs) const;
			/// <summary>
			/// Test if two ConstIterators are equal to each other. ConstIterators are equal to each other if they both belong to the same SList and point to the same Node.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterators are equal to each other</returns>
			bool operator==(const ConstIterator& rhs) const;
		private:
			/// <summary>
			/// Private constructor for a ConstIterator that sets its container and node information with parameterized input.
			/// </summary>
			/// <param name="container">The SList this ConstIterator belongs to</param>
			/// <param name="node">The Node this ConstIterator points at, defaulted to nullptr</param>
			ConstIterator(const SList& container, Node* node = nullptr);

			/// <summary>
			/// The Node this ConstIterator points to.
			/// </summary>
			const Node* _node{ nullptr };
			/// <summary>
			/// The SList this ConstIterator belongs to.
			/// </summary>
			const SList* _container{ nullptr };
		};

		/// <summary>
		/// Default constructor for an SList, using the compiler provided default behavior. Its front and back pointers will both be set to nullptr, and its size to zero.
		/// </summary>
		SList() = default;
		/// <summary>
		/// An Initializer list constructor for SList. The items will be pushed back to the constructor SList in the same order as given into this function.
		/// </summary>
		/// <param name="list">A set of items to add to an SList as it's being constructed</param>
		SList(std::initializer_list<value_type> list);
		/// <summary>
		/// A copy constructor for an SList that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The SList to copy</param>
		SList(const SList& rhs);
		/// <summary>
		/// The move constructor for SList is currently not implemented and has been deleted.
		/// </summary>
		/// <param name="rhs">The SList to move data from</param>
		SList(SList&& rhs) noexcept;
		/// <summary>
		/// The copy assignment operator for an Slist that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The SList to copy</param>
		/// <returns>A reference to the updated SList</returns>
		SList& operator=(const SList& rhs);
		/// <summary>
		/// The move assignment operator for SList is currently not implemented and has been deleted.
		/// </summary>
		/// <param name="rhs">The Slist to move data from</param>
		/// <returns></returns>
		SList& operator=(SList&& rhs) noexcept;
		/// <summary>
		/// The destructor for SList deletes each element of the list to free memory.
		/// </summary>
		~SList();

		/// <summary>
		/// Gets a reference to the data stored in the first element of the SList.
		/// </summary>
		/// <returns>A reference to the first item in the SList</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the list is empty</exception>
		[[nodiscard]] reference Front();
		/// <summary>
		/// Gets a constant reference to the data stored in the first element of the SList.
		/// </summary>
		/// <returns>A constant reference to the first item in the SList</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the list is empty</exception>
		[[nodiscard]] const_reference Front() const;
		/// <summary>
		/// Gets a reference to the data stored in the last element of the SList.
		/// </summary>
		/// <returns>A reference to the last item in the SList</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the list is empty</exception>
		[[nodiscard]] reference Back();
		/// <summary>
		/// Gets a constant reference to the data stored in the last element of the SList.
		/// </summary>
		/// <returns>A constant reference to the last item in the SList</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the list is empty</exception>
		[[nodiscard]] const_reference Back() const;

		/// <summary>
		/// Gets the current size of the SList.
		/// </summary>
		/// <returns>A size_t value of the current size of the SList</returns>
		[[nodiscard]] size_type Size() const;
		/// <summary>
		/// Determines if the SList is currently empty.
		/// </summary>
		/// <returns>A boolean indicating if the SList is currently empty</returns>
		[[nodiscard]] bool IsEmpty() const;

		/// <summary>
		/// Add a new element to the beginning of the SList.
		/// </summary>
		/// <param name="data">The item to be added to the SList</param>
		/// <returns>An Iterator pointing to the item that was just added</returns>
		Iterator PushFront(const_reference data);
		/// <summary>
		/// Add a new element to the beginning of the SList.
		/// </summary>
		/// <param name="data">The item to be added to the SList</param>
		/// <returns>An Iterator pointing to the item that was just added</returns>
		Iterator PushFront(rvalue_reference data);
		/// <summary>
		/// Add a new element to the end of the SList.
		/// </summary>
		/// <param name="data">The item to be added to the SList</param>
		/// <returns>An Iterator pointing to the item that was just added</returns>
		Iterator PushBack(const_reference data);
		/// <summary>
		/// Add a new element to the end of the SList.
		/// </summary>
		/// <param name="data">The item to be added to the SList</param>
		/// <returns>An Iterator pointing to the item that was just added</returns>
		Iterator PushBack(rvalue_reference data);
		/// <summary>
		/// Remove the first element in the SList.
		/// </summary>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the list is empty</exception>
		void PopFront();
		/// <summary>
		/// Remove the last element in the SList.
		/// </summary>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the list is empty</exception>
		void PopBack();
		/// <summary>
		/// Remove all elements in the SList and return it to an empty state.
		/// </summary>
		void Clear();

		/// <summary>
		/// Get an Iterator pointing to the start of the SList.
		/// </summary>
		/// <returns>An Iterator pointing to the first element in the SList</returns>
		[[nodiscard]] Iterator begin();
		/// <summary>
		/// Get an Iterator pointing past the end of the SList.
		/// </summary>
		/// <returns>An Iterator pointing to nullptr, representing the element past the last element in the SList</returns>
		[[nodiscard]] Iterator end();
		/// <summary>
		/// Get a ConstIterator pointing to the start of the SList.
		/// </summary>
		/// <returns>A ConstIterator pointing to the first element in the SList</returns>
		[[nodiscard]] ConstIterator begin() const;
		/// <summary>
		/// Get a ConstIterator pointing past the end of the SList.
		/// </summary>
		/// <returns>A ConstIterator pointing to nullptr, representing the element past the last element in the SList</returns>
		[[nodiscard]] ConstIterator end() const;
		/// <summary>
		/// Get a ConstIterator pointing to the start of the SList. Can be used to force retrieval of a ConstIterator from non-const SLists.
		/// </summary>
		/// <returns>A ConstIterator pointing to the first element in the SList</returns>
		[[nodiscard]] ConstIterator cbegin() const; 
		/// <summary>
		/// Get a ConstIterator pointing past the end of the SList. Can be used to force retrieval of a ConstIterator from non-const SLists.
		/// </summary>
		/// <returns>A ConstIterator pointing to nullptr, representing the element past the last element in the SList</returns>
		[[nodiscard]] ConstIterator cend() const;

		/// <summary>
		/// Add a new element to the SList directly after the element pointed to by the given Iterator.
		/// </summary>
		/// <param name="data">The new item to add to the SList</param>
		/// <param name="iter">The Iterator pointing to the Node that will be the predecessor of the new item in the SList</param>
		/// <returns>An Iterator pointing to the item just inserted</returns>
		/// <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the SList the Iterator belngs to is not valid or not the same as the SList being modified</exception>
		Iterator InsertAfter(const_reference data, const Iterator& iter);
		/// <summary>
		/// Look for an item in the SList and return a ConstIterator to the first instance of it if it's found, or pointing to end() otherwise.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A ConstIterator pointing to the found item</returns>
		template <typename EqualityFunctor = DefaultEquality<T>>
		ConstIterator Find(const_reference data, EqualityFunctor equalityFunctor = EqualityFunctor{}) const;
		/// <summary>
		/// Look for an item in the SList and return an Iterator to the first instance of it if it's found, or pointing to end() otherwise.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>An Iterator pointing to the found item</returns>
		template <typename EqualityFunctor = DefaultEquality<T>>
		Iterator Find(const_reference data, EqualityFunctor equalityFunctor = EqualityFunctor{});
		/// <summary>
		/// Look for an item in the SList and remove the first instance of it if it's found.
		/// </summary>
		/// <param name="data">The item to look for</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		template <typename EqualityFunctor = DefaultEquality<T>>
		bool Remove(const_reference data, EqualityFunctor equalityFunctor = EqualityFunctor{});
		/// <summary>
		/// Remove the element in the SList pointed to by a given Iterator.
		/// </summary>
		/// <param name="data">An Iterator pointing at which element to remove</param>
		/// <returns>A boolean indicating whether a successful remove occured</returns>
		///  <exception cref="std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the SList the Iterator belngs to is not valid or not the same as the SList being modified</exception>
		bool Remove(const Iterator& iter);

	private:
		/// <summary>
		/// The first item in the SList.
		/// </summary>
		Node* _front{ nullptr };
		/// <summary>
		/// The last item in the SList.
		/// </summary>
		Node* _back{ nullptr };
		/// <summary>
		/// The current size of the SList.
		/// </summary>
		size_type _size{ 0 };

		/// <summary>
		/// A helper function that preforms a deep copy of the given SList into this.
		/// </summary>
		/// <param name="rhs">The SList to copy</param>
		void DeepCopy(const SList& rhs);
	};
}

#include "SList.inl"