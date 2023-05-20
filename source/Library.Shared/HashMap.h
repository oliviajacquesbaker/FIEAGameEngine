/// <summary>
/// The declaration of the templated class HashMap, as well as its enveloped classes Iterator and ConstIterator.
/// </summary>
#pragma once

#include <cstddef>
#include <cassert>
#include "gsl/gsl"
#include "Vector.h"
#include "SList.h"
#include "HashMap.h"
#include "DefaultHash.h"
#include "DefaultEquality.h"

namespace FIEAGameEngine {
	/// <summary>
	/// A templated container class for a hashmap. 
	/// HashMaps have Iterators and ConstIterators and move semantics are implemented in assignments, copy constructors, and inserts. 
	/// The load factor can be retrieved but there is no automatic rehashing.
	/// </summary>
	/// <typeparam name="TKey">The type of object used as a key.</typeparam>
	/// <typeparam name="TData">The type of object stored at a key.</typeparam>
	/// <typeparam name="THashFunctor">The functor used to hash a key type.</typeparam>
	template <typename TKey, typename TData, typename THashFunctor = DefaultHash<TKey>>
	class HashMap final {
	public:
		using PairType = std::pair<const TKey, TData>;
		using ChainType = SList<PairType>;

		using key_type = const TKey;
		using mapped_type = TData;
		using value_type = PairType;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using hasher = THashFunctor;
		using reference = value_type&;
		using rvalue_reference = value_type&&;
		using const_reference = const value_type&;
		using iterator = class Iterator;
		using const_iterator = class ConstIterator;
		using EqualityFunctor = std::function<bool(key_type&, key_type&)>;
		
		/// <summary>
		/// Iterator class allows traversal of HashMap objects. Iterators function as Forward Iterators and have all related functionaliy.
		/// Dereferencing an Iterator equal to end or trying to increment an Iterator that is not assigned to an HashMap will result in a runtime error.
		/// </summary>
		class Iterator final {
			friend HashMap;
			friend class ConstIterator;

		public:
			//difference_type, pointer, and iterator_category choices are taken from information on https://en.cppreference.com/w/cpp/iterator/iterator_traits
			//This allows usage of std:: functions requiring iterator types with this custom Iterator, i.e. std::equal
			using size_type = HashMap::size_type;
			using value_type = HashMap::value_type;
			using reference = HashMap::reference;
			using const_reference = HashMap::const_reference;
			using rvalue_reference = HashMap::rvalue_reference;
			using difference_type = std::ptrdiff_t;
			using pointer = value_type*;
			using const_pointer = const value_type*;
			using iterator_category = std::forward_iterator_tag;

			/// <summary>
			/// Default constructor for an Iterator. Using the compiler provided default behavior.
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
			/// <returns>A reference to the PairType the Iterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator's data is equal to end()</exception>
			[[nodiscard]] reference operator*() const;
			/// <summary>
			/// Allow access to the PairType the Iterator points to.
			/// </summary>
			/// <returns>A pointer to the PairType the Iterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator's data is equal to end()</exception>
			[[nodiscard]] pointer operator->() const;
			/// <summary>
			/// Pre-increment an Iterator.
			/// </summary>
			/// <returns>A reference to an Iterator, now pointing to the next item in the HashMap</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator is not associted with an HashMap</exception>
			Iterator& operator++(); 
			/// <summary>
			/// Post-increment an Iterator. Calls the pre-fixed increment as part of it.
			/// </summary>
			/// <param name="">Unused parameter to disambiguate between post and pre increment overloads</param>
			/// <returns>An Iterator pointing to the item in the list that the incremented Iterator used to be pointing to</returns>
			Iterator operator++(int);
			/// <summary>
			/// Test if two Iterators are not equal to each other. Iterators are not equal to each other if they either do not belong to the same HashMap or if they do not point to the same PairType.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterators are not equal to each other</returns>
			[[nodiscard]] bool operator!=(const Iterator& rhs) const;
			/// <summary>
			/// Test if two Iterators are equal to each other. Iterators are equal to each other if they both belong to the same HashMap and point to the same PairType.
			/// </summary>
			/// <param name="rhs">The Iterator being compared to</param>
			/// <returns>A boolean indicating if the Iterators are equal to each other</returns>
			[[nodiscard]] bool operator==(const Iterator& rhs) const;
		private:
			/// <summary>
			/// Private constructor for an Iterator that sets its container and chain information with parameterized input.
			/// </summary>
			/// <param name="container">The HashMap this Iterator belongs to</param>
			/// <param name="chainNum">The chain index this Iterator is pointing into</param>
			/// <param name="chainIter">The iterator for the chain we're inside of that points to the PairType itself</param>
			Iterator(HashMap& container, size_type chainNum, typename SList<PairType>::Iterator chainIter);

