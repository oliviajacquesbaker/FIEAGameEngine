#include "pch.h"
#include "JsonIntegerParseHelper.h"

namespace UnitTests
{
#pragma region Wrapper
	RTTI_DEFINITIONS(JsonIntegerParseHelper::Wrapper);

	std::shared_ptr<FIEAGameEngine::Wrapper> JsonIntegerParseHelper::Wrapper::Create() const {
		return std::make_shared<JsonIntegerParseHelper::Wrapper>();
	}

	bool JsonIntegerParseHelper::Wrapper::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const JsonIntegerParseHelper::Wrapper* other = rhs->As<JsonIntegerParseHelper::Wrapper>();
		return (other != nullptr && data.Size() == other->data.Size() && std::equal(data.cbegin(), data.cend(), other->data.cbegin()));
	}

	std::string JsonIntegerParseHelper::Wrapper::ToString() const {
		std::string dataStr = "Data:";
		for (auto item : data) {
			dataStr += " " + std::to_string(item);
		}
		return dataStr;
	}
#pragma endregion Wrapper

#pragma region ParseHelper
	RTTI_DEFINITIONS(JsonIntegerParseHelper);

	std::shared_ptr<FIEAGameEngine::IJsonParseHelper> JsonIntegerParseHelper::Create() const {
		return std::make_shared<JsonIntegerParseHelper>();
	}
	
	bool JsonIntegerParseHelper::StartHandler(FIEAGameEngine::Wrapper& wrapper, const std::string&, const Json::Value& jsonObject) {
		JsonIntegerParseHelper::Wrapper* intWrapper = wrapper.As<JsonIntegerParseHelper::Wrapper>();
		if (!isParsing && intWrapper != nullptr && jsonObject.isInt()) {
			int value = jsonObject.asInt();
			intWrapper->data.PushBack(value);
			isParsing = true;
			return true;
		}		

		return false;
	}
	
	bool JsonIntegerParseHelper::EndHandler(FIEAGameEngine::Wrapper& wrapper, const std::string&) {
		JsonIntegerParseHelper::Wrapper* intWrapper = wrapper.As<JsonIntegerParseHelper::Wrapper>();
		if (intWrapper == nullptr) return false;
		isParsing = false;
		return true;
	}

	bool JsonIntegerParseHelper::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const JsonIntegerParseHelper* other = rhs->As<JsonIntegerParseHelper>();
		return (other != nullptr && isParsing == other->isParsing);
	}

	std::string JsonIntegerParseHelper::ToString() const {
		return "Integer Parse Helper. Is currently parsing: " + std::to_string(isParsing);
	}
#pragma endregion ParseHelper
}