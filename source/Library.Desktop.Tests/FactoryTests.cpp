/// <summary>
/// Unit tests exercising the functionality of DefaultHash and its specializations
/// </summary>

#include "pch.h"
#include "CppUnitTest.h"
#include "ToStringSpecializations.h"
#include "Factory.h"
#include "AttributedFoo.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace UnitTests;
using namespace FIEAGameEngine;
using namespace std;
using namespace glm;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<vec4>(const vec4& t) {
		std::string stringResult = glm::to_string(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<mat4>(const mat4& t) {
		std::string stringResult = glm::to_string(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Scope>(const Scope& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}
}

namespace LibraryDesktopTests
{
	TEST_CLASS(FactoryTests)
	{
	public:

		TEST_METHOD_INITIALIZE(Initialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();

#if defined(DEBUG) || defined(_DEBUG)
			_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF);
			_CrtMemCheckpoint(&_startMemState);
#endif
		}

		TEST_METHOD_CLEANUP(Cleanup)
		{
#if defined(DEBUG) || defined(_DEBUG)
			_CrtMemState endMemState, diffMemState;
			_CrtMemCheckpoint(&endMemState);
			if (_CrtMemDifference(&diffMemState, &_startMemState, &endMemState))
			{
				_CrtMemDumpStatistics(&diffMemState);
				Assert::Fail(L"Memory Leaks!");
			}
#endif
		}

		TEST_METHOD(Find)
		{
			RegisterType<AttributedFoo>();
			Factory<Scope>::Add(make_unique<AttributedFooFactory>());

			Assert::IsNotNull(Factory<Scope>::Find("AttributedFoo"));
			Assert::IsNull(Factory<Scope>::Find("AttributedFooFactory"));
			Assert::IsNull(Factory<Scope>::Find("A"));

			auto factory = Factory<Scope>::Find("AttributedFoo");
			Assert::AreEqual("AttributedFoo"s, factory->ClassName());

			gsl::owner<Scope*> createdScope = factory->Create();
			Assert::IsNotNull(createdScope);
			Assert::IsTrue(createdScope->Is(AttributedFoo::TypeIdClass()));

			//prevent memory leaks
			delete createdScope;
			Factory<Scope>::Clear();
			TypeManager::Clear();
		}

		TEST_METHOD(Creates)
		{
			RegisterType<AttributedFoo>();
			Factory<Scope>::Add(make_unique<AttributedFooFactory>());

			auto factory = Factory<Scope>::Find("AttributedFoo");
			Assert::AreEqual("AttributedFoo"s, factory->ClassName());

			gsl::owner<Scope*> createdScope = factory->Create();
			Assert::IsNotNull(createdScope);
			Assert::IsTrue(createdScope->Is(AttributedFoo::TypeIdClass()));
			Assert::AreEqual(size_t(15), createdScope->Size());
			Assert::AreEqual(0, createdScope->At("externalInteger").GetAsInt());
			Assert::AreEqual(0.0f, createdScope->At("externalFloat").GetAsFloat());
			Assert::AreEqual(""s, createdScope->At("externalString").GetAsString());
			Assert::AreEqual(vec4(), createdScope->At("externalVector").GetAsVector());
			Assert::AreEqual(mat4(), createdScope->At("externalMatrix").GetAsMatrix());
			Assert::IsNull(createdScope->At("externalPointer").GetAsPointer());

			gsl::owner<Scope*> createdScope2 = Factory<Scope>::Create("AttributedFoo");

			Assert::IsNotNull(createdScope2);
			Assert::IsTrue(createdScope2->Is(AttributedFoo::TypeIdClass()));
			Assert::AreEqual(size_t(15), createdScope2->Size());
			Assert::AreEqual(0, createdScope2->At("externalInteger").GetAsInt());
			Assert::AreEqual(0.0f, createdScope2->At("externalFloat").GetAsFloat());
			Assert::AreEqual(""s, createdScope2->At("externalString").GetAsString());
			Assert::AreEqual(vec4(), createdScope2->At("externalVector").GetAsVector());
			Assert::AreEqual(mat4(), createdScope2->At("externalMatrix").GetAsMatrix());
			Assert::IsNull(createdScope2->At("externalPointer").GetAsPointer());

			Assert::AreEqual(*createdScope, *createdScope2);

			Assert::IsNull( Factory<Scope>::Create("Blah")); 

			//prevent memory leaks
			delete createdScope;
			delete createdScope2;
			Factory<Scope>::Clear();
			TypeManager::Clear();
		}

		TEST_METHOD(AddAndRemove)
		{
			Assert::AreEqual(size_t(0), Factory<RTTI>::Size());
			const Factory<RTTI>* foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNull(foundFactory);

			Factory<RTTI>::Add(make_unique<FooFactory>());
			Assert::AreEqual(size_t(1), Factory<RTTI>::Size());
			foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNotNull(foundFactory);
			Factory<RTTI>::Remove("Foo"s);
			foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNull(foundFactory);

			Assert::AreEqual(size_t(0), Factory<RTTI>::Size());
			foundFactory = Factory<RTTI>::Find("Bar"s);
			Assert::IsNull(foundFactory);

			Factory<RTTI>::Add(make_unique<BarFactory>());
			Assert::AreEqual(size_t(1), Factory<RTTI>::Size());
			foundFactory = Factory<RTTI>::Find("Bar"s);
			Assert::IsNotNull(foundFactory);
			Factory<RTTI>::Remove("Bar"s);
			foundFactory = Factory<RTTI>::Find("Bar"s);
			Assert::IsNull(foundFactory);

			Assert::AreEqual(size_t(0), Factory<RTTI>::Size());
		}

		TEST_METHOD(Clear)
		{
			Assert::AreEqual(size_t(0), Factory<RTTI>::Size());
			const Factory<RTTI>* foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNull(foundFactory);

			Factory<RTTI>::Add(make_unique<FooFactory>());
			Assert::AreEqual(size_t(1), Factory<RTTI>::Size());
			foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNotNull(foundFactory);
			Factory<RTTI>::Clear();
			foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNull(foundFactory);

			Assert::AreEqual(size_t(0), Factory<RTTI>::Size());
			foundFactory = Factory<RTTI>::Find("Bar"s);
			Assert::IsNull(foundFactory);

			Factory<RTTI>::Add(make_unique<BarFactory>());
			foundFactory = Factory<RTTI>::Find("Bar"s);
			Assert::IsNotNull(foundFactory);
			Factory<RTTI>::Add(make_unique<FooFactory>());
			foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNotNull(foundFactory);
			Assert::AreEqual(size_t(2), Factory<RTTI>::Size());

			Factory<RTTI>::Clear();

			foundFactory = Factory<RTTI>::Find("Bar"s);
			Assert::IsNull(foundFactory);
			foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::IsNull(foundFactory);

			Assert::AreEqual(size_t(0), Factory<RTTI>::Size());
		}

		TEST_METHOD(ClassName)
		{
			Factory<RTTI>::Add(make_unique<FooFactory>());
			Factory<RTTI>::Add(make_unique<BarFactory>());
			Factory<Scope>::Add(make_unique<AttributedFooFactory>());
			Assert::AreEqual(size_t(2), Factory<RTTI>::Size());
			Assert::AreEqual(size_t(1), Factory<Scope>::Size());

			auto foundFactory = Factory<RTTI>::Find("Foo"s);
			Assert::AreEqual("Foo"s, foundFactory->ClassName());
			foundFactory = Factory<RTTI>::Find("Bar"s);
			Assert::AreEqual("Bar"s, foundFactory->ClassName());

			auto foundFactory2 = Factory<Scope>::Find("AttributedFoo"s);
			Assert::AreEqual("AttributedFoo"s, foundFactory2->ClassName());

			Factory<RTTI>::Clear();
			Factory<Scope>::Clear();
			TypeManager::Clear();
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}