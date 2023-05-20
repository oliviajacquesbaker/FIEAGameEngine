/// <summary>
/// The fully specified implementation of the tempated class Vector and its enveloped classes Iterator and ConstIterator.
/// </summary>

#pragma once
#include "Vector.h"

namespace FIEAGameEngine {
#pragma region Vector
	template<typename T>
	inline Vector<T>::Vector(size_type capacity, std::function<size_type(size_type)> incrementFunctor) :
		_incrementFunctor{incrementFunctor}
	{
		Reserve(capacity);
	}

	template<typename value_type>
	inline Vector<value_type>::Vector(std::initializer_list<value_type> list, std::function<size_type(size_type)> incrementFunctor) :
		_incrementFunctor{incrementFunctor}
	{
		Reserve(list.size());
		for (const_reference value : list) {
			PushBack(value);
		}
	}

	template<typename value_type>
	Vector<value_type>::Vector(const Vector& rhs) {
		DeepCopy(rhs);
	}

	template<typename value_type>
	Vector<value_type>::Vector(Vector&& rhs) noexcept :
		_data{ rhs._data }, _capacity{ rhs._capacity }, _size{ rhs._size }, _incrementFunctor{rhs._incrementFunctor}
	{
		rhs._data = nullptr;
		rhs._capacity = 0;
		rhs._size = 0;
	}

	template<typename value_type>
	Vector<value_type>& Vector<value_type>::operator=(const Vector& rhs) {
		if (this != &rhs) {
			Clear();
			ShrinkToFit();
			DeepCopy(rhs);
		}
		return *this;
	}

	template<typename value_type>
	Vector<value_type>& Vector<value_type>::operator=(Vector&& rhs) noexcept {
		if (this != &rhs) {
			Clear();
			ShrinkToFit();
			
			_data = rhs._data;
			_capacity = rhs._capacity;
			_size = rhs._size;
			rhs._data = nullptr;
			rhs._capacity = 0;
			rhs._size = 0;
		}
		return *this;
	}

	template<typename value_type>
	Vector<value_type>::~Vector() {
		Clear();
		ShrinkToFit();
	}

	template<typename value_type>
	inline typename Vector<value_type>::reference Vector<value_type>::operator[](size_type index) {
#ifdef USE_EXCEPTIONS
		if (index >= _size) throw std::runtime_error("Index is out of bounds.");
#endif // USE_EXCEPTIONS
		return _data[index];
	}

	template<typename value_type>
	inline typename Vector<value_type>::const_reference Vector<value_type>::operator[](size_type index) const {
#ifdef USE_EXCEPTIONS
		if (index >= _size) throw std::runtime_error("Index is out of bounds.");
#endif // USE_EXCEPTIONS
		return _data[index];
	}

	template<typename value_type>
	inline typename Vector<value_type>::reference Vector<value_type>::At(size_type index) {
#ifdef USE_EXCEPTIONS
		if (index >= _size) throw std::runtime_error("Index is out of bounds.");
#endif // USE_EXCEPTIONS
		return _data[index];
	}

	template<typename value_type>
	inline typename Vector<value_type>::const_reference Vector<value_type>::At(size_type index) const {
#ifdef USE_EXCEPTIONS
		if (index >= _size) throw std::runtime_error("Index is out of bounds.");
#endif // USE_EXCEPTIONS
		return _data[index];
	}

	template<typename value_type>
	inline typename Vector<value_type>::size_type Vector<value_type>::Size() const {
		return _size;
	}

	template<typename value_type>
	inline typename Vector<value_type>::size_type Vector<value_type>::Capacity() const {
		return _capacity;
	}

	template<typename value_type>
	inline bool Vector<value_type>::IsEmpty() const {
		return (_size == size_type(0));
	}

	template<typename value_type>
	void Vector<value_type>::Reserve(size_type capacity) {
		if(capacity > _capacity) {
			value_type* data = reinterpret_cast<value_type*>(realloc(_data, capacity * sizeof(value_type)));
			assert(data != nullptr);
			_data = data;
			_capacity = capacity;
		}
	}

	template<typename value_type>
	void Vector<value_type>::ShrinkToFit() {
		if (_capacity > _size) {
			if (_size == 0) {
				free(_data);
				_data = nullptr;
			}
			else {
				value_type* data = reinterpret_cast<value_type*>(realloc(_data, _size * sizeof(value_type)));
				assert(data != nullptr);
				_data = data;
			}
			_capacity = _size;
		}
	}

