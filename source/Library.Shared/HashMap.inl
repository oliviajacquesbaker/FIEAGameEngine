/// <summary>
/// The fully specified implementation of the tempated class HashMap and its enveloped classes Iterator and ConstIterator.
/// </summary>

#pragma once

#include "HashMap.h"

namespace FIEAGameEngine {
#pragma region HashMap
	template <typename TKey, typename TData, typename THashFunctor>
	HashMap <TKey, TData, THashFunctor>::HashMap(size_type chainCount, EqualityFunctor equalityFunctor) :
		_hashMap{ Vector<ChainType>(chainCount) }, _equalityFunctor{ equalityFunctor }
	{
		CreateEmptyChains(chainCount);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	HashMap <TKey, TData, THashFunctor>::HashMap(std::initializer_list<PairType> list, size_type chainCount, EqualityFunctor equalityFunctor) :
		_hashMap{Vector<ChainType>(chainCount)}, _equalityFunctor{ equalityFunctor }
	{
		CreateEmptyChains(chainCount);
		for (const_reference value : list) {
			Insert(value);
		}
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::size_type HashMap<TKey, TData, THashFunctor>::Size() const {
		return _population;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline double HashMap<TKey, TData, THashFunctor>::GetLoadFactor() const {
		return _populatedChains / gsl::narrow_cast<double>(_hashMap.Size());
	}
	
	template<typename TKey, typename TData, typename THashFunctor>
	inline bool HashMap<TKey, TData, THashFunctor>::ContainsKey(const TKey& key) const {
		return (Find(key) != end());
	}
	
	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::Iterator HashMap<TKey, TData, THashFunctor>::Find(const TKey& key) {
		size_type chainIndex = GetHashedIndex(key);
		return Find(key, chainIndex);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	typename HashMap<TKey, TData, THashFunctor>::Iterator HashMap<TKey, TData, THashFunctor>::Find(const TKey& key, size_type chainIndex) {
		Iterator foundIter = end();

		for (typename SList<PairType>::Iterator chainIter = _hashMap[chainIndex].begin(); chainIter != _hashMap[chainIndex].end(); ++chainIter) {
			if(_equalityFunctor((*chainIter).first, key)) {
				foundIter = Iterator(*this, chainIndex, chainIter);
				break;
			}
		}
		return foundIter;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::ConstIterator HashMap<TKey, TData, THashFunctor>::Find(const TKey& key) const {
		size_type chainIndex = GetHashedIndex(key);
		return Find(key, chainIndex);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	typename HashMap<TKey, TData, THashFunctor>::ConstIterator HashMap<TKey, TData, THashFunctor>::Find(const TKey& key, size_type chainIndex) const {
		ConstIterator foundIter = end();

		for (typename SList<PairType>::ConstIterator chainIter = _hashMap[chainIndex].begin(); chainIter != _hashMap[chainIndex].end(); ++chainIter) {
			if (_equalityFunctor((*chainIter).first, key)) {
				foundIter = ConstIterator(*this, chainIndex, chainIter);
				break;
			}
		}
		return foundIter;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline TData& HashMap<TKey, TData, THashFunctor>::At(const TKey& key) {
		Iterator foundIter = Find(key);

#ifdef USE_EXCEPTIONS
		if (foundIter == end()) throw std::runtime_error("Out of bounds - key does not exist in container.");
#endif // USE_EXCEPTIONS
		return foundIter->second;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline const TData& HashMap<TKey, TData, THashFunctor>::At(const TKey& key) const {
		ConstIterator foundIter = Find(key);

#ifdef USE_EXCEPTIONS
		if (foundIter == cend()) throw std::runtime_error("Out of bounds - key does not exist in container.");
#endif // USE_EXCEPTIONS

		return foundIter->second;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	TData& HashMap<TKey, TData, THashFunctor>::operator[](const TKey& key) {
		size_type chainIndex = GetHashedIndex(key);
		auto[foundIter, inserted] = Insert(PairType(key, TData()), chainIndex);
		return foundIter->second;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline std::pair<typename HashMap<TKey, TData, THashFunctor>::Iterator, bool> HashMap<TKey, TData, THashFunctor>::Insert(const PairType& entry) {
		size_type chainIndex = GetHashedIndex(entry.first);
		return Insert(entry, chainIndex);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline std::pair<typename HashMap<TKey, TData, THashFunctor>::Iterator, bool> HashMap<TKey, TData, THashFunctor>::Insert(PairType&& entry) {
		size_type chainIndex = GetHashedIndex(entry.first);
		return Insert(std::forward<value_type>(entry), chainIndex);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	std::pair<typename HashMap<TKey, TData, THashFunctor>::Iterator, bool> HashMap<TKey, TData, THashFunctor>::Insert(const PairType& entry, size_type chainIndex) {
		HashMap<TKey, TData, THashFunctor>::Iterator iter = Find(entry.first, chainIndex);
		bool inserted = (iter == end());

		if (inserted) {
			if (_hashMap[chainIndex].IsEmpty()) ++_populatedChains;
			++_population;

			typename SList<PairType>::Iterator chainIter = _hashMap[chainIndex].PushBack(entry);
			iter = Iterator(*this, chainIndex, chainIter);
		}

		return std::pair<Iterator, bool>(iter, inserted);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	std::pair<typename HashMap<TKey, TData, THashFunctor>::Iterator, bool> HashMap<TKey, TData, THashFunctor>::Insert(PairType&& entry, size_type chainIndex) {
		HashMap<TKey, TData, THashFunctor>::Iterator iter = Find(entry.first, chainIndex);
		bool inserted = (iter == end());

		if (inserted) {
			if (_hashMap[chainIndex].IsEmpty()) ++_populatedChains;
			++_population;

			typename SList<PairType>::Iterator chainIter = _hashMap[chainIndex].PushBack(std::forward<value_type>(entry));
			iter = Iterator(*this, chainIndex, chainIter);
		}

		return std::pair<Iterator, bool>(iter, inserted);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	void HashMap<TKey, TData, THashFunctor>::Remove(const TKey& key) {
		size_type chainIndex = GetHashedIndex(key);
		Remove(Find(key, chainIndex));
	}

	template<typename TKey, typename TData, typename THashFunctor>
	void HashMap<TKey, TData, THashFunctor>::Remove(const Iterator& iter) {
		if (iter != end()) {
			_hashMap[iter._chainNumber].Remove(iter._chainIterator);
			if (_hashMap[iter._chainNumber].IsEmpty()) --_populatedChains;
			--_population;
		}
	}

	template<typename TKey, typename TData, typename THashFunctor>
	void HashMap<TKey, TData, THashFunctor>::Clear() {
		for (auto& chain : _hashMap) { 
			chain.Clear(); 
		} 
		_population = _populatedChains = 0;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	typename HashMap<TKey, TData, THashFunctor>::Iterator HashMap<TKey, TData, THashFunctor>::begin() {
		if (_populatedChains == 0) return end();

		Iterator beginIter( *this, size_type(0), _hashMap[size_type(0)].begin() );
		if (beginIter._chainIterator == _hashMap[beginIter._chainNumber].end()) { 
			++beginIter;
		}

		return beginIter;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::Iterator HashMap<TKey, TData, THashFunctor>::end() {
		return Iterator(*this, _hashMap.Size(), typename SList<PairType>::Iterator());
	}

	template<typename TKey, typename TData, typename THashFunctor>
	typename HashMap<TKey, TData, THashFunctor>::ConstIterator HashMap<TKey, TData, THashFunctor>::begin() const {
		return cbegin();
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::ConstIterator HashMap<TKey, TData, THashFunctor>::end() const {
		return cend();
	}

	template<typename TKey, typename TData, typename THashFunctor>
	typename HashMap<TKey, TData, THashFunctor>::ConstIterator HashMap<TKey, TData, THashFunctor>::cbegin() const {
		if (_populatedChains == 0) return end();

		ConstIterator beginIter(*this, size_type(0), _hashMap[size_type(0)].cbegin());
		if (beginIter._chainIterator == _hashMap[beginIter._chainNumber].cend()) {
			++beginIter;
		}

		return beginIter;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::ConstIterator HashMap<TKey, TData, THashFunctor>::cend() const {
		return ConstIterator(*this, _hashMap.Size(), typename SList<PairType>::ConstIterator());
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline void HashMap <TKey, TData, THashFunctor>::CreateEmptyChains(size_type chainNumber) {
		assert(chainNumber > 1);
		for (size_type i = size_type(0); i < chainNumber; ++i) {
			_hashMap.PushBack(ChainType());
		}
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::size_type HashMap<TKey, TData, THashFunctor>::GetHashedIndex(const TKey& key) const {
		THashFunctor hashFunctor;
		return hashFunctor(key) % _hashMap.Size();
	}
#pragma endregion HashMap

#pragma region Iterator
	template<typename TKey, typename TData, typename THashFunctor>
	HashMap<TKey, TData, THashFunctor>::Iterator::Iterator(HashMap<TKey, TData, THashFunctor>& container, size_type chainNum, typename SList<PairType>::Iterator chainIter) :
		_container{ &container }, _chainNumber{ chainNum }, _chainIterator{ chainIter }
	{
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::reference HashMap <TKey, TData, THashFunctor>::Iterator::operator*() const {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
		if (_chainNumber >= _container->_hashMap.Size()) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return *_chainIterator;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::Iterator::pointer HashMap <TKey, TData, THashFunctor>::Iterator::operator->() const {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
		if (_chainNumber >= _container->_hashMap.Size()) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return &(*_chainIterator);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	typename HashMap<TKey, TData, THashFunctor>::Iterator& HashMap <TKey, TData, THashFunctor>::Iterator::operator++() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

		++_chainIterator;
		while (_chainNumber < _container->_hashMap.Size() && _chainIterator == _container->_hashMap[_chainNumber].end()) {
#ifdef USE_CLAMPS
			_chainNumber = (_chainNumber == std::numeric_limits<std::size_t>::max()) ? std::numeric_limits<std::size_t>::max() : ++_chainNumber;
#else
			++_chainNumber;
#endif // USE_CLAMPS
			_chainIterator = (_chainNumber < _container->_hashMap.Size()) ? _container->_hashMap[_chainNumber].begin() : SList<PairType>::Iterator();
		}

		return *this;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap <TKey, TData, THashFunctor>::Iterator HashMap <TKey, TData, THashFunctor>::Iterator::operator++(int) {
		Iterator it = *this;
		operator++();
		return it;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline bool HashMap <TKey, TData, THashFunctor>::Iterator::operator!=(const Iterator& rhs) const {
		return (_container != rhs._container || _chainNumber != rhs._chainNumber || _chainIterator != rhs._chainIterator);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline bool HashMap <TKey, TData, THashFunctor>::Iterator::operator==(const Iterator& rhs) const {
		return !(operator!=(rhs));
	}
#pragma endregion Iterator

#pragma region ConstIterator
	template<typename TKey, typename TData, typename THashFunctor>
	HashMap<TKey, TData, THashFunctor>::ConstIterator::ConstIterator(const HashMap<TKey, TData, THashFunctor>& container, size_type chainNum, typename SList<PairType>::ConstIterator chainIter) :
		_container{ &container }, _chainNumber{ chainNum }, _chainIterator{ chainIter }
	{
	}

	template<typename TKey, typename TData, typename THashFunctor>
	HashMap<TKey, TData, THashFunctor>::ConstIterator::ConstIterator(const Iterator& other) :
		_container{ other._container }, _chainNumber{ other._chainNumber }, _chainIterator{ other._chainIterator }
	{
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::const_reference HashMap<TKey, TData, THashFunctor>::ConstIterator::operator*() const {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
		if (_chainNumber >= _container->_hashMap.Size()) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return *_chainIterator;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap<TKey, TData, THashFunctor>::ConstIterator::pointer HashMap <TKey, TData, THashFunctor>::ConstIterator::operator->() const {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
		if (_chainNumber >= _container->_hashMap.Size()) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return &(*_chainIterator);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	typename HashMap<TKey, TData, THashFunctor>::ConstIterator& HashMap <TKey, TData, THashFunctor>::ConstIterator::operator++() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

		++_chainIterator;
		while (_chainNumber < _container->_hashMap.Size() && _chainIterator == _container->_hashMap[_chainNumber].end()) {
#ifdef USE_CLAMPS
			_chainNumber = (_chainNumber == std::numeric_limits<std::size_t>::max()) ? std::numeric_limits<std::size_t>::max() : ++_chainNumber;
#else
			++_chainNumber;
#endif // USE_CLAMPS
			_chainIterator = (_chainNumber < _container->_hashMap.Size()) ? _container->_hashMap[_chainNumber].begin() : SList<PairType>::ConstIterator();
		}

		return *this;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline typename HashMap <TKey, TData, THashFunctor>::ConstIterator HashMap <TKey, TData, THashFunctor>::ConstIterator::operator++(int) {
		ConstIterator it = *this;
		operator++();
		return it;
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline bool HashMap <TKey, TData, THashFunctor>::ConstIterator::operator!=(const ConstIterator& rhs) const {
		return (_container != rhs._container || _chainNumber != rhs._chainNumber || _chainIterator != rhs._chainIterator);
	}

	template<typename TKey, typename TData, typename THashFunctor>
	inline bool HashMap <TKey, TData, THashFunctor>::ConstIterator::operator==(const ConstIterator& rhs) const {
		return !(operator!=(rhs));
	}
#pragma endregion ConstIterator
}