			/// <summary>
			/// The chain this Iterator is inside of.
			/// </summary>
			size_type _chainNumber{ 0 }; 
			/// <summary>
			/// The chain (SList) iterator pointing to a specific PairType
			/// </summary>
			typename SList<PairType>::Iterator _chainIterator;
			/// <summary>
			/// The HashMap this Iterator belongs to.
			/// </summary>
			HashMap<TKey, TData, THashFunctor>* _container{ nullptr };
		};

		/// <summary>
		/// ConstIterator class allows traversal of HashMap objects. Functionally almost identical to Iterator class, except that dereferencing a ConstIterator returns a const reference.
		/// Dereferencing a ConstIterator equal to end or trying to increment a ConstIterator that is not assigned to an HashMap will result in a runtime error.
		/// </summary>
		class ConstIterator final {
			friend HashMap;

		public:
			//difference_type, pointer, and iterator_category choices are taken from information on https://en.cppreference.com/w/cpp/iterator/iterator_traits
			//This allows usage of std:: functions requiring iterator types with this custom Iterator, i.e. std::equal
			using size_type = HashMap::size_type;
			using value_type = HashMap::value_type;
			using reference = HashMap::reference;
			using const_reference = HashMap::const_reference;
			using rvalue_reference = HashMap::rvalue_reference;
			using difference_type = std::ptrdiff_t;
			using pointer = const value_type*;
			using iterator_category = std::forward_iterator_tag;

			/// <summary>
			/// Default constructor for a ConstIterator. Using the compiler provided default behavior.
			/// </summary>
			ConstIterator() = default;
			/// <summary>
			/// A type-cast constructor for a ConstIterator that takes in a ConstIterator and copies its information to create a ConstIterator pointing at the same PairType in the same HashMap.
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
			/// The move assignment operator for an Iterator. Using the compiler provided default behavior.
			/// </summary>
			/// <param name="rhs">The ConstIterator to move data from</param>
			/// <returns>A ConstIterator reference to the copy</returns>
			ConstIterator& operator=(ConstIterator&& rhs) noexcept = default;
			/// <summary>
			/// The destructor for a ConstIterator. Using the compiler provided default behavior.
			/// </summary>
			~ConstIterator() = default;

