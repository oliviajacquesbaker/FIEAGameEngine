/// <summary>
/// The definitions of the class Datum, which is an object that stores values of a single type selected from one of several supported types.
/// </summary>

#include "pch.h"
#include "Datum.h"

namespace FIEAGameEngine {
#pragma region Constructors
	Datum::Datum(Datum::IncrementFunctor incrementFunctor) :
		_incrementFunctor{ incrementFunctor } 
	{
	}

	Datum::Datum(DatumTypes type, std::size_t size, Datum::IncrementFunctor incrementFunctor) :
		_type{type}, _incrementFunctor{ incrementFunctor }
	{
#ifdef USE_EXCEPTIONS
		if (type == DatumTypes::Unknown) throw std::runtime_error("Type cannot be Unknown.");
#endif // USE_EXCEPTIONS
		Reserve(size);
	}

	Datum::Datum(std::initializer_list<std::int32_t> list, Datum::IncrementFunctor incrementFunctor ) :
		_type{ DatumTypes::Integer}, _incrementFunctor{incrementFunctor}
	{
		Reserve(list.size());
		for (auto& value : list) {
			PushBack(value);
		}
	}

	Datum::Datum(std::initializer_list<float> list, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Float }, _incrementFunctor{ incrementFunctor }
	{
		Reserve(list.size());
		for (auto& value : list) {
			PushBack(value);
		}
	}

	Datum::Datum(std::initializer_list<std::string> list, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::String }, _incrementFunctor{ incrementFunctor }
	{
		Reserve(list.size());
		for (auto& value : list) {
			PushBack(value);
		}
	}

	Datum::Datum(std::initializer_list<glm::vec4> list, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Vector }, _incrementFunctor{ incrementFunctor }
	{
		Reserve(list.size());
		for (auto& value : list) {
			PushBack(value);
		}
	}

	Datum::Datum(std::initializer_list<glm::mat4> list, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Matrix }, _incrementFunctor{ incrementFunctor }
	{
		Reserve(list.size());
		for (auto& value : list) {
			PushBack(value);
		}
	}

	Datum::Datum(std::initializer_list<RTTI*> list, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Pointer }, _incrementFunctor{ incrementFunctor }
	{
		Reserve(list.size());
		for (auto& value : list) {
			PushBack(value);
		}
	}

	Datum::Datum(const Datum& rhs) {
		CopyHelper(rhs);
	}

	Datum::Datum(std::int32_t rhs, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Integer }, _incrementFunctor{ incrementFunctor }
	{
		PushBack(rhs);
	}

	Datum::Datum(float rhs, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Float }, _incrementFunctor{ incrementFunctor }
	{
		PushBack(rhs);
	}

	Datum::Datum(const std::string& rhs, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::String }, _incrementFunctor{ incrementFunctor }
	{
		PushBack(rhs);
	}

	Datum::Datum(const glm::vec4& rhs, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Vector }, _incrementFunctor{ incrementFunctor }
	{
		PushBack(rhs);
	}

	Datum::Datum(const glm::mat4& rhs, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Matrix }, _incrementFunctor{ incrementFunctor }
	{
		PushBack(rhs);
	}

	Datum::Datum(RTTI* rhs, Datum::IncrementFunctor incrementFunctor) :
		_type{ DatumTypes::Pointer }, _incrementFunctor{ incrementFunctor }
	{
		PushBack(rhs);
	}

	Datum::Datum(Datum&& rhs) noexcept :
		_data{ rhs._data }, _type{ rhs._type }, _capacity{ rhs._capacity }, _size{ rhs._size }, _isExternal{ rhs._isExternal }, _incrementFunctor{ rhs._incrementFunctor }
	{
		rhs._isExternal = false;
		rhs._size = rhs._capacity = 0;
	}
#pragma endregion Constructors

#pragma region AssignmentOperators
	Datum& Datum::operator=(const Datum& rhs) {
		if (this != &rhs) {
			ResetInternalStorage();
			CopyHelper(rhs);
		}
		return *this;
	}

	Datum& Datum::operator=(std::initializer_list<std::int32_t> list) {
		Datum(list).CopySwapHelper(*this);
		return *this;
	}

	Datum& Datum::operator=(std::initializer_list<float> list) {
		Datum(list).CopySwapHelper(*this);
		return *this;
	}

	Datum& Datum::operator=(std::initializer_list<std::string> list) {
		Datum(list).CopySwapHelper(*this);
		return *this;
	}

	Datum& Datum::operator=(std::initializer_list<glm::vec4> list) {
		Datum(list).CopySwapHelper(*this);
		return *this;
	}

	Datum& Datum::operator=(std::initializer_list<glm::mat4> list) {
		Datum(list).CopySwapHelper(*this);
		return *this;
	}

