#include "pch.h"
#include "JsonTestParseHelper.h"

namespace UnitTests
{
#pragma region Wrapper
	RTTI_DEFINITIONS(JsonTestParseHelper::Wrapper);

	std::shared_ptr<FIEAGameEngine::Wrapper> JsonTestParseHelper::Wrapper::Create() const {

		return std::make_shared<JsonTestParseHelper::Wrapper>();
	}

	void JsonTestParseHelper::Wrapper::Initialize() {
		initializeCalled = true;
	}

	void JsonTestParseHelper::Wrapper::Cleanup() {
		cleanupCalled = true;
	}
	
	bool JsonTestParseHelper::Wrapper::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const JsonTestParseHelper::Wrapper* other = rhs->As<JsonTestParseHelper::Wrapper>();
		return (other != nullptr && initializeCalled == other->initializeCalled && cleanupCalled == other->cleanupCalled && maxDepth == other->maxDepth);
	}
	
	std::string JsonTestParseHelper::Wrapper::ToString() const {
		return "Test Parse Wrapper, with its max depth at " + std::to_string(maxDepth);
	}
#pragma endregion Wrapper

#pragma region ParseHelper
	RTTI_DEFINITIONS(JsonTestParseHelper);

	std::shared_ptr<FIEAGameEngine::IJsonParseHelper> JsonTestParseHelper::Create() const {
		return std::make_shared<JsonTestParseHelper>();
	}

	void JsonTestParseHelper::Initialize() {
		initializeCalled = true;
	}

	void JsonTestParseHelper::Cleanup() {
		cleanupCalled = true;
	}

	bool JsonTestParseHelper::StartHandler(FIEAGameEngine::Wrapper& wrapper, const std::string&, const Json::Value&) {
		JsonTestParseHelper::Wrapper* testWrapper = wrapper.As<JsonTestParseHelper::Wrapper>();
		if (testWrapper == nullptr) return false;

		if (testWrapper->Depth() >= testWrapper->maxDepth) testWrapper->maxDepth = testWrapper->Depth();
		++startHandlerCount;
		return true;
	}

	bool JsonTestParseHelper::EndHandler(FIEAGameEngine::Wrapper& wrapper, const std::string&) {
		JsonTestParseHelper::Wrapper* testWrapper = wrapper.As<JsonTestParseHelper::Wrapper>();
		if (testWrapper == nullptr) return false;

		++endHandlerCount;
		return true;
	}
	
	bool JsonTestParseHelper::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const JsonTestParseHelper* other = rhs->As<JsonTestParseHelper>();
		return (other != nullptr && initializeCalled == other->initializeCalled && cleanupCalled == other->cleanupCalled &&
			startHandlerCount == other->startHandlerCount && endHandlerCount == other->endHandlerCount);
	}
	
	std::string JsonTestParseHelper::ToString() const {
		return "Test Parse Helper, with its start handler called " + std::to_string(startHandlerCount) + " times, end handler called " + std::to_string(endHandlerCount) + " times.";
	}
#pragma endregion ParseHelper
}
