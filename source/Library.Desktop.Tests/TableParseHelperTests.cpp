#include "pch.h"
#include "CppUnitTest.h"
#include "JsonParseCoordinator.h"
#include "JsonTableParseHelper.h"
#include "JsonTestParseHelper.h"
#include "AttributedFoo.h"
#include "Factory.h"
#include "ToStringSpecializations.h"

#pragma warning(push)
#pragma warning(disable:4201)
#include "glm/gtx/string_cast.hpp"
#pragma warning(pop)

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace FIEAGameEngine;
using namespace UnitTests;
using namespace glm;
using namespace std;
using namespace std::string_literals;

namespace Microsoft::VisualStudio::CppUnitTestFramework {
	template<>
	std::wstring ToString<JsonTableParseHelper>(JsonTableParseHelper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonTableParseHelper>(const JsonTableParseHelper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonTableParseHelper::Wrapper>(JsonTableParseHelper::Wrapper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<JsonTableParseHelper::Wrapper>(const JsonTableParseHelper::Wrapper* t) {
		RETURN_WIDE_STRING(t);
	}

	template<>
	std::wstring ToString<Scope>(const Scope& t) {
		std::string stringResult = t.ToString();
		return std::wstring(stringResult.begin(), stringResult.end());
	}

	template<>
	std::wstring ToString<Datum::DatumTypes>(const Datum::DatumTypes& t) {
		std::string stringResult = Datum::typeToString.At(t);
		return std::wstring(stringResult.begin(), stringResult.end());
	}

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
}

namespace LibraryDesktopTests
{
	TEST_CLASS(TableParseHelperTests)
	{
	public:
		TEST_CLASS_INITIALIZE(TestClassInitialize)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();

			RegisterType<AttributedFoo>();
			Factory<Scope>::Add(make_unique<AttributedFooFactory>());
		}

		TEST_CLASS_CLEANUP(TestClassCleanup)
		{
			Factory<Scope>::Clear();
			TypeManager::Clear();
		}

		TEST_METHOD_INITIALIZE(Initialize)
		{
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

		TEST_METHOD(WrapperRTTITests)
		{
			{
				Scope scope;
				shared_ptr<JsonTableParseHelper::Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
				RTTI* rtti = wrapper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTableParseHelper::Wrapper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTableParseHelper::Wrapper::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				JsonTableParseHelper::Wrapper* p = rtti->As<JsonTableParseHelper::Wrapper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(wrapper.get(), p);

				Assert::AreEqual(string("Wrapper Data: "), rtti->ToString());
				wrapper->_root.Append("testKey") = 0;
				Assert::AreEqual(std::string("Wrapper Data: testKey: (0) "), rtti->ToString());

				Scope scope2;
				JsonTableParseHelper::Wrapper P_wrapper(scope2);
				P_wrapper._root.Append("testKey") = 0;
				RTTI* pRtti = &P_wrapper;
				Assert::IsTrue(rtti->Equals(pRtti));

				Scope scopetest;
				RTTI* nope = &scopetest;
				Assert::IsFalse(rtti->Equals(nope));

				RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
			{
				Scope scope;
				shared_ptr<const JsonTableParseHelper::Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
				const RTTI* rtti = wrapper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTableParseHelper::Wrapper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTableParseHelper::Wrapper::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const JsonTableParseHelper::Wrapper* p = rtti->As<JsonTableParseHelper::Wrapper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(wrapper.get(), p);

				Assert::AreEqual(string("Wrapper Data: "), rtti->ToString());

				Scope scope2;
				JsonTableParseHelper::Wrapper P_wrapper(scope2);
				const RTTI* pRtti = &P_wrapper;
				Assert::IsTrue(rtti->Equals(pRtti));

				const RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
		}

		TEST_METHOD(HelperRTTITests)
		{
			{
				shared_ptr<JsonTableParseHelper> helper = make_shared<JsonTableParseHelper>();
				RTTI* rtti = helper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTableParseHelper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTableParseHelper::TypeIdClass(), rtti->TypeIdInstance());

				Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				JsonTableParseHelper* p = rtti->As<JsonTableParseHelper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(helper.get(), p);

				Assert::AreEqual(string("Table Parse Helper. Currently parsing on layer 0"), rtti->ToString());

				JsonTableParseHelper P_helper(*helper);
				RTTI* pRtti = &P_helper;
				Assert::IsTrue(rtti->Equals(pRtti));

				Scope scopetest;
				RTTI* nope = &scopetest;
				Assert::IsFalse(rtti->Equals(nope));

				RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
			{
				shared_ptr<const JsonTableParseHelper> helper = make_shared<JsonTableParseHelper>();
				const RTTI* rtti = helper.get();
				Assert::IsFalse(rtti->Is(Bar::TypeIdClass()));
				Assert::IsTrue(rtti->Is(JsonTableParseHelper::TypeIdClass()));
				Assert::IsTrue(rtti->Is(rtti->TypeIdInstance()));
				Assert::AreEqual(JsonTableParseHelper::TypeIdClass(), rtti->TypeIdInstance());

				const Bar* b = rtti->As<Bar>();
				Assert::IsNull(b);

				const JsonTableParseHelper* p = rtti->As<JsonTableParseHelper>();
				Assert::IsNotNull(p);
				Assert::AreEqual(helper.get(), p);

				Assert::AreEqual(string("Table Parse Helper. Currently parsing on layer 0"), rtti->ToString());

				const JsonTableParseHelper P_helper(*helper);
				const RTTI* pRtti = &P_helper;
				Assert::IsTrue(rtti->Equals(pRtti));

				const RTTI* empty = nullptr;
				Assert::IsFalse(rtti->Equals(empty));
			}
		}

		TEST_METHOD(WrapperCreate)
		{
			Scope scope;
			JsonTableParseHelper::Wrapper tableWrapper(scope);
			tableWrapper._root.Append("hello");
			Assert::AreEqual(size_t(1), tableWrapper._root.Size());

			shared_ptr<Wrapper> wrapper = tableWrapper.Create();
			Assert::AreEqual(tableWrapper.TypeIdInstance(), wrapper->TypeIdInstance());

			JsonTableParseHelper::Wrapper* tableTest = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			Assert::AreEqual(size_t(1), tableTest->_root.Size());
			Assert::IsNotNull(tableTest->_root.Find("hello"));
		}

		TEST_METHOD(HelperCreate)
		{
			JsonTableParseHelper tableHelper;

			shared_ptr<IJsonParseHelper> wrapper = tableHelper.Create();
			Assert::AreEqual(tableHelper.TypeIdInstance(), wrapper->TypeIdInstance());

			JsonTableParseHelper* tableTest = static_cast<JsonTableParseHelper*>(wrapper.get());
			Assert::IsNotNull(tableTest);
		}

		TEST_METHOD(BadHelpers)
		{
			auto badWrapper = JsonTestParseHelper::Wrapper();
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();

			bool handled = testHelper->EndHandler(badWrapper, "nonsense");
			Assert::IsFalse(handled);

			Json::Value value;
			handled = testHelper->StartHandler(badWrapper, "nonsense", value);
			Assert::IsFalse(handled);
		}

		TEST_METHOD(GarbageInGarbageOut)
		{
			const string missingType = R"delim(
			{
				"Health":
				{
					"value": 10
				}
			})delim"s;

			const string badType = R"delim(
			{
				"Health":
				{
					"type": 10
				}
			})delim"s;

			const string badGrammar = R"delim(
			{
				"Health": 10
			})delim"s;

			{
				Scope scope;
				shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
				JsonParseCoordinator parseCoordinator(wrapper);
				shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
				parseCoordinator.AddHelper(testHelper);

				Assert::ExpectException<runtime_error>([&parseCoordinator, &missingType] {parseCoordinator.DeserializeObject(missingType); });
			}
			{
				Scope scope;
				shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
				JsonParseCoordinator parseCoordinator(wrapper);
				shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
				parseCoordinator.AddHelper(testHelper);

				Assert::ExpectException<runtime_error>([&parseCoordinator, &badType] {parseCoordinator.DeserializeObject(badType); });
			}
			{
				Scope scope;
				shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
				JsonParseCoordinator parseCoordinator(wrapper);
				shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
				parseCoordinator.AddHelper(testHelper);

				Assert::ExpectException<runtime_error>([&parseCoordinator, &badGrammar] {parseCoordinator.DeserializeObject(badGrammar); });
			}
		}

		TEST_METHOD(DeserializationPrimitives)
		{
			const string inputString = R"delim(
			{
				"Health":
				{
					"type": "Integer",
					"value": 10
				},
				"Damage":
				{
					"type": "Float",
					"value": 1.5
				},
				"Name":
				{
					"type": "String",
					"value": "pip"
				},
				"Velocity":
				{
					"type": "Vector",
					"value": "vec4(1.000000, 2.000000, 3.000000, 4.000000)"
				},
				"Transform":
				{
					"type": "Matrix",
					"value": "mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))"
				}
			})delim"s;

			Scope scope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(scope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(5), scope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(1), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(1), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(1), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(1), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(1), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
		}

		TEST_METHOD(DeserializationPrimitivesExternal)
		{
			const string inputString = R"delim(
			{
				"Health":
				{
					"type": "Integer",
					"value": 10
				},
				"Damage":
				{
					"type": "Float",
					"value": 1.5
				},
				"Name":
				{
					"type": "String",
					"value": "pip"
				},
				"Velocity":
				{
					"type": "Vector",
					"value": "vec4(1.000000, 2.000000, 3.000000, 4.000000)"
				},
				"Transform":
				{
					"type": "Matrix",
					"value": "mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))"
				}
			})delim"s;

			Scope scope;
			int a = -5;
			float b = 0.005f;
			string c = "wow";
			vec4 d = vec4(9, 9, 9, 11);
			mat4 e = mat4(111);

			scope.Append("Health").SetStorage(&a, size_t(1));
			scope.Append("Damage").SetStorage(&b, size_t(1));
			scope.Append("Name").SetStorage(&c, size_t(1));
			scope.Append("Velocity").SetStorage(&d, size_t(1));
			scope.Append("Transform").SetStorage(&e, size_t(1));
			Assert::AreEqual(size_t(5), scope.Size());

			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(scope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(5), scope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(1), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(1), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(1), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(1), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(1), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
		}

		TEST_METHOD(DeserializationPreexistingPrimitiveArrays)
		{
			const string inputString = R"delim(
			{
				"Health":
				{
					"type": "Integer",
					"value": [20, 30]
				},
				"Damage":
				{
					"type": "Float",
					"value": [2.0]
				},
				"Name":
				{
					"type": "String",
					"value": ["owo", "uwu"]
				},
				"Velocity":
				{
					"type": "Vector",
					"value": ["vec4(2.000000, 2.000000, 2.000000, 2.000000)", "vec4(4.000000, 3.000000, 2.000000, 1.000000)"]
				},
				"Transform":
				{
					"type": "Matrix",
					"value": ["mat4x4((9.000000, 0.000000, 0.000000, 0.000000), (0.000000, 9.000000, 0.000000, 0.000000), (0.000000, 0.000000, 9.000000, 0.000000), (0.000000, 0.000000, 0.000000, 9.000000))",
								"mat4x4((5.000000, 0.000000, 0.000000, 0.000000), (0.000000, 5.000000, 0.000000, 0.000000), (0.000000, 0.000000, 5.000000, 0.000000), (0.000000, 0.000000, 0.000000, 5.000000))"]
				}
			})delim"s;

			Scope scope;
			scope.Append("Health") = 10;
			scope.Append("Damage") = 1.5f;
			scope.Append("Name") = "pip"s;
			scope.Append("Velocity") = vec4(1, 2, 3, 4);
			scope.Append("Transform") = mat4(1);
			Assert::AreEqual(size_t(5), scope.Size());
			Assert::AreEqual(size_t(1), scope.At("Health").Size());
			Assert::AreEqual(size_t(1), scope.At("Damage").Size());
			Assert::AreEqual(size_t(1), scope.At("Name").Size());
			Assert::AreEqual(size_t(1), scope.At("Velocity").Size());
			Assert::AreEqual(size_t(1), scope.At("Transform").Size());

			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(scope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(5), scope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(3), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());
			Assert::AreEqual(20, scope.At("Health").GetAsInt(1));
			Assert::AreEqual(30, scope.At("Health").GetAsInt(2));

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(2), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());
			Assert::AreEqual(2.0f, scope.At("Damage").GetAsFloat(1));

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(3), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());
			Assert::AreEqual("owo"s, scope.At("Name").GetAsString(1));
			Assert::AreEqual("uwu"s, scope.At("Name").GetAsString(2));

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(3), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());
			Assert::AreEqual(vec4(2), scope.At("Velocity").GetAsVector(1));
			Assert::AreEqual(vec4(4, 3, 2, 1), scope.At("Velocity").GetAsVector(2));

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(3), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
			Assert::AreEqual(mat4(9), scope.At("Transform").GetAsMatrix(1));
			Assert::AreEqual(mat4(5), scope.At("Transform").GetAsMatrix(2));
		}

		TEST_METHOD(DeserializationPrimitiveArrays)
		{
			const string inputString = R"delim(
			{
				"Health":
				{
					"type": "Integer",
					"value": [10, 20, 30]
				},
				"Damage":
				{
					"type": "Float",
					"value": [1.5, 2.0]
				},
				"Name":
				{
					"type": "String",
					"value": ["pip", "owo", "uwu"]
				},
				"Velocity":
				{
					"type": "Vector",
					"value": ["vec4(1.000000, 2.000000, 3.000000, 4.000000)", "vec4(2.000000, 2.000000, 2.000000, 2.000000)", "vec4(4.000000, 3.000000, 2.000000, 1.000000)"]
				},
				"Transform":
				{
					"type": "Matrix",
					"value": ["mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))",
								"mat4x4((9.000000, 0.000000, 0.000000, 0.000000), (0.000000, 9.000000, 0.000000, 0.000000), (0.000000, 0.000000, 9.000000, 0.000000), (0.000000, 0.000000, 0.000000, 9.000000))",
								"mat4x4((5.000000, 0.000000, 0.000000, 0.000000), (0.000000, 5.000000, 0.000000, 0.000000), (0.000000, 0.000000, 5.000000, 0.000000), (0.000000, 0.000000, 0.000000, 5.000000))"]
				}
			})delim"s;

			Scope scope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(scope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(scope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(5), scope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(3), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());
			Assert::AreEqual(20, scope.At("Health").GetAsInt(1));
			Assert::AreEqual(30, scope.At("Health").GetAsInt(2));

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(2), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());
			Assert::AreEqual(2.0f, scope.At("Damage").GetAsFloat(1));

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(3), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());
			Assert::AreEqual("owo"s, scope.At("Name").GetAsString(1));
			Assert::AreEqual("uwu"s, scope.At("Name").GetAsString(2));

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(3), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());
			Assert::AreEqual(vec4(2), scope.At("Velocity").GetAsVector(1));
			Assert::AreEqual(vec4(4, 3, 2, 1), scope.At("Velocity").GetAsVector(2));

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(3), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
			Assert::AreEqual(mat4(9), scope.At("Transform").GetAsMatrix(1));
			Assert::AreEqual(mat4(5), scope.At("Transform").GetAsMatrix(2));
		}

		TEST_METHOD(DeserializationNestedEmptyScope)
		{
			const string inputString = R"delim(
			{
				"DummyVariable" :
				{
					"type": "Integer",
					"value": 19
				},
				"Character" :
				{
					"type": "Table",
					"value":
					{
						
					}
				}
				
			})delim"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(2), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, parentScope.At("DummyVariable").Type());
			Assert::AreEqual(size_t(1), parentScope.At("DummyVariable").Size());
			Assert::AreEqual(19, parentScope.At("DummyVariable").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("Character").Type());
			Assert::AreEqual(size_t(1), parentScope.At("Character").Size());

			const Scope& scope = parentScope.At("Character").GetAsTable();
			Assert::AreEqual(size_t(0), scope.Size());
		}

		TEST_METHOD(DeserializationNestedScope)
		{
			const string inputString = R"delim(
			{
				"DummyVariable" :
				{
					"type": "Integer",
					"value": 19
				},
				"Character" :
				{
					"type": "Table",
					"value":
					{
						"Health":
						{
							"type": "Integer",
							"value": 10
						},
						"Damage":
						{
							"type": "Float",
							"value": 1.5
						},
						"Name":
						{
							"type": "String",
							"value": "pip"
						},
						"Velocity":
						{
							"type": "Vector",
							"value": "vec4(1.000000, 2.000000, 3.000000, 4.000000)"
						},
						"Transform":
						{
							"type": "Matrix",
							"value": "mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))"
						}
					}
				}
				
			})delim"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(2), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, parentScope.At("DummyVariable").Type());
			Assert::AreEqual(size_t(1), parentScope.At("DummyVariable").Size());
			Assert::AreEqual(19, parentScope.At("DummyVariable").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("Character").Type());
			Assert::AreEqual(size_t(1), parentScope.At("Character").Size());

			const Scope& scope = parentScope.At("Character").GetAsTable();
			Assert::AreEqual(size_t(5), scope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(1), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(1), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(1), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(1), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(1), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
		}

		TEST_METHOD(DeserializationNestedClassType)
		{
			const string inputString = R"delim(
			{
				"DummyVariable" :
				{
					"type": "Integer",
					"value": 19
				},
				"CharacterFoo" :
				{
					"type": "Table",
					"class" : "AttributedFoo",
					"value":
					{
						"externalInteger":
						{
							"type": "Integer",
							"value": 10
						},
						"Health":
						{
							"type": "Integer",
							"value": 10
						},
						"Damage":
						{
							"type": "Float",
							"value": 1.5
						},
						"Name":
						{
							"type": "String",
							"value": "pip"
						},
						"Velocity":
						{
							"type": "Vector",
							"value": "vec4(1.000000, 2.000000, 3.000000, 4.000000)"
						},
						"Transform":
						{
							"type": "Matrix",
							"value": "mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))"
						}
					}
				}
				
			})delim"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(2), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, parentScope.At("DummyVariable").Type());
			Assert::AreEqual(size_t(1), parentScope.At("DummyVariable").Size());
			Assert::AreEqual(19, parentScope.At("DummyVariable").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("CharacterFoo").Type());
			Assert::AreEqual(size_t(1), parentScope.At("CharacterFoo").Size());

			const Scope& scope = parentScope.At("CharacterFoo").GetAsTable();
			Assert::AreEqual(size_t(20), scope.Size()); //"this", 14 prescribed attributes, 5 added ones from the json

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("externalInteger").Type());
			Assert::AreEqual(size_t(1), scope.At("externalInteger").Size());
			Assert::AreEqual(10, scope.At("externalInteger").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("externalVector").Type());
			Assert::AreEqual(size_t(1), scope.At("externalVector").Size());
			Assert::AreEqual(vec4(0), scope.At("externalVector").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(1), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(1), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(1), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(1), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(1), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
		}

		TEST_METHOD(DeserializationNestedNestedScope)
		{
			const string inputString = R"delim(
			{
				"DummyVariable" :
				{
					"type": "Integer",
					"value": 19
				},
				"Character" :
				{
					"type": "Table",
					"value":
					{
						"DummyVariable" :
						{
							"type": "Integer",
							"value": 20
						},
						"AnotherChild" :
						{
							"type": "Table",
							"value": 
							{
									"Health":
								{
									"type": "Integer",
									"value": 10
								},
								"Damage":
								{
									"type": "Float",
									"value": 1.5
								},
								"Name":
								{
									"type": "String",
									"value": "pip"
								},
								"Velocity":
								{
									"type": "Vector",
									"value": "vec4(1.000000, 2.000000, 3.000000, 4.000000)"
								},
								"Transform":
								{
									"type": "Matrix",
									"value": "mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))"
								}
							}
						}	
						
					}
				}
				
			})delim"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(2), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, parentScope.At("DummyVariable").Type());
			Assert::AreEqual(size_t(1), parentScope.At("DummyVariable").Size());
			Assert::AreEqual(19, parentScope.At("DummyVariable").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("Character").Type());
			Assert::AreEqual(size_t(1), parentScope.At("Character").Size());

			const Scope& middleScope = parentScope.At("Character").GetAsTable();
			Assert::AreEqual(size_t(2), middleScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, middleScope.At("DummyVariable").Type());
			Assert::AreEqual(size_t(1), middleScope.At("DummyVariable").Size());
			Assert::AreEqual(20, middleScope.At("DummyVariable").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Table, middleScope.At("AnotherChild").Type());
			Assert::AreEqual(size_t(1), middleScope.At("AnotherChild").Size());
			const Scope& scope = middleScope.At("AnotherChild").GetAsTable();
			Assert::AreEqual(size_t(5), scope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(1), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(1), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(1), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(1), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(1), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
		}

		TEST_METHOD(DeserializationNestedNestedClassType)
		{
			const string inputString = R"delim(
			{
				"DummyVariable" :
				{
					"type": "Integer",
					"value": 19
				},
				"Character" :
				{
					"type": "Table",
					"value":
					{
						"DummyVariable" :
						{
							"type": "Integer",
							"value": 20
						},
						"AnotherChild" :
						{
							"type": "Table",
							"class": "AttributedFoo",
							"value": 
							{
								"Health":
								{
									"type": "Integer",
									"value": 10
								},
								"externalInteger":
								{
									"type": "Integer",
									"value": 10
								},
								"Damage":
								{
									"type": "Float",
									"value": 1.5
								},
								"Name":
								{
									"type": "String",
									"value": "pip"
								},
								"Velocity":
								{
									"type": "Vector",
									"value": "vec4(1.000000, 2.000000, 3.000000, 4.000000)"
								},
								"Transform":
								{
									"type": "Matrix",
									"value": "mat4x4((1.000000, 0.000000, 0.000000, 0.000000), (0.000000, 1.000000, 0.000000, 0.000000), (0.000000, 0.000000, 1.000000, 0.000000), (0.000000, 0.000000, 0.000000, 1.000000))"
								}
							}
						}	
						
					}
				}
				
			})delim"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			parseCoordinator.DeserializeObject(inputString);
			Assert::AreEqual(size_t(2), parentScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, parentScope.At("DummyVariable").Type());
			Assert::AreEqual(size_t(1), parentScope.At("DummyVariable").Size());
			Assert::AreEqual(19, parentScope.At("DummyVariable").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Table, parentScope.At("Character").Type());
			Assert::AreEqual(size_t(1), parentScope.At("Character").Size());

			const Scope& middleScope = parentScope.At("Character").GetAsTable();
			Assert::AreEqual(size_t(2), middleScope.Size());

			Assert::AreEqual(Datum::DatumTypes::Integer, middleScope.At("DummyVariable").Type());
			Assert::AreEqual(size_t(1), middleScope.At("DummyVariable").Size());
			Assert::AreEqual(20, middleScope.At("DummyVariable").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Table, middleScope.At("AnotherChild").Type());
			Assert::AreEqual(size_t(1), middleScope.At("AnotherChild").Size());
			const Scope& scope = middleScope.At("AnotherChild").GetAsTable();
			Assert::AreEqual(size_t(20), scope.Size());

			Assert::IsTrue(scope.At("externalInteger").IsExternal());
			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("externalInteger").Type());
			Assert::AreEqual(size_t(1), scope.At("externalInteger").Size());
			Assert::AreEqual(10, scope.At("externalInteger").FrontAsInt());

			Assert::IsTrue(scope.At("externalVector").IsExternal());
			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("externalVector").Type());
			Assert::AreEqual(size_t(1), scope.At("externalVector").Size());
			Assert::AreEqual(vec4(0), scope.At("externalVector").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Integer, scope.At("Health").Type());
			Assert::AreEqual(size_t(1), scope.At("Health").Size());
			Assert::AreEqual(10, scope.At("Health").FrontAsInt());

			Assert::AreEqual(Datum::DatumTypes::Float, scope.At("Damage").Type());
			Assert::AreEqual(size_t(1), scope.At("Damage").Size());
			Assert::AreEqual(1.5f, scope.At("Damage").FrontAsFloat());

			Assert::AreEqual(Datum::DatumTypes::String, scope.At("Name").Type());
			Assert::AreEqual(size_t(1), scope.At("Name").Size());
			Assert::AreEqual("pip"s, scope.At("Name").FrontAsString());

			Assert::AreEqual(Datum::DatumTypes::Vector, scope.At("Velocity").Type());
			Assert::AreEqual(size_t(1), scope.At("Velocity").Size());
			Assert::AreEqual(vec4(1, 2, 3, 4), scope.At("Velocity").FrontAsVector());

			Assert::AreEqual(Datum::DatumTypes::Matrix, scope.At("Transform").Type());
			Assert::AreEqual(size_t(1), scope.At("Transform").Size());
			Assert::AreEqual(mat4(1), scope.At("Transform").FrontAsMatrix());
		}

		TEST_METHOD(DeserializationFailedClassType)
		{
			const string inputString = R"delim(
			{
				"DummyVariable" :
				{
					"type": "Integer",
					"value": 19
				},
				"Character" :
				{
					"type": "Table",
					"class" : "Blegh",
					"value":
					{
						"Health":
						{
							"type": "Integer",
							"value": 10
						}
					}
				}
				
			})delim"s;

			Scope parentScope;
			shared_ptr<Wrapper> wrapper = make_shared<JsonTableParseHelper::Wrapper>(parentScope);
			JsonTableParseHelper::Wrapper* rawWrapper = static_cast<JsonTableParseHelper::Wrapper*>(wrapper.get());
			JsonParseCoordinator parseCoordinator(wrapper);
			shared_ptr<JsonTableParseHelper> testHelper = make_shared<JsonTableParseHelper>();
			parseCoordinator.AddHelper(testHelper);

			Assert::AreEqual(parentScope, rawWrapper->_root);

			Assert::ExpectException<runtime_error>([&parseCoordinator, &inputString] {parseCoordinator.DeserializeObject(inputString); });			
		}

		TEST_METHOD(ContextFrameCopyMove)
		{
			Scope scope;
			scope.Append("hi") = "hello"s;
			string name = "test";
			JsonTableParseHelper::ContextFrame frame(name, scope);

			JsonTableParseHelper::ContextFrame frameCopy = frame;
			Assert::AreSame(frame.root, frameCopy.root);
			Assert::AreSame(frame.name, frameCopy.name);
			Assert::AreEqual(frame.type, frameCopy.type);
			Assert::AreEqual(frame.jsonData.Size(), frameCopy.jsonData.Size());

			JsonTableParseHelper::ContextFrame frameCopy2 = std::move(frameCopy);
			Assert::AreSame(scope, frameCopy2.root);
			Assert::AreSame(name, frameCopy2.name);
			Assert::AreEqual(frame.type, frameCopy2.type);
			Assert::AreEqual(frame.jsonData.Size(), frameCopy2.jsonData.Size());
		}

		TEST_METHOD(WrapperCopyMove)
		{
			Scope scope;
			JsonTableParseHelper::Wrapper wrapper(scope);

			JsonTableParseHelper::Wrapper wrapperCopy = wrapper;
			Assert::AreSame(wrapper._root, wrapperCopy._root);

			JsonTableParseHelper::Wrapper wrapperCopy2 = std::move(wrapperCopy);
			Assert::AreSame(wrapper._root, wrapperCopy2._root);
		}

	private:
		inline static _CrtMemState _startMemState;
	};
}