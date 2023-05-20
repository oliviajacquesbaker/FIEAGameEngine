#include "pch.h"
#include "JsonTableParseHelper.h"

namespace FIEAGameEngine
{
#pragma region ContextFrame
	JsonTableParseHelper::ContextFrame::ContextFrame(const std::string& key, Scope& root):
		name{key}, root{root}, type{Datum::DatumTypes::Unknown}
	{
	}
#pragma endregion ContextFrame

#pragma region TableWrapper
	RTTI_DEFINITIONS(JsonTableParseHelper::Wrapper);

	JsonTableParseHelper::Wrapper::Wrapper(Scope& root) :
		_root{root}
	{
	}

	std::shared_ptr<FIEAGameEngine::Wrapper> JsonTableParseHelper::Wrapper::Create() const {
		return std::make_shared<JsonTableParseHelper::Wrapper>(_root);
	}

	bool JsonTableParseHelper::Wrapper::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const JsonTableParseHelper::Wrapper* other = rhs->As<JsonTableParseHelper::Wrapper>();
		return (other != nullptr && _root == other->_root);
	}

	std::string JsonTableParseHelper::Wrapper::ToString() const {
		return "Wrapper Data: " + _root.ToString();
	}
#pragma endregion TableWrapper

#pragma region TableHelper
	RTTI_DEFINITIONS(JsonTableParseHelper);

	std::shared_ptr<FIEAGameEngine::IJsonParseHelper> JsonTableParseHelper::Create() const {
		return std::make_shared<JsonTableParseHelper>();
	}

	bool JsonTableParseHelper::StartHandler(FIEAGameEngine::Wrapper& wrapper, const std::string& jsonKey, const Json::Value& jsonObject) {
		JsonTableParseHelper::Wrapper* tableWrapper = wrapper.As<JsonTableParseHelper::Wrapper>();
		if (tableWrapper != nullptr) {
			if (jsonKey == typeKey) {
				Datum::DatumTypes datumType = Datum::stringToType.At(jsonObject.asString());
				_stack.Back().type = datumType;

				if(datumType != Datum::DatumTypes::Table) _stack.Back().root.Append(_stack.Back().name).SetType(datumType);
			}
			else if (jsonKey == valueKey) {
				if (_stack.Back().type == Datum::DatumTypes::Table && _stack.Back().className == "") _stack.Back().root.AppendScope(_stack.Back().name); //has become order dependent :(
				_stack.Back().jsonData.PushBack(std::reference_wrapper<const Json::Value>(jsonObject));
			}
			else if (jsonKey == classKey) {
				_stack.Back().className = jsonObject.asString();
				AppendFromFactory(_stack.Back().className);
			}
			else {
				Scope& context = (_stack.IsEmpty()) ? (tableWrapper->_root) : 
					(_stack.Back().type == Datum::DatumTypes::Table) ? (_stack.Back().root.At(_stack.Back().name).BackAsTable()) : (_stack.Back().root);
				_stack.PushBack(ContextFrame(jsonKey, context));
			}

			return true;
		}
		return false;
	}

	bool JsonTableParseHelper::EndHandler(FIEAGameEngine::Wrapper& wrapper, const std::string& jsonKey) {
		JsonTableParseHelper::Wrapper* tableWrapper = wrapper.As<JsonTableParseHelper::Wrapper>();
		if (tableWrapper == nullptr) return false;

		if (&jsonKey == &(_stack.Back().name)) {
			if (_stack.Back().type != Datum::DatumTypes::Table) {
				Datum& datum = _stack.Back().root.At(jsonKey);
				SetDataFunction setFunction = (datum.IsExternal()) ? _SetFunction[static_cast<std::int32_t>(datum.Type())] :
					_PushFunction[static_cast<std::int32_t>(datum.Type())];

				//There should never be a case where we've reached this far in the parsing and still have an invalid type.
				assert(setFunction != nullptr);
				
				for (std::size_t index = 0; index < _stack.Back().jsonData.Size(); ++index) {
					(this->*setFunction)(datum, _stack.Back().jsonData[index].get(), index);
				}
			}
			_stack.PopBack();
		}

		return true;
	}

	void JsonTableParseHelper::AppendFromFactory(const std::string& className) {
		gsl::owner<Scope*> newScope = Factory<Scope>::Create(className);

#ifdef USE_EXCEPTIONS
		if (newScope == nullptr) throw std::runtime_error("Prescribed class type could not be found.");
#endif // USE_EXCEPTIONS

		_stack.Back().root.Adopt(*newScope, _stack.Back().name);
	}

	bool JsonTableParseHelper::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const JsonTableParseHelper* other = rhs->As<JsonTableParseHelper>();
		return (other != nullptr && _stack.IsEmpty() && other->_stack.IsEmpty());
	}

	inline std::string JsonTableParseHelper::ToString() const {
		return "Table Parse Helper. Currently parsing on layer " + std::to_string(_stack.Size());
	}
#pragma endregion TableHelper
}