			/// <summary>
			/// Dereference a CostIterator.
			/// </summary>
			/// <returns>A reference to the PairType the ConstIterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator's data is equal to end() / if the data is equal to nullptr</exception>
			[[nodiscard]] const_reference operator*() const;
			/// <summary>
			/// Allow access to the PairType the ConstIterator points to.
			/// </summary>
			/// <returns>A pointer to the PairType the ConstIterator points to</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the Iterator's data is equal to end()</exception>
			[[nodiscard]] pointer operator->() const;
			/// <summary>
			/// Pre-increment a ConstIterator.
			/// </summary>
			/// <returns>A reference to a ConstIterator, now pointing to the next item in the HashMap</returns>
			/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the ConstIterator is not associted with an HashMap</exception>
			ConstIterator& operator++();
			/// <summary>
			/// Post-increment a ConstIterator. Calls the pre-fixed increment as part of it.
			/// </summary>
			/// <param name="">Unused parameter to disambiguate between post and pre increment overloads</param>
			/// <returns>A ConstIterator pointing to the item in the list that the incremented ConstIterator used to be pointing to</returns>
			ConstIterator operator++(int);
			/// <summary>
			/// Test if two ConstIterators are not equal to each other. ConstIterators are not equal to each other if they either do not belong to the same HashMap or if they do not point to the same PairType.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterators are not equal to each other</returns>
			[[nodiscard]] bool operator!=(const ConstIterator& rhs) const;
			/// <summary>
			/// Test if two ConstIterators are equal to each other. ConstIterators are equal to each other if they both belong to the same HashMap and point to the same PairType.
			/// </summary>
			/// <param name="rhs">The ConstIterator being compared to</param>
			/// <returns>A boolean indicating if the ConstIterators are equal to each other</returns>
			[[nodiscard]] bool operator==(const ConstIterator& rhs) const;
		private:
			/// <summary>
			/// Private constructor for a ConstIterator that sets its container, chain, and chain iterator information with parameterized input.
			/// </summary>
			/// <param name="container">The HashMap this ConstIterator belongs to</param>
			/// <param name="chainNum">The chain this ConstIterator is inside of</param>
			/// <param name="chainIter">The chain iterator this ConstIterator is using to point at a PairType</param>
			ConstIterator(const HashMap& container, size_type chainNum, typename SList<PairType>::ConstIterator chainIter);

			/// <summary>
			/// The chain number this ConstIterator is inside of
			/// </summary>
			size_type _chainNumber{ 0 };
			/// <summary>
			/// The chain iterator this ConstIterator is using to point to a specific PairType
			/// </summary>
			typename SList<PairType>::ConstIterator _chainIterator;
			/// <summary>
			/// The HashMap this ConstIterator belongs to.
			/// </summary>
			const HashMap<TKey, TData, THashFunctor>* _container{ nullptr };
		};

		/// <summary>
		/// The main constructor for a HashMap.
		/// </summary>
		/// <param name="chainCount">The number of chains this HashMap should have, defaulted to 13. Must be greater than 1.</param>
		/// <param name="equalityFunctor">A method used to determine relevant equality between two PairTypes; i.e. key equality</param>
		explicit HashMap(size_type chainCount = 13, EqualityFunctor equalityFunctor = DefaultEquality<key_type>{});
		/// <summary>
		/// An Initializer list constructor for HashMap.
		/// </summary>
		/// <param name="list">A set of items to add to the HashMap as it's being constructed</param>
		/// <param name="chainCount">The number of chains this HashMap should have, defaulted to 13. Must be greater than 1.</param>
		/// <param name="equalityFunctor">A method used to determine relevant equality between two PairTypes; i.e. key equality</param>
		HashMap(std::initializer_list<PairType> list, size_type chainCount = 13, EqualityFunctor equalityFunctor = DefaultEquality<key_type>{});
		/// <summary>
		/// A copy constructor for a HashMap. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="other">The HashMap to copy</param>
		HashMap(const HashMap& rhs) = default;
		/// <summary>
		/// The move constructor for a HashMap. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="rhs">The HashMap to move data from</param>
		HashMap(HashMap&& rhs) noexcept = default;
		/// <summary>
		/// The copy assignment operator for a HashMap. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="rhs">The HashMap to copy</param>
		/// <returns>A reference to the updated HashMap</returns>
		HashMap& operator=(const HashMap& rhs) = default;
		/// <summary>
		/// The move assignment operator for a HashMap. Using the compiler provided default implementation.
		/// </summary>
		/// <param name="rhs">The HashMap to move data from</param>
		/// <returns>A reference to the updated HashMap</returns>
		HashMap& operator=(HashMap&& rhs) noexcept = default;
		/// <summary>
		/// The destructor for a HashMap. Using the compiler provided default implementation.
		/// </summary>
		~HashMap() = default;

