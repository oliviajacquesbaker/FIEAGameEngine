/// <summary>
/// The fully specified implementation of the tempated struct DefaultHash and some of its specializations.
/// </summary>

#pragma once
#include "DefaultHash.h"

namespace FIEAGameEngine {
	template<typename TKey>
	inline std::size_t DefaultHash<TKey>::operator()(const TKey& key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;		
		
		const std::byte* data = reinterpret_cast<const std::byte*>(&key);
		assert(data != nullptr);

		for (std::size_t index = 0; index < sizeof(TKey); ++index) {
			hashValue += static_cast<std::size_t>(data[index]) * hashPrime;
		}

		return hashValue;
	}

	inline std::size_t DefaultHash<std::string>::operator()(const std::string& key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (const char& letter : key) {
			hashValue += static_cast<std::size_t>(letter)*hashPrime;
		}
		return hashValue;
	}

	inline std::size_t DefaultHash<const std::string>::operator()(const std::string& key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (const char& letter : key) {
			hashValue += static_cast<std::size_t>(letter)*hashPrime;
		}
		return hashValue;
	}

	inline std::size_t DefaultHash<std::wstring>::operator()(const std::wstring& key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (const wchar_t& letter : key) {
			hashValue += static_cast<std::size_t>(letter) * hashPrime;
		}
		return hashValue;
	}

	inline std::size_t DefaultHash<const std::wstring>::operator()(const std::wstring& key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (const wchar_t& letter : key) {
			hashValue += static_cast<std::size_t>(letter) * hashPrime;
		}
		return hashValue;
	}

	inline std::size_t DefaultHash<char*>::operator()(const char* key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (std::size_t index = 0; index < strlen(key); ++index) {
			hashValue += static_cast<std::size_t>(key[index]) * hashPrime;
		}

		return hashValue;
	}

	inline std::size_t DefaultHash<const char*>::operator()(const char* key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (std::size_t index = 0; index < strlen(key); ++index) {
			hashValue += static_cast<std::size_t>(key[index]) * hashPrime;
		}

		return hashValue;
	}

	inline std::size_t DefaultHash<char* const>::operator()(const char* const key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (std::size_t index = 0; index < strlen(key); ++index) {
			hashValue += static_cast<std::size_t>(key[index]) * hashPrime;
		}

		return hashValue;
	}

	inline std::size_t DefaultHash<const char* const>::operator()(const char* const key) const {
		const std::size_t hashPrime = 11;
		std::size_t hashValue = 0;

		for (std::size_t index = 0; index < strlen(key); ++index) {
			hashValue += static_cast<std::size_t>(key[index]) * hashPrime;
		}

		return hashValue;
	}
}