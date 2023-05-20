/// <summary>
/// The inline definitions of the class Datum, which is an object that stores values of a single type selected from one of several supported types.
/// </summary>

#pragma once
#include "Datum.h"

namespace FIEAGameEngine {
	inline std::size_t Datum::Size() const {
		return _size;
	}

	inline std::size_t Datum::Capacity() const {
		return _capacity;
	}

	inline Datum::DatumTypes Datum::Type() const {
		return _type;
	}

	inline bool Datum::IsExternal() const {
		return _isExternal;
	}

	inline bool Datum::IsEmpty() const {
		return (_size == std::size_t(0));
	}

	inline Datum::IncrementFunctor Datum::GetIncrementFunction() const {
		return _incrementFunctor;
	}

	inline void Datum::SetType(DatumTypes type) {
#ifdef USE_EXCEPTIONS
		if (type == DatumTypes::Unknown) throw std::runtime_error("Type cannot be set to Unknown.");
		if (_type != DatumTypes::Unknown && _type != type) throw std::runtime_error("Type has already been assigned.");
#endif // USE_EXCEPTIONS
		_type = type;
	}

	inline void Datum::SetIncrementFunction(IncrementFunctor incrementFunctor) {
		_incrementFunctor = incrementFunctor;
	}

#pragma region SetStorage
	inline void Datum::SetStorage(std::int32_t* intArray, std::size_t arraySize) {
		SetStorage(intArray, arraySize, DatumTypes::Integer);
	}

	inline void Datum::SetStorage(float* floatArray, std::size_t arraySize) {
		SetStorage(floatArray, arraySize, DatumTypes::Float);
	}

	inline void Datum::SetStorage(std::string* stringArray, std::size_t arraySize) {
		SetStorage(stringArray, arraySize, DatumTypes::String);
	}

	inline void Datum::SetStorage(glm::vec4* vectorArray, std::size_t arraySize) {
		SetStorage(vectorArray, arraySize, DatumTypes::Vector);
	}

	inline void Datum::SetStorage(glm::mat4* matrixArray, std::size_t arraySize) {
		SetStorage(matrixArray, arraySize, DatumTypes::Matrix);
	}

	inline void Datum::SetStorage(RTTI** pointerArray, std::size_t arraySize) {
		SetStorage(pointerArray, arraySize, DatumTypes::Pointer);
	}
#pragma endregion SetStorage

#pragma region Front
	inline std::int32_t& Datum::FrontAsInt() {
		FrontBackChecker(DatumTypes::Integer);
		return _data.i[0];
	}

	inline std::int32_t Datum::FrontAsInt() const {
		FrontBackChecker(DatumTypes::Integer);
		return _data.i[0];
	}

	inline float& Datum::FrontAsFloat() {
		FrontBackChecker(DatumTypes::Float);
		return _data.f[0];
	}

	inline float Datum::FrontAsFloat() const {
		FrontBackChecker(DatumTypes::Float);
		return _data.f[0];
	}

	inline std::string& Datum::FrontAsString() {
		FrontBackChecker(DatumTypes::String);
		return _data.s[0];
	}

	inline const std::string& Datum::FrontAsString() const {
		FrontBackChecker(DatumTypes::String);
		return _data.s[0];
	}

	inline glm::vec4& Datum::FrontAsVector() {
		FrontBackChecker(DatumTypes::Vector);
		return _data.v[0];
	}

	inline const glm::vec4& Datum::FrontAsVector() const {
		FrontBackChecker(DatumTypes::Vector);
		return _data.v[0];
	}

	inline glm::mat4& Datum::FrontAsMatrix() {
		FrontBackChecker(DatumTypes::Matrix);
		return _data.m[0];
	}

	inline const glm::mat4& Datum::FrontAsMatrix() const {
		FrontBackChecker(DatumTypes::Matrix);
		return _data.m[0];
	}

	inline RTTI*& Datum::FrontAsPointer() {
		FrontBackChecker(DatumTypes::Pointer);
		return _data.p[0];
	}