		/// <summary>
		/// Get the current population.
		/// </summary>
		/// <returns>The current population of the HashMap</returns>
		[[nodiscard]] size_type Size() const;
		/// <summary>
		/// Get the current load factor, the proportion of non empty chains / total chains.
		/// </summary>
		/// <returns>The current load factor of the HashMap</returns>
		[[nodiscard]] double GetLoadFactor() const;
		/// <summary>
		/// Check if the HashMap contains a given key.
		/// </summary>
		/// <param name="key">The key to look for</param>
		/// <returns>A boolean determining if the HashMap contains the given key</returns>
		[[nodiscard]] bool ContainsKey(const TKey& key) const;
		/// <summary>
		/// Check if the HashMap contains a given key, and return an Iterator pointing to it, or end() if it does not exist in the HashMap.
		/// </summary>
		/// <param name="key">The key to look for</param>
		/// <returns>An Iterator pointing to the PairType with the given key, or end() if none exists.</returns>
		[[nodiscard]] Iterator Find(const TKey& key);
		/// <summary>
		/// Check if the HashMap contains a given key, and return a ConstIterator pointing to it, or end() if it does not exist in the HashMap.
		/// </summary>
		/// <param name="key">The key to look for</param>
		/// <returns>A ConstIterator pointing to the PairType with the given key, or end() if none exists.</returns>
		[[nodiscard]] ConstIterator Find(const TKey& key) const;
		/// <summary>
		/// Check if the HashMap contains a given key, return the data stored there.
		/// </summary>
		/// <param name="key">The key to look for</param>
		/// <returns>A reference to the data stored at key.</returns>
		/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the key does not exist in the HashMap</exception>
		[[nodiscard]] TData& At(const TKey& key);
		/// <summary>
		/// Check if the HashMap contains a given key, return the data stored there.
		/// </summary>
		/// <param name="key">The key to look for</param>
		/// <returns>A constant reference to the data stored at key.</returns>
		/// <exception cref = "std::runtime_error">Thrown if USE_EXCEPTIONS is defined and the key does not exist in the HashMap</exception>
		[[nodiscard]] const TData& At(const TKey& key) const;
		/// <summary>
		/// Returns a reference to the data stored at the given key. If the given key does not exist in the HashMap, it is inserted with a default constructed TData.
		/// </summary>
		/// <param name="rhs">The key to lookup</param>
		/// <returns>A reference to the data stored at that key</returns>
		TData& operator[](const TKey& key);

		/// <summary>
		/// Insert a given PairType into the HashMap, if the key does not already exist.
		/// </summary>
		/// <param name="entry">The PairType to insert</param>
		/// <returns>A pair of an Iterator pointing to the inserted object (or the preexisting object with the same key) and a bool indicating if something new was inserted</returns>
		std::pair<Iterator, bool> Insert(const PairType& entry);
		/// <summary>
		/// Insert a given PairType into the HashMap, if the key does not already exist, using move semantics
		/// </summary>
		/// <param name="entry">The PairType to insert</param>
		/// <returns>A pair of an Iterator pointing to the inserted object (or the preexisting object with the same key) and a bool indicating if something new was inserted</returns>
		std::pair<Iterator, bool> Insert(PairType&& entry);
		/// <summary>
		/// Remove a PairType with the given key if it exists in the HashMap.
		/// </summary>
		/// <param name="key">The key to look for removal</param>
		void Remove(const TKey& key);
		/// <summary>
		/// Remove a PairType with the given iterator if it isn't equal to end().
		/// </summary>
		/// <param name="key">The iterator to look for removal</param>
		void Remove(const Iterator& iter);
		/// <summary>
		/// Empty the HashMap.
		/// </summary>
		void Clear();