	Datum& Datum::operator=(std::initializer_list<RTTI*> list) {
		Datum(list).CopySwapHelper(*this);
		return *this;
	}

	Datum& Datum::operator=(std::int32_t rhs) {
		SetType(DatumTypes::Integer);
		ResetInternalStorage();
		PushBack(rhs); 
		return *this;
	}

	Datum& Datum::operator=(float rhs) {
		SetType(DatumTypes::Float);
		ResetInternalStorage();
		PushBack(rhs);
		return *this;
	}

	Datum& Datum::operator=(const std::string& rhs) {
		SetType(DatumTypes::String);
		ResetInternalStorage();
		PushBack(rhs); 
		return *this;
	}

	Datum& Datum::operator=(const glm::vec4& rhs) {
		SetType(DatumTypes::Vector);
		ResetInternalStorage();
		PushBack(rhs);
		return *this;
	}

	Datum& Datum::operator=(const glm::mat4& rhs) {
		SetType(DatumTypes::Matrix);
		ResetInternalStorage();
		PushBack(rhs);
		return *this;
	}

	Datum& Datum::operator=(RTTI* rhs) {
		SetType(DatumTypes::Pointer);
		ResetInternalStorage();
		PushBack(rhs);
		return *this;
	}

	Datum& Datum::operator=(Datum&& rhs) noexcept {
		if (this != &rhs) {
			rhs.CopySwapHelper(*this);
		}
		return *this;
	}
#pragma endregion AssignmentOperators

	Datum::~Datum() {
		ResetInternalStorage();
	}

	void Datum::CopyHelper(const Datum& rhs) {
		_incrementFunctor = rhs._incrementFunctor;
		_isExternal = rhs._isExternal;
		_type = rhs._type;
		if (_isExternal) {
			_data = rhs._data;
			_size = rhs._size;
			_capacity = rhs._capacity;
		}
		else {
			if(rhs._size > 0) Reserve(rhs._size);
			if (_type == DatumTypes::String) {
				for (std::size_t i = 0; i < rhs._size; ++i) {
					PushBack(rhs._data.s[i]);
				}
			}
			else {
				memcpy(_data.vp, rhs._data.vp, rhs._size * _typeSizes[static_cast<std::int32_t>(_type)]);
				_size = rhs._size;
			}
		}
	}

	void Datum::CopySwapHelper(Datum& rhs) {
		rhs.SetType(_type);
		rhs.ResetInternalStorage();
		rhs._size = _size;
		rhs._capacity = _capacity;
		rhs._isExternal = _isExternal;
		std::swap(rhs._data.vp, _data.vp);

		_size = _capacity = std::size_t(0);
		_data.vp = nullptr;
	}

	void Datum::Clear() {
#ifdef USE_EXCEPTIONS
		if (_isExternal) throw std::runtime_error("Datum does not own its storage; cannot change the size of its data.");
#endif // USE_EXCEPTIONS

		if (_type == DatumTypes::String) {
			for (std::size_t i = 0; i < _size; ++i) {
				std::string test = _data.s[i]; UNREFERENCED_LOCAL(test);
				_data.s[i].~basic_string();
			}			
		}	
		_size = 0;
	}

	void Datum::ShrinkToFit() {
#ifdef USE_EXCEPTIONS
		if (_isExternal) throw std::runtime_error("Datum does not own this memory, cannot resize.");
#endif // USE_EXCEPTIONS
		if (_capacity > _size) {
			if (_size == 0) {
				free(_data.vp);
				_data.vp = nullptr;
			}
			else {
				void* allocatedMem = realloc(_data.vp, _size * _typeSizes[static_cast<std::int32_t>(_type)]);
				assert(allocatedMem != nullptr);
				_data.vp = allocatedMem;
			}
			_capacity = _size;
		}
	}

	void Datum::Resize(std::size_t newSize) {
#ifdef USE_EXCEPTIONS
		if (_isExternal) throw std::runtime_error("Datum does not own this memory, cannot resize.");
#endif // USE_EXCEPTIONS
		if (newSize < _size) {
			if (_type == DatumTypes::String) {
				for (std::size_t i = newSize; i < _size; ++i) {
					_data.s[i].~basic_string();
				}
			}
			_size = newSize;
			ShrinkToFit();
		}
		else if (newSize > _size) {
			Reserve(newSize);

			PushDefaultFunction defaultPushFunction = _defaultPlaceFunction[static_cast<std::int32_t>(_type)];
#ifdef USE_EXCEPTIONS
			if (defaultPushFunction == nullptr) throw std::runtime_error("Datum has no known way to construct a default value of this type.");
#endif // USE_EXCEPTIONS

			while (_size < newSize) {
				(this->*defaultPushFunction)();
			}
		}
	}