	inline RTTI* Datum::FrontAsPointer() const {
		FrontBackChecker(DatumTypes::Pointer);
		return _data.p[0];
	}
#pragma endregion Front

#pragma region Back
	inline std::int32_t& Datum::BackAsInt() {
		FrontBackChecker(DatumTypes::Integer);
		return _data.i[_size - 1];
	}

	inline std::int32_t Datum::BackAsInt() const {
		FrontBackChecker(DatumTypes::Integer);
		return _data.i[_size - 1];
	}

	inline float& Datum::BackAsFloat() {
		FrontBackChecker(DatumTypes::Float);
		return _data.f[_size - 1];
	}

	inline float Datum::BackAsFloat() const {
		FrontBackChecker(DatumTypes::Float);
		return _data.f[_size - 1];
	}

	inline std::string& Datum::BackAsString() {
		FrontBackChecker(DatumTypes::String);
		return _data.s[_size - 1];
	}

	inline const std::string& Datum::BackAsString() const {
		FrontBackChecker(DatumTypes::String);
		return _data.s[_size - 1];
	}

	inline glm::vec4& Datum::BackAsVector() {
		FrontBackChecker(DatumTypes::Vector);
		return _data.v[_size - 1];
	}

	inline const glm::vec4& Datum::BackAsVector() const {
		FrontBackChecker(DatumTypes::Vector);
		return _data.v[_size - 1];
	}

	inline glm::mat4& Datum::BackAsMatrix() {
		FrontBackChecker(DatumTypes::Matrix);
		return _data.m[_size - 1];
	}

	inline const glm::mat4& Datum::BackAsMatrix() const {
		FrontBackChecker(DatumTypes::Matrix);
		return _data.m[_size - 1];
	}

	inline RTTI*& Datum::BackAsPointer() {
		FrontBackChecker(DatumTypes::Pointer);
		return _data.p[_size - 1];
	}

	inline RTTI* Datum::BackAsPointer() const {
		FrontBackChecker(DatumTypes::Pointer);
		return _data.p[_size - 1];
	}

	inline Scope& Datum::BackAsTable() {
		FrontBackChecker(DatumTypes::Table);
		return *_data.t[_size - 1];
	}

	inline const Scope& Datum::BackAsTable() const {
		FrontBackChecker(DatumTypes::Table);
		return *_data.t[_size - 1];
	}
#pragma endregion Back

	inline void Datum::GetSetChecker(DatumTypes type, size_t index) const {
#ifdef USE_EXCEPTIONS
		if (_type != type) throw std::runtime_error("Datum is not of the same type as the given value.");
		if (index >= _size) throw std::runtime_error("Index out of bounds.");
#endif // USE_EXCEPTIONS
	}

	inline void Datum::FrontBackChecker(DatumTypes type) const {
#ifdef USE_EXCEPTIONS
		if (_type != type) throw std::runtime_error("Datum is not of the indicated type.");
		if (_size == 0) throw std::runtime_error("Datum is empty.");
#endif // USE_EXCEPTIONS
	}

#pragma region Getters
	inline std::int32_t& Datum::GetAsInt(std::size_t index) {
		GetSetChecker(DatumTypes::Integer, index);
		return _data.i[index];
	}

	inline std::int32_t Datum::GetAsInt(std::size_t index) const {
		GetSetChecker(DatumTypes::Integer, index);
		return _data.i[index];
	}

	inline float& Datum::GetAsFloat(std::size_t index) {
		GetSetChecker(DatumTypes::Float, index);
		return _data.f[index];
	}

	inline float Datum::GetAsFloat(std::size_t index) const {
		GetSetChecker(DatumTypes::Float, index);
		return _data.f[index];
	}

	inline std::string& Datum::GetAsString(std::size_t index) {
		GetSetChecker(DatumTypes::String, index);
		return _data.s[index];
	}

	inline const std::string& Datum::GetAsString(std::size_t index) const {
		GetSetChecker(DatumTypes::String, index);
		return _data.s[index];
	}

	inline glm::vec4& Datum::GetAsVector(std::size_t index) {
		GetSetChecker(DatumTypes::Vector, index);
		return _data.v[index];
	}