		/// <summary>
		/// Get an Iterator pointing to the start of the HashMap.
		/// </summary>
		/// <returns>An Iterator pointing to the first element in the HashMap</returns>
		[[nodiscard]] Iterator begin();
		/// <summary>
		/// Get an Iterator pointing past the end of the HashMap.
		/// </summary>
		/// <returns>An Iterator representing the element past the last element in the HashMap</returns>
		[[nodiscard]] Iterator end();
		/// <summary>
		/// Get a ConstIterator pointing to the start of the HashMap.
		/// </summary>
		/// <returns>A ConstIterator pointing to the first element in the HashMap</returns>
		[[nodiscard]] ConstIterator begin() const;
		/// <summary>
		/// Get a ConstIterator pointing past the end of the HashMap.
		/// </summary>
		/// <returns>A ConstIterator representing the element past the last element in the HashMap</returns>
		[[nodiscard]] ConstIterator end() const;
		/// <summary>
		/// Get a ConstIterator pointing to the start of the HashMap. Can be used to force retrieval of a ConstIterator from non-const HashMaps.
		/// </summary>
		/// <returns>A ConstIterator pointing to the first element in the HashMap</returns>
		[[nodiscard]] ConstIterator cbegin() const;
		/// <summary>
		/// Get a ConstIterator pointing past the end of the HashMap. Can be used to force retrieval of a ConstIterator from non-const HashMaps.
		/// </summary>
		/// <returns>A ConstIterator representing the element past the last element in the HashMap</returns>
		[[nodiscard]] ConstIterator cend() const;

	private:
		/// <summary>
		/// The HashMap itself.
		/// </summary>
		Vector<ChainType> _hashMap;
		/// <summary>
		/// The current population of the HashMap.
		/// </summary>
		size_type _population{ 0 };
		/// <summary>
		/// The current number of chains with at least one element in them.
		/// </summary>
		size_type _populatedChains{ 0 };
		/// <summary>
		/// A function indicating how to find eqaulity between two keys.
		/// </summary>
		EqualityFunctor _equalityFunctor;

		/// <summary>
		/// Helper function to get the hashed and modulo'd index of a key.
		/// </summary>
		/// <param name="key">The key to hash</param>
		/// <returns>The chain index for the key</returns>
		[[nodiscard]] size_type GetHashedIndex(const TKey& key) const;
		/// <summary>
		/// Helper function that performs a find call wih a precomputed chain Index.
		/// </summary>
		/// <param name="key">The key to look for</param>
		/// <param name="chainIndex">The precomputed chain index</param>
		/// <returns>An Iterator pointing to the found PairType</returns>
		[[nodiscard]] Iterator Find(const TKey& key, size_type chainIndex);
		/// <summary>
		/// Helper function that performs a find call wih a precomputed chain Index.
		/// </summary>
		/// <param name="key">The key to look for</param>
		/// <param name="chainIndex">The precomputed chain index</param>
		/// <returns>A ConstIterator pointing to the found PairType</returns>
		[[nodiscard]] ConstIterator Find(const TKey& key, size_type chainIndex) const;
		/// <summary>
		/// Helper function that performs an insert call wih a precomputed chain Index.
		/// </summary>
		/// <param name="key">The PairType to insert</param>
		/// <param name="chainIndex">The precomputed chain index</param>
		/// <returns>An Iterator pointing to the inserted/found PairType and a boolean indicating if something was actually inserted</returns>
		std::pair<Iterator, bool> Insert(const PairType& entry, size_type chainIndex);
		/// <summary>
		/// Helper function that performs an insert call wih a precomputed chain Index.
		/// </summary>
		/// <param name="key">The PairType to insert</param>
		/// <param name="chainIndex">The precomputed chain index</param>
		/// <returns>An Iterator pointing to the inserted/found PairType and a boolean indicating if something was actually inserted</returns>
		std::pair<Iterator, bool> Insert(PairType&& entry, size_type chainIndex);
		/// <summary>
		/// Helper function that creates the chains (SLists) for the HashMap.
		/// </summary>
		/// <param name="key">The number of chains to create.</param>
		void CreateEmptyChains(size_type chainNumber);
	};
}

#include "HashMap.inl"