	void Datum::Reserve(std::size_t capacity) {
#ifdef USE_EXCEPTIONS
		if (_isExternal) throw std::runtime_error("Datum does not own this memory, cannot resize.");
		if (_type == DatumTypes::Unknown) throw std::runtime_error("Datum does not have an assigned type to resize on yet.");
#endif // USE_EXCEPTIONS

		if (capacity > _capacity) {
			std::size_t size = _typeSizes[static_cast<std::int32_t>(_type)];
			assert(size > 0);

			void* allocatedMem = realloc(_data.vp, capacity * size);
			assert(allocatedMem != nullptr);
			_data.vp = allocatedMem;
			_capacity = capacity;
		}
	}

	void Datum::ResetInternalStorage() {
		if (!_isExternal && _capacity > std::size_t(0)) {
			Clear();
			ShrinkToFit();
		}
	}

	void Datum::SetStorage(void* array, std::size_t arraySize, DatumTypes type) {
#ifdef USE_EXCEPTIONS
		if (array == nullptr) throw std::runtime_error("Cannot assign a null array pointer.");
		if (arraySize == std::size_t(0)) throw std::runtime_error("Cannot assign a size zero array.");
#endif // USE_EXCEPTIONS

		SetType(type);
		ResetInternalStorage();
		_isExternal = true;
		_size = _capacity = arraySize;
		_data.vp = array;
	}

	void Datum::PushBackPrep(DatumTypes type) {
#ifdef USE_EXCEPTIONS
		if (_isExternal) throw std::runtime_error("Datum does not own its storage; cannot change the size of its data.");
#endif // USE_EXCEPTIONS

		SetType(type);
		if (_size == _capacity) {
			std::size_t increment = std::max(_incrementFunctor(_capacity), std::size_t(1));
			Reserve(_capacity + increment);
		}
	}

	void Datum::PopBack() {
#ifdef USE_EXCEPTIONS
		if (_isExternal) throw std::runtime_error("Datum does not own its storage; cannot change the size of its data.");
		if (_size == 0) throw std::runtime_error("Datum is empty.");
#endif // USE_EXCEPTIONS

		--_size;
		if (_type == DatumTypes::String) _data.s[_size].~basic_string();
	}

	bool Datum::RemoveAt(std::size_t index) {
#ifdef USE_EXCEPTIONS
		if (_isExternal) throw std::runtime_error("Datum does not own its storage; cannot change the size of its data.");
		if (_type == DatumTypes::Unknown) throw std::runtime_error("Datum's type has not been set yet, cannot perform remove operations.");
#endif // USE_EXCEPTIONS

		bool removed = false;
		if (index < _size) {
			if (_type == DatumTypes::String) _data.s[index].~basic_string();

			std::size_t typeSize = _typeSizes[static_cast<std::int32_t>(_type)];
			std::size_t shiftAmount = (_size-- - index) * typeSize;
			if (shiftAmount > 0) {
				std::byte* ptr = reinterpret_cast<std::byte*>(_data.vp);
#ifdef _WINDOWS
				memmove_s(ptr + (index * typeSize), shiftAmount, ptr + ((index + 1) * typeSize), shiftAmount);
#else
				memmove(ptr + (index * typeSize), ptr + ((index + 1) * typeSize), shiftAmount);
#endif
			}
			removed = true;
		}

		return removed;
	}

#pragma region FindIndex
	std::size_t Datum::FindIndex(std::int32_t value) const {
#ifdef USE_EXCEPTIONS
		if (_type != DatumTypes::Integer) throw std::runtime_error("Datum is not of the given type.");
#endif // USE_EXCEPTIONS

		for (std::size_t i = 0; i < _size; ++i) {
			if (_data.i[i] == value) return i;
		}
		return _size;
	}

	std::size_t Datum::FindIndex(float value) const {
#ifdef USE_EXCEPTIONS
		if (_type != DatumTypes::Float) throw std::runtime_error("Datum is not of the given type.");
#endif // USE_EXCEPTIONS
		for (std::size_t i = 0; i < _size; ++i) {
			if (_data.f[i] == value) return i;
		}
		return _size;
	}

	std::size_t Datum::FindIndex(const std::string& value) const {
#ifdef USE_EXCEPTIONS
		if (_type != DatumTypes::String) throw std::runtime_error("Datum is not of the given type.");
#endif // USE_EXCEPTIONS
		for (std::size_t i = 0; i < _size; ++i) {
			if (_data.s[i] == value) return i;
		}
		return _size;
	}

	std::size_t Datum::FindIndex(const glm::vec4& value) const {
#ifdef USE_EXCEPTIONS
		if (_type != DatumTypes::Vector) throw std::runtime_error("Datum is not of the given type.");
#endif // USE_EXCEPTIONS
		for (std::size_t i = 0; i < _size; ++i) {
			if (_data.v[i] == value) return i;
		}
		return _size;
	}

