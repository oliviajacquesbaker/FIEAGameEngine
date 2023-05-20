/// <summary>
/// The declaration of the abstract base Factory template, as well as the macro to produce concrete factories.
/// </summary>

#pragma once

#include <cstddef>
#include <cassert>
#include "RTTI.h"
#include "HashMap.h"

namespace FIEAGameEngine {
	template <typename ProductType>
	class Factory { 

	public:
		/// <summary>
		/// Default constructor for a Factory. If no size is given, it will default to zero.
		/// </summary>
		Factory() = default;
		/// <summary>
		/// A copy constructor for a Factory that performs a deep copy during construction.
		/// </summary>
		/// <param name="other">The Factory to copy</param>
		Factory(const Factory& rhs) = delete;
		/// <summary>
		/// The move constructor for Factory.
		/// </summary>
		/// <param name="rhs">The Factory to move data from</param>
		Factory(Factory&& rhs) noexcept = delete;
		/// <summary>
		/// The copy assignment operator for an Factory that first clears the existing elements and then performs a deep copy.
		/// </summary>
		/// <param name="rhs">The Factory to copy</param>
		/// <returns>A reference to the updated Factory</returns>
		Factory& operator=(const Factory& rhs) = delete;
		/// <summary>
		/// The move assignment operator for a Factory that first clears the existing elements and then performs a move assignment.
		/// </summary>
		/// <param name="rhs">The Factory to copy</param>
		/// <returns>A reference to the updated Factory</returns>
		Factory& operator=(Factory&& rhs) noexcept = delete;
		/// <summary>
		/// The destructor for a Factory.
		/// </summary>
		virtual ~Factory() = default;

		/// <summary>
		/// Find factory to produce items with the given class name.
		/// </summary>
		/// <param name="className">The class name to look for a factory for.</param>
		/// <returns>A pointer to the found factory, or nullptr if one doesn't exist.</returns>
		static const Factory<ProductType>* Find(const std::string& className);
		/// <summary>
		/// Convenience function to create an object from the static scope based on the given name.
		/// </summary>
		/// <param name="className">The name of the class to create.</param>
		/// <returns>A gsl owner with a pointer to the created object.</returns>
		static gsl::owner<ProductType*> Create(const std::string& className); 

		/// <summary>
		/// Create an object from this concrete factory.
		/// </summary>
		/// <returns>A gsl owner with a pointer to the created object.</returns>
		virtual gsl::owner<ProductType*> Create() const = 0;
		/// <summary>
		/// Get the class name of the object this factory produces.
		/// </summary>
		/// <returns>The class name of the object this factory produces.</returns>
		virtual std::string ClassName() const = 0;

		/// <summary>
		/// Get how many factories are registered in this abstract base type factory manager.
		/// </summary>
		/// <returns>The number of registered factories.</returns>
		static std::size_t Size();
		/// <summary>
		/// Add a new factory to the factory "manager" - will not add something that has already been added.
		/// </summary>
		/// <param name="factory">The factory to add.</param>
		static void Add(std::unique_ptr<Factory<ProductType>> factory); 
		/// <summary>
		/// Given a class name, remove that factory from this factory "manager"
		/// </summary>
		/// <param name="className">The name of the class associated with the factory you want to remove.</param>
		static void Remove(const std::string& className);
		/// <summary>
		/// Clear all registered factories from the base "manager".
		/// </summary>
		static void Clear();

	private:
		/// <summary>
		/// All the factories registered, associated with the name of the class they produce for.
		/// </summary>
		inline static HashMap<std::string, std::unique_ptr<const Factory>> _factoryLookup = HashMap<std::string, std::unique_ptr<const Factory<ProductType>>>();
	};
}

/// <summary>
/// A macro to produce concrete factory classes.
/// </summary>
#define CONCRETE_FACTORY(ConcreteProductType, AbstractProductBase)															\
class ConcreteProductType##Factory final : public FIEAGameEngine::Factory<AbstractProductBase> {							\
	public:																													\
		virtual gsl::owner<AbstractProductBase*> Create() const override {													\
			return new ConcreteProductType();					 															\
		}																													\
		virtual std::string ClassName() const override {																	\
			return std::string(#ConcreteProductType);																		\
		}																													\
};																															\
	

#include "Factory.inl"