	template<typename value_type>
	inline void Vector<value_type>::Clear() {
		for (size_type i = 0; i < _size; ++i) {
			_data[i].~value_type();
		}
		_size = 0;
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator Vector<value_type>::begin() {
		return Iterator(*this);
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator Vector<value_type>::end() {
		return Iterator(*this, _size);
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator Vector<value_type>::begin() const {
		return ConstIterator(*this);
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator Vector<value_type>::end() const {
		return ConstIterator(*this, _size);
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator Vector<value_type>::cbegin() const {
		return ConstIterator(*this);
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator Vector<value_type>::cend() const {
		return ConstIterator(*this, _size);
	}

	template<typename value_type>
	inline typename Vector<value_type>::reference Vector<value_type>::Front() {
#ifdef USE_EXCEPTIONS
		if (_size == 0) throw std::runtime_error("Vector is empty.");
#endif // USE_EXCEPTIONS
		return _data[0];
	}

	template<typename value_type>
	inline typename Vector<value_type>::const_reference Vector<value_type>::Front() const {
#ifdef USE_EXCEPTIONS
		if (_size == 0) throw std::runtime_error("Vector is empty.");
#endif // USE_EXCEPTIONS
		return _data[0];
	}

	template<typename value_type>
	inline typename Vector<value_type>::reference Vector<value_type>::Back() {
#ifdef USE_EXCEPTIONS
		if (_size == 0) throw std::runtime_error("Vector is empty.");
#endif // USE_EXCEPTIONS
		return _data[_size - 1];
	}

	template<typename value_type>
	inline typename Vector<value_type>::const_reference Vector<value_type>::Back() const {
#ifdef USE_EXCEPTIONS
		if (_size == 0) throw std::runtime_error("Vector is empty.");
#endif // USE_EXCEPTIONS
		return _data[_size - 1];
	}

	template<typename value_type>
	void Vector<value_type>::DeepCopy(const Vector& rhs) {
		Reserve(rhs._size);
		_incrementFunctor = rhs._incrementFunctor;
		for (size_type i = 0; i < rhs._size; ++i) {
			PushBack(rhs[i]);
		}
	}

	template<typename value_type>
	template<typename EqualityFunctor>
	inline typename Vector<value_type>::ConstIterator Vector<value_type>::Find(const_reference data, EqualityFunctor equalityFunctor) const {
		for (size_type i = 0; i < _size; ++i) {
			if (equalityFunctor(At(i), data)) return ConstIterator(*this, i);
		}
		return end();
	}

	template<typename value_type>
	template<typename EqualityFunctor>
	inline typename Vector<value_type>::Iterator Vector<value_type>::Find(const_reference data, EqualityFunctor equalityFunctor) {
		for (size_type i = 0; i < _size; ++i) {
			if (equalityFunctor(At(i), data)) return Iterator(*this, i);
		}
		return end();
	}

	template<typename value_type>
	typename Vector<value_type>::Iterator Vector<value_type>::PushBack(const_reference data) {
		if (_size == _capacity) {
			size_type increment = std::max(_incrementFunctor(_capacity), size_type(1));
			Reserve(_capacity + increment);
		}
		new (_data + _size) value_type(data);
		++_size;
		return Iterator(*this, _size - 1);
	}

	template<typename value_type>
	typename Vector<value_type>::Iterator Vector<value_type>::PushBack(rvalue_reference data) {
		if (_size == _capacity) {
			size_type increment = std::max(_incrementFunctor(_capacity), size_type(1));
			Reserve(_capacity + increment);
		}
		new (_data + _size) value_type(std::forward<value_type>(data));
		++_size;
		return Iterator(*this, _size - 1);
	}

	template<typename value_type>
	template<typename EqualityFunctor>
	inline bool Vector<value_type>::Remove(const_reference data, EqualityFunctor equalityFunctor) {
		return Remove(Find(data, equalityFunctor));
	}

	template<typename value_type>
	inline bool Vector<value_type>::Remove(const Iterator& iter) {
		return Remove(iter, iter + 1);
	}

	template<typename value_type>
	inline bool Vector<value_type>::Remove(const Iterator& iterFirst, const Iterator& iterLast) {
#ifdef USE_EXCEPTIONS
		if (iterFirst._container == nullptr || iterFirst._container != this) throw std::runtime_error("Given iterator(s) are not associated with this container.");
		if (iterFirst._container != iterLast._container) throw std::runtime_error("Given iterators do not refer to the same container.");
#endif // USE_EXCEPTIONS

		if (_size == 0 || iterFirst._index < 0 || iterLast._index <= iterFirst._index || iterLast._index > _size) return false;

		size_type shiftAmount = (end() - iterLast);
		for (size_type i = iterFirst._index; i < iterLast._index; ++i) {
			_data[i].~value_type();
			--_size;
		}
#ifdef _WINDOWS
		memmove_s(&_data[iterFirst._index], (shiftAmount) * sizeof(value_type), &_data[iterLast._index], (shiftAmount) * sizeof(value_type));
#else
		memmove(&_data[iterFirst._index], &_data[iterLast._index], (shiftAmount) * sizeof(value_type));
#endif

		return true;
	}

	template<typename value_type>
	inline void Vector<value_type>::PopBack() {
#ifdef USE_EXCEPTIONS
		if (_size == 0) throw std::runtime_error("Vector is empty.");
#endif // USE_EXCEPTIONS

		_data[--_size].~value_type();
	}
#pragma endregion Vector

#pragma region Iterator
	template<typename value_type>
	Vector<value_type>::Iterator::Iterator(Vector& container, size_type index) :
		_container{ &container }, _index{ index }
	{
	}

	template<typename value_type>
	inline typename Vector<value_type>::reference Vector<value_type>::Iterator::operator*() const {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
		if (_index >= _container->Size()) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return _container->_data[_index];
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator& Vector<value_type>::Iterator::operator++() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS
		
#ifdef USE_CLAMPS
		_index = (_index == std::numeric_limits<std::size_t>::max()) ? std::numeric_limits<std::size_t>::max() : ++_index;
#else
		++_index;
#endif // USE_CLAMPS

		return *this;
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator Vector<value_type>::Iterator::operator++(int) {
		Iterator it = *this;
		operator++();
		return it;
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator& Vector<value_type>::Iterator::operator--() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS
		
#ifdef USE_CLAMPS
		_index = (_index == size_type(0)) ? size_type(0) : --_index;
#else
		--_index;
#endif // USE_CLAMPS

		return *this;
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator Vector<value_type>::Iterator::operator--(int) {
		Iterator it = *this;
		operator--();
		return it;
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator::difference_type Vector<value_type>::Iterator::operator-(const Vector<value_type>::Iterator& rhs) const {
#ifdef USE_EXCEPTIONS
		if (_container != rhs._container) throw std::runtime_error("Iterators are not associated with the same container.");
#endif // USE_EXCEPTIONS
		return (_index - rhs._index);
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator& Vector<value_type>::Iterator::operator+=(difference_type incrementAmount) {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

#ifdef USE_CLAMPS
		if (incrementAmount > 0 && _index >= std::numeric_limits<std::size_t>::max() - incrementAmount) _index = std::numeric_limits<std::size_t>::max();
		else if (incrementAmount < 0 && _index <= size_type(0) - incrementAmount) _index = size_type(0);
		else _index += incrementAmount;
#else
		_index += incrementAmount;
#endif // USE_CLAMPS
		return *this;
	}

	template<typename value_type>
	inline typename Vector<value_type>::Iterator& Vector<value_type>::Iterator::operator-=(difference_type decrementAmount) {
		return *this += (-decrementAmount);
	}

	template<typename value_type>
	inline typename Vector<value_type>::reference Vector<value_type>::Iterator::operator[](difference_type index) {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

		_index = index;
		return _container->operator[](_index);
	}

	template<typename value_type>
	inline bool Vector<value_type>::Iterator::operator!=(const Iterator& rhs) const {
		return (_container != rhs._container || _index != rhs._index);
	}

	template<typename value_type>
	inline bool Vector<value_type>::Iterator::operator==(const Iterator& rhs) const {
		return !(operator!=(rhs));
	}

	template<typename value_type>
	inline bool Vector<value_type>::Iterator::operator<=(const Iterator& rhs) const {
		return (_index <= rhs._index && _container == rhs._container);
	}

	template<typename value_type>
	inline bool Vector<value_type>::Iterator::operator>=(const Iterator& rhs) const {
		return (_index >= rhs._index && _container == rhs._container);
	}

	template<typename value_type>
	inline bool Vector<value_type>::Iterator::operator<(const Iterator& rhs) const {
		return (_index < rhs._index && _container == rhs._container);
	}

	template<typename value_type>
	inline bool Vector<value_type>::Iterator::operator>(const Iterator& rhs) const {
		return (_index > rhs._index && _container == rhs._container);
	}
#pragma endregion Iterator

#pragma region ConstIterator
	template<typename value_type>
	Vector<value_type>::ConstIterator::ConstIterator(const Vector& container, size_type index) :
		_container{ &container }, _index{ index }
	{
	}

	template<typename value_type>
	Vector<value_type>::ConstIterator::ConstIterator(const Iterator& other) :
		_container(other._container), _index(other._index)
	{
	}

	template<typename value_type>
	inline typename Vector<value_type>::const_reference Vector<value_type>::ConstIterator::operator*() const {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
		if (_index >= _container->Size()) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return _container->_data[_index];
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator& Vector<value_type>::ConstIterator::operator++() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

#ifdef USE_CLAMPS
		_index = (_index == std::numeric_limits<std::size_t>::max()) ? std::numeric_limits<std::size_t>::max() : ++_index;
#else
		++_index;
#endif // USE_CLAMPS

		return *this;
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator Vector<value_type>::ConstIterator::operator++(int) {
		ConstIterator it = *this;
		operator++();
		return it;
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator& Vector<value_type>::ConstIterator::operator--() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

#ifdef USE_CLAMPS
		_index = (_index == size_type(0)) ? size_type(0) : --_index;
#else
		--_index;
#endif // USE_CLAMPS

		return *this;
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator Vector<value_type>::ConstIterator::operator--(int) {
		ConstIterator it = *this;
		operator--();
		return it;
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator::difference_type Vector<value_type>::ConstIterator::operator-(const Vector<value_type>::ConstIterator& rhs) const {
#ifdef USE_EXCEPTIONS
		if (_container != rhs._container) throw std::runtime_error("Iterators are not associated with the same container.");
#endif // USE_EXCEPTIONS
		return (_index - rhs._index);
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator& Vector<value_type>::ConstIterator::operator+=(difference_type incrementAmount) {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

#ifdef USE_CLAMPS
		if (incrementAmount > 0 && _index >= std::numeric_limits<std::size_t>::max() - incrementAmount) _index = std::numeric_limits<std::size_t>::max();
		else if (incrementAmount < 0 && _index <= size_type(0) - incrementAmount) _index = size_type(0);
		else _index += incrementAmount;
#else
		_index += incrementAmount;
#endif // USE_CLAMPS
		return *this;
	}

	template<typename value_type>
	inline typename Vector<value_type>::ConstIterator& Vector<value_type>::ConstIterator::operator-=(difference_type decrementAmount) {
		return *this += (-decrementAmount);
	}

	template<typename value_type>
	inline typename Vector<value_type>::const_reference Vector<value_type>::ConstIterator::operator[](difference_type index) {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS

		_index = index;
		return _container->operator[](_index);
	}

	template<typename value_type>
	inline bool Vector<value_type>::ConstIterator::operator!=(const ConstIterator& rhs) const {
		return (_container != rhs._container || _index != rhs._index);
	}

	template<typename value_type>
	inline bool Vector<value_type>::ConstIterator::operator==(const ConstIterator& rhs) const {
		return !(operator!=(rhs));
	}

	template<typename value_type>
	inline bool Vector<value_type>::ConstIterator::operator<=(const ConstIterator& rhs) const {
		return (_index <= rhs._index && _container == rhs._container);
	}

	template<typename value_type>
	inline bool Vector<value_type>::ConstIterator::operator>=(const ConstIterator& rhs) const {
		return (_index >= rhs._index && _container == rhs._container);
	}

	template<typename value_type>
	inline bool Vector<value_type>::ConstIterator::operator<(const ConstIterator& rhs) const {
		return (_index < rhs._index && _container == rhs._container);
	}

	template<typename value_type>
	inline bool Vector<value_type>::ConstIterator::operator>(const ConstIterator& rhs) const {
		return (_index > rhs._index && _container == rhs._container);
	}
#pragma endregion ConstIterator

}