	std::size_t Datum::FindIndex(const glm::mat4& value) const {
#ifdef USE_EXCEPTIONS
		if (_type != DatumTypes::Matrix) throw std::runtime_error("Datum is not of the given type.");
#endif // USE_EXCEPTIONS
		for (std::size_t i = 0; i < _size; ++i) {
			if (_data.m[i] == value) return i;
		}
		return _size;
	}

	std::size_t Datum::FindIndex(RTTI* value) const {
#ifdef USE_EXCEPTIONS
		if (_type != DatumTypes::Pointer) throw std::runtime_error("Datum is not of the given type.");
#endif // USE_EXCEPTIONS
		for (std::size_t i = 0; i < _size; ++i) {
			if (_data.p[i]->Equals(value)) return i;
		}
		return _size;
	}
	
	std::size_t Datum::FindIndex(Scope& value) const {
#ifdef USE_EXCEPTIONS
		if (_type != DatumTypes::Table) throw std::runtime_error("Datum is not of the given type.");
#endif // USE_EXCEPTIONS
		for (std::size_t i = 0; i < _size; ++i) {
			if (_data.t[i] == &value) return i;
		}
		return _size;
	}
#pragma endregion FindIndex

#pragma region StringConversions
	void Datum::SetVectorFromString(const std::string& strValue, size_t index) {
		glm::vec4 vectorData;
		int capturedVals = sscanf_s(strValue.c_str(), "vec4(%f, %f, %f, %f)", &vectorData[0], &vectorData[1], &vectorData[2], &vectorData[3]);
#ifdef USE_EXCEPTIONS
		if (capturedVals != vectorData.length()) throw std::runtime_error("Datum could not find correct format to get variables from.");
#else
		UNREFERENCED_LOCAL(capturedVals);
#endif // USE_EXCEPTIONS
		Set(vectorData, index);
	}

	void Datum::SetMatrixFromString(const std::string& strValue, size_t index) {
		glm::mat4 matrixData;
		int capturedVals = sscanf_s(strValue.c_str(), "mat4x4((%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f), (%f, %f, %f, %f))",
			&matrixData[0][0], &matrixData[0][1], &matrixData[0][2], &matrixData[0][3],
			&matrixData[1][0], &matrixData[1][1], &matrixData[1][2], &matrixData[1][3],
			&matrixData[2][0], &matrixData[2][1], &matrixData[2][2], &matrixData[2][3],
			&matrixData[3][0], &matrixData[3][1], &matrixData[3][2], &matrixData[3][3]
		);
#ifdef USE_EXCEPTIONS
		if (capturedVals != (matrixData.length() * matrixData[0].length())) throw std::runtime_error("Datum could not find correct format to get variables from.");
#else
		UNREFERENCED_LOCAL(capturedVals);
#endif // USE_EXCEPTIONS
		Set(matrixData, index);
	}

	std::string Datum::ToString(std::size_t index) const {
		ToStringFunction strFunction = _toStringFunctions[static_cast<std::int32_t>(_type)];
#ifdef USE_EXCEPTIONS
		if (strFunction == nullptr) throw std::runtime_error("Datum has no known way to be converted to a string.");
#endif // USE_EXCEPTIONS
		return (this->*strFunction)(index);
	}

	void Datum::PushBackFromString(const std::string& stringValue) {
		Resize(Size() + 1);
		SetFromString(stringValue, Size() - 1);
	}

	void Datum::SetFromString(const std::string& stringValue, std::size_t index) {
		SetFromStringFunction setFunction = _setFromStringFunctions[static_cast<std::int32_t>(_type)];
#ifdef USE_EXCEPTIONS
		if (setFunction == nullptr) throw std::runtime_error("Datum has no known way to be converted from a string.");
#endif // USE_EXCEPTIONS
		(this->*setFunction)(stringValue, index);
	}
#pragma endregion StringConversions

	bool Datum::operator==(const Datum& rhs) const {
		if (_type != rhs._type || _size != rhs._size) return false;
		if (_type == DatumTypes::String) {
			for (std::size_t i = 0; i < _size; ++i) {
				if (_data.s[i] != rhs._data.s[i]) return false;
			}
		}
		else if (_type == DatumTypes::Pointer || _type == DatumTypes::Table) {
			for (std::size_t i = 0; i < _size; ++i) {
				if (_data.p[i] == nullptr && rhs._data.p[i] == nullptr) continue;
				if (_data.p[i] == nullptr || !_data.p[i]->Equals(rhs._data.p[i])) return false;
			}
		}
		else if (memcmp(_data.vp, rhs._data.vp, _size * _typeSizes[static_cast<std::int32_t>(_type)]) != 0) return false;
		return true;
	}
}