	inline const glm::vec4& Datum::GetAsVector(std::size_t index) const {
		GetSetChecker(DatumTypes::Vector, index);
		return _data.v[index];
	}

	inline glm::mat4& Datum::GetAsMatrix(std::size_t index) {
		GetSetChecker(DatumTypes::Matrix, index);
		return _data.m[index];
	}

	inline const glm::mat4& Datum::GetAsMatrix(std::size_t index) const {
		GetSetChecker(DatumTypes::Matrix, index);
		return _data.m[index];
	}

	inline RTTI*& Datum::GetAsPointer(std::size_t index) {
		GetSetChecker(DatumTypes::Pointer, index);
		return _data.p[index];
	}

	inline RTTI* Datum::GetAsPointer(std::size_t index) const {
		GetSetChecker(DatumTypes::Pointer, index);
		return _data.p[index];
	}

	inline Scope& Datum::GetAsTable(std::size_t index) {
		GetSetChecker(DatumTypes::Table, index);
		return *_data.t[index];
	}

	inline const Scope& Datum::GetAsTable(std::size_t index) const {
		GetSetChecker(DatumTypes::Table, index);
		return *_data.t[index];
	}
#pragma endregion Getters

#pragma region Setters
	inline void Datum::Set(std::int32_t value, std::size_t index) {
		GetSetChecker(DatumTypes::Integer, index);
		_data.i[index] = value;
	}

	inline void Datum::Set(float value, std::size_t index) {
		GetSetChecker(DatumTypes::Float, index);
		_data.f[index] = value;
	}

	inline void Datum::Set(std::string value, std::size_t index) {
		GetSetChecker(DatumTypes::String, index);
		_data.s[index] = value;
	}

	inline void Datum::Set(glm::vec4 value, std::size_t index) {
		GetSetChecker(DatumTypes::Vector, index);
		_data.v[index] = value;
	}

	inline void Datum::Set(glm::mat4 value, std::size_t index) {
		GetSetChecker(DatumTypes::Matrix, index);
		_data.m[index] = value;
	}

	inline void Datum::Set(RTTI* value, std::size_t index) {
		GetSetChecker(DatumTypes::Pointer, index);
		_data.p[index] = value;
	}
#pragma endregion Setters

#pragma region PushBacks
	inline void Datum::PushDefaultInt() {
		PushBack(int());
	}

	inline void Datum::PushDefaultFloat() {
		PushBack(float());
	}

	inline void Datum::PushDefaultString() {
		PushBack(std::string());
	}

	inline void Datum::PushDefaultVector() {
		PushBack(glm::vec4());
	}

	inline void Datum::PushDefaultMatrix() {
		PushBack(glm::mat4());
	}

	inline void Datum::PushDefaultPointer() {
		PushBack(nullptr);
	}

	inline void Datum::PushBack(std::int32_t data) {
		PushBackPrep(DatumTypes::Integer);
		new (_data.i + _size++) std::int32_t(data);
	}

	inline void Datum::PushBack(float data) {
		PushBackPrep(DatumTypes::Float);
		new (_data.f + _size++) float(data);
	}

	inline void Datum::PushBack(const std::string& data) {
		PushBackPrep(DatumTypes::String);
		new (_data.s + _size++) std::string(data);
	}

	inline void Datum::PushBack(const glm::vec4& data) {
		PushBackPrep(DatumTypes::Vector);
		new (_data.v + _size++) glm::vec4(data);
	}

	inline void Datum::PushBack(const glm::mat4& data) {
		PushBackPrep(DatumTypes::Matrix);
		new (_data.m + _size++) glm::mat4(data);
	}

	inline void Datum::PushBack(RTTI* data) {
		PushBackPrep(DatumTypes::Pointer);
		new (_data.p + _size++) RTTI* (data);
	}

	inline void Datum::PushBack(Scope& data) {
		PushBackPrep(DatumTypes::Table);
		new (_data.t + _size++) Scope* (&data);
	}
#pragma endregion PushBacks

#pragma region Removals
	inline bool Datum::Remove(std::int32_t data) {
		return RemoveAt(FindIndex(data));
	}

