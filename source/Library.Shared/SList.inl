/// <summary>
/// The fully specified implementation of the tempated class SList and its enveloped classes Iterator, ConstIterator, and its struct Node.
/// </summary>

#include "SList.h"

namespace FIEAGameEngine {
#pragma region Node
	template<typename value_type>
	template<typename... Args>
	inline SList<value_type>::Node::Node(Node* next, Args&&... args) :
		Data{ std::forward<Args>(args)... }, Next{ next }
	{
	}
#pragma endregion Node

#pragma region SList
	template <typename value_type>
	inline SList<value_type>::SList(std::initializer_list<value_type> list) {
		for (const_reference value : list) {
			PushBack(value);
		}
	}

	template<typename value_type>
	SList<value_type>::SList(const SList& rhs) {
		DeepCopy(rhs);
	}

	template<typename value_type>
	SList<value_type>::SList(SList&& rhs) noexcept :
		_front{ rhs._front }, _back{ rhs._back }, _size{ rhs._size }
	{
		rhs._front = nullptr;
		rhs._back = nullptr;
		rhs._size = 0;
	}

	template<typename value_type>
	SList<value_type>& SList<value_type>::operator=(const SList& rhs) {
		if (this != &rhs) {
			Clear();
			DeepCopy(rhs);
		}
		return *this;
	}

	template<typename value_type>
	SList<value_type>& SList<value_type>::operator=(SList&& rhs) noexcept {
		if (this != &rhs) {
			Clear();
			_front = rhs._front;
			_back = rhs._back;
			_size = rhs._size;
			rhs._front = nullptr;
			rhs._back = nullptr;
			rhs._size = 0;
		}
		return *this;
	}

	template<typename value_type>
	SList<value_type>::~SList() {
		Clear();
	}

	template<typename value_type>
	inline  typename SList<value_type>::reference SList<value_type>::Front() {
#ifdef USE_EXCEPTIONS
		if (_front == nullptr) throw std::runtime_error("List is empty.");
#endif // USE_EXCEPTIONS
		return _front->Data;
	}

	template<typename value_type>
	inline  typename SList<value_type>::const_reference SList<value_type>::Front() const {
#ifdef USE_EXCEPTIONS
		if (_front == nullptr) throw std::runtime_error("List is empty.");
#endif // USE_EXCEPTIONS
		return _front->Data;
	}

	template<typename value_type>
	inline  typename SList<value_type>::reference SList<value_type>::Back() {
#ifdef USE_EXCEPTIONS
		if (_back == nullptr) throw std::runtime_error("List is empty.");
#endif // USE_EXCEPTIONS
		return _back->Data;
	}

	template<typename value_type>
	inline  typename SList<value_type>::const_reference SList<value_type>::Back() const {
#ifdef USE_EXCEPTIONS
		if (_back == nullptr) throw std::runtime_error("List is empty.");
#endif // USE_EXCEPTIONS
		return _back->Data;
	}

	template <typename value_type>
	inline std::size_t SList<value_type>::Size() const {
		return _size;
	}

	template<typename value_type>
	inline bool SList<value_type>::IsEmpty() const {
		return (_size == std::size_t(0));
	}

	template<typename value_type>
	typename SList<value_type>::Iterator SList<value_type>::PushFront(const_reference data) {
		_front = new Node(_front, data);

		if (_size == 0) _back = _front;
		++_size;

		return Iterator(*this, _front);
	}

	template<typename value_type>
	typename SList<value_type>::Iterator SList<value_type>::PushFront(rvalue_reference data)
	{
		_front = new Node(_front, std::forward<value_type>(data));

		if (_size == 0) _back = _front;
		++_size;

		return Iterator(*this, _front);
	}

	template<typename value_type>
	typename SList<value_type>::Iterator SList<value_type>::PushBack(const_reference data) {
		Node* newNode = new Node(nullptr, data);
		if (_back != nullptr) _back->Next = newNode;
		_back = newNode;

		if (_size == 0) _front = newNode;
		++_size;

		return Iterator(*this, newNode);
	}

