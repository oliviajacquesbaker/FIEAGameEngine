/// <summary>
/// The definitions of the templated functions for the abstract base Factory.
/// </summary>

#pragma once

#include "Factory.h"

namespace FIEAGameEngine {
	template <typename ProductType>
	inline const Factory<ProductType>* Factory<ProductType>::Find(const std::string& className) {
		auto factoryIter = _factoryLookup.Find(className);
		return (factoryIter == _factoryLookup.end()) ? nullptr : factoryIter->second.get();
	}
	
	template<typename ProductType>
	inline gsl::owner<ProductType*> Factory<ProductType>::Create(const std::string& className) {
		const Factory<ProductType>* concreteFactory = Find(className);

		if (!concreteFactory) return nullptr;
		else return concreteFactory->Create();
	}
	
	template<typename ProductType>
	inline std::size_t Factory<ProductType>::Size() {
		return _factoryLookup.Size();
	}

	template<typename ProductType>
	inline void Factory<ProductType>::Add(std::unique_ptr<Factory<ProductType>> factory) {
		auto[ iter, inserted ] = _factoryLookup.Insert({ factory->ClassName(), std::move(factory) });
		UNREFERENCED_LOCAL(iter);
		assert(inserted);
	}

	template<typename ProductType>
	inline void Factory<ProductType>::Remove(const std::string& className) {
		_factoryLookup.Remove(_factoryLookup.Find(className));
	}
	
	template<typename ProductType>
	inline void Factory<ProductType>::Clear() {
		_factoryLookup.Clear();
	}
}