	inline bool Datum::Remove(float data) {
		return RemoveAt(FindIndex(data));
	}

	inline bool Datum::Remove(const std::string& data) {
		return RemoveAt(FindIndex(data));
	}

	inline bool Datum::Remove(const glm::vec4& data) {
		return RemoveAt(FindIndex(data));
	}

	inline bool Datum::Remove(const glm::mat4& data) {
		return RemoveAt(FindIndex(data));
	}

	inline bool Datum::Remove(RTTI* data) {
		return RemoveAt(FindIndex(data));
	}
#pragma endregion Removals

#pragma region StringConversions
	inline std::string Datum::StringFromInt(size_t index) const {
		return std::to_string(GetAsInt(index));
	}

	inline std::string Datum::StringFromFloat(size_t index) const {
		return std::to_string(GetAsFloat(index));
	}

	inline std::string Datum::StringFromString(size_t index) const {
		return GetAsString(index);
	}

	inline std::string Datum::StringFromVector(size_t index) const {
		return glm::to_string(GetAsVector(index));
	}

	inline std::string Datum::StringFromMatrix(size_t index) const {
		return glm::to_string(GetAsMatrix(index));
	}

	inline std::string Datum::StringFromPointer(size_t index) const {
		RTTI* ptr = GetAsPointer(index);
		return (ptr == nullptr) ? std::string("nullptr") : ptr->ToString();
	}

	inline std::string Datum::StringFromTable(size_t index) const {
		GetSetChecker(DatumTypes::Table, index);
		RTTI* ptr = _data.p[index];
		return ptr->ToString();
	}

	inline void Datum::SetIntFromString(const std::string& strValue, size_t index) {
		Set(std::stoi(strValue), index);
	}

	inline void Datum::SetFloatFromString(const std::string& strValue, size_t index) {
		Set(std::stof(strValue), index);
	}

	inline void Datum::SetStringFromString(const std::string& strValue, size_t index) {
		Set(strValue, index);
	}
#pragma endregion StringConversions

#pragma region EqualityOperators
	inline bool Datum::operator==(const std::int32_t rhs) const {
		return (_type == DatumTypes::Integer && _size == std::size_t(1) && _data.i[0] == rhs);
	}

	inline bool Datum::operator==(const float rhs) const {
		return (_type == DatumTypes::Float && _size == std::size_t(1) && _data.f[0] == rhs);
	}

	inline bool Datum::operator==(const std::string& rhs) const {
		return (_type == DatumTypes::String && _size == std::size_t(1) && _data.s[0] == rhs);
	}

	inline bool Datum::operator==(const glm::vec4& rhs) const {
		return (_type == DatumTypes::Vector && _size == std::size_t(1) && _data.v[0] == rhs);
	}

	inline bool Datum::operator==(const glm::mat4& rhs) const {
		return (_type == DatumTypes::Matrix && _size == std::size_t(1) && _data.m[0] == rhs);
	}

	inline bool Datum::operator==(const RTTI* rhs) const {
		return (_type == DatumTypes::Pointer && _size == std::size_t(1) && _data.p[0] != nullptr && _data.p[0]->Equals(rhs));
	}

	inline bool Datum::operator!=(const Datum& rhs) const {
		return !(operator==(rhs));
	}

	inline bool Datum::operator!=(const std::int32_t rhs) const {
		return !(operator==(rhs));
	}

	inline bool Datum::operator!=(const float rhs) const {
		return !(operator==(rhs));
	}

	inline bool Datum::operator!=(const std::string& rhs) const {
		return !(operator==(rhs));
	}

	inline bool Datum::operator!=(const glm::vec4& rhs) const {
		return !(operator==(rhs));
	}

	inline bool Datum::operator!=(const glm::mat4& rhs) const {
		return !(operator==(rhs));
	}

	inline bool Datum::operator!=(const RTTI* rhs) const {
		return !(operator==(rhs));
	}
#pragma endregion EqualityOperators
}