	template<typename value_type>
	typename SList<value_type>::Iterator SList<value_type>::PushBack(rvalue_reference data)
	{
		Node* newNode = new Node(nullptr, std::forward<value_type>(data));
		if (_back != nullptr) _back->Next = newNode;
		_back = newNode;

		if (_size == 0) _front = newNode;
		++_size;

		return Iterator(*this, newNode);
	}

	template<typename value_type>
	void SList<value_type>::PopFront() {
#ifdef USE_EXCEPTIONS
		if (_front == nullptr) throw std::runtime_error("List is empty.");
#endif // USE_EXCEPTIONS

		Node* removedNode = _front;
		_front = _front->Next;
		delete removedNode;

		--_size;
		if (_size == 0) _back = nullptr;
	}

	template<typename value_type>
	void SList<value_type>::PopBack() {
#ifdef USE_EXCEPTIONS
		if (_back == nullptr) throw std::runtime_error("List is empty.");
#endif // USE_EXCEPTIONS

		Node* removedNode = _back;
		--_size;
		if (_size == 0) {
			_front = nullptr;
			_back = nullptr;
		}
		else {
			Node* newBackNode = _front;
			while (newBackNode != nullptr && newBackNode != removedNode) {
				_back = newBackNode;
				newBackNode = newBackNode->Next;
			}
		}		
		delete removedNode;		
	}

	template<typename value_type>
	void SList<value_type>::Clear() {
		Node* currentNode = _front;
		while (currentNode != nullptr) {
			Node* nextNode = currentNode->Next;
			delete currentNode;
			currentNode = nextNode;
		}
		_size = 0;
		_front = nullptr;
		_back = nullptr;
	}

	template<typename value_type>
	void SList<value_type>::DeepCopy(const SList& rhs) {
		for (const auto& value : rhs) {
			PushBack(value);
		}
	}

	template<typename value_type>
	typename SList<value_type>::Iterator SList<value_type>::begin() {
		return Iterator(*this, _front);
	}

	template<typename value_type>
	typename SList<value_type>::Iterator SList<value_type>::end() {
		return Iterator(*this);
	}

	template<typename value_type>
	typename SList<value_type>::ConstIterator SList<value_type>::begin() const {
		return ConstIterator(*this, _front);
	}

	template<typename value_type>
	typename SList<value_type>::ConstIterator SList<value_type>::end() const {
		return ConstIterator(*this);
	}

	template<typename value_type>
	typename SList<value_type>::ConstIterator SList<value_type>::cbegin() const {
		return ConstIterator(*this, _front);
	}

	template<typename value_type>
	typename SList<value_type>::ConstIterator SList<value_type>::cend() const {
		return ConstIterator(*this);
	}

	template<typename value_type>
	typename SList<value_type>::Iterator SList<value_type>::InsertAfter(const_reference data, const Iterator& iter) {
#ifdef USE_EXCEPTIONS
		if(iter._container == nullptr || iter._container != this) throw std::runtime_error("Given iterator is not associated with this list.");
#endif // USE_EXCEPTIONS

		if (iter._node == nullptr) return PushBack(data);
		else {
			Node* newNode = new Node(iter._node->Next, data);
			if (newNode->Next == nullptr) _back = newNode;
			iter._node->Next = newNode;
			++_size;
			return Iterator(*this, newNode);
		}
	}

	template<typename value_type>
	template<typename EqualityFunctor>
	typename SList<value_type>::ConstIterator SList<value_type>::Find(const_reference data, EqualityFunctor equalityFunctor) const {
		Node* currentNode = _front;
		while (currentNode != nullptr) {
			if(equalityFunctor(currentNode->Data, data)) return ConstIterator(*this, currentNode);
			else currentNode = currentNode->Next;
		}
		return end();
	}

	template<typename value_type>
	template<typename EqualityFunctor>
	typename SList<value_type>::Iterator SList<value_type>::Find(const_reference data, EqualityFunctor equalityFunctor) {
		Node* currentNode = _front;
		while (currentNode != nullptr) {
			if (equalityFunctor(currentNode->Data, data)) return Iterator(*this, currentNode);
			else currentNode = currentNode->Next;
		}
		return end();
	}

	template<typename value_type>
	template<typename EqualityFunctor>
	inline bool SList<value_type>::Remove(const_reference data, EqualityFunctor equalityFunctor) {
		return Remove(Find(data, equalityFunctor));
	}

	template<typename value_type>
	inline bool SList<value_type>::Remove(const Iterator& iter) {
#ifdef USE_EXCEPTIONS
		if (iter._container == nullptr || iter._container != this) throw std::runtime_error("Given iterator is not associated with this list.");
#endif // USE_EXCEPTIONS
		if (iter._node == nullptr) return false;

		Node* currentNode = _front;
		if (_front == iter._node) {
			_front = currentNode->Next;
			--_size;
			if (_size == 0) _back = nullptr;
			delete currentNode;
			return true;
		}
		
		while (currentNode != nullptr) {
			if (currentNode->Next == iter._node) {
				if (currentNode->Next == _back) _back = currentNode;
				currentNode->Next = iter._node->Next;
				delete iter._node;
				--_size;
				return true;
			}
			currentNode = currentNode->Next;
		}

		return false;
	}

#pragma endregion SList

#pragma region Iterator
	template<typename value_type>
	SList<value_type>::Iterator::Iterator(const SList& container, Node* node) :
		_container{ &container }, _node{ node }
	{
	}

	template<typename value_type>
	inline typename SList<value_type>::reference SList<value_type>::Iterator::operator*() const {
#ifdef USE_EXCEPTIONS
		if (_node == nullptr) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return _node->Data;
	}

	template<typename value_type>
	inline typename SList<value_type>::Iterator& SList<value_type>::Iterator::operator++() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS
		if (_node != nullptr) _node = _node->Next;
		return *this;
	}

	template<typename value_type>
	inline typename SList<value_type>::Iterator SList<value_type>::Iterator::operator++(int) {
		Iterator it = *this;
		operator++();
		return it;
	}

	template<typename value_type>
	inline bool SList<value_type>::Iterator::operator!=(const Iterator& rhs) const {
		return (_container != rhs._container || _node != rhs._node);
	}

	template<typename value_type>
	inline bool SList<value_type>::Iterator::operator==(const Iterator& rhs) const {
		return !(operator!=(rhs));
	}
#pragma endregion Iterator

#pragma region ConstIterator
	template<typename value_type>
	SList<value_type>::ConstIterator::ConstIterator(const Iterator& other) :
		_container(other._container), _node(other._node)
	{
	}

	template<typename value_type>
	SList<value_type>::ConstIterator::ConstIterator(const SList& container, Node* node) :
		_container{ &container }, _node{ node }
	{
	}

	template<typename value_type>
	inline  typename SList<value_type>::const_reference SList<value_type>::ConstIterator::operator*() const {
#ifdef USE_EXCEPTIONS
		if (_node == nullptr) throw std::runtime_error("Iterator's data is equal to end, cannot be dereferenced.");
#endif // USE_EXCEPTIONS
		return _node->Data;
	}

	template<typename value_type>
	inline typename SList<value_type>::ConstIterator& SList<value_type>::ConstIterator::operator++() {
#ifdef USE_EXCEPTIONS
		if (_container == nullptr) throw std::runtime_error("Iterator is not associated with a container.");
#endif // USE_EXCEPTIONS
		if (_node != nullptr) _node = _node->Next;
		return *this;
	}

	template<typename value_type>
	inline typename SList<value_type>::ConstIterator SList<value_type>::ConstIterator::operator++(int) {
		ConstIterator it = *this;
		operator++();
		return it;
	}

	template<typename value_type>
	inline bool SList<value_type>::ConstIterator::operator!=(const ConstIterator& rhs) const {
		if (_container != rhs._container) return true;
		if (_node != rhs._node) return true;
		return false;
	}

	template<typename value_type>
	inline bool SList<value_type>::ConstIterator::operator==(const ConstIterator& rhs) const {
		return !(operator!=(rhs));
	}
	
#pragma endregion ConstIterator
}