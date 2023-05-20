#include "pch.h"
#include "JsonParseCoordinator.h"

namespace FIEAGameEngine
{
#pragma region Wrapper
	RTTI_DEFINITIONS(Wrapper);

	void Wrapper::Initialize() {
	}

	void Wrapper::Cleanup() {
	}

	void Wrapper::IncrementDepth() {
		++_nestingDepth;
	}

	void Wrapper::DecrementDepth() {
		--_nestingDepth;
	}

	std::size_t Wrapper::Depth() const {
		return _nestingDepth;
	}
#pragma endregion Wrapper

#pragma region ParseCoordinator
	RTTI_DEFINITIONS(JsonParseCoordinator);

	JsonParseCoordinator::JsonParseCoordinator(std::shared_ptr<Wrapper> wrapper) :
		_wrapper{wrapper}
	{
#ifdef USE_EXCEPTIONS
		if (_wrapper == nullptr) throw std::runtime_error("Cannot have a null wrapper.");
#endif // USE_EXCEPTIONS
	}
	
	std::shared_ptr<JsonParseCoordinator> JsonParseCoordinator::Create() const {
		std::shared_ptr<Wrapper> wrapper = _wrapper->Create();
		std::shared_ptr<JsonParseCoordinator> toReturn = std::make_shared<JsonParseCoordinator>(wrapper);

		for (auto helper : _helpers) {
			toReturn->AddHelper(helper->Create());
		}
		return toReturn;
	}

	Vector<std::shared_ptr<IJsonParseHelper>>& JsonParseCoordinator::GetHelpers() {
		return _helpers;
	}

	const Vector<std::shared_ptr<IJsonParseHelper>>& JsonParseCoordinator::GetHelpers() const {
		return _helpers;
	}

	std::shared_ptr<Wrapper> JsonParseCoordinator::GetWrapper() {
		return _wrapper;
	}

	const std::shared_ptr<Wrapper> JsonParseCoordinator::GetWrapper() const {
		return _wrapper;
	}

	bool JsonParseCoordinator::AddHelper(std::shared_ptr<IJsonParseHelper> helper) {
		for (auto h : _helpers) {
			if (h->Is(helper->TypeIdInstance())) {
				return false;
			}
		}

		_helpers.PushBack(helper);
		return true;
	}
	
	bool JsonParseCoordinator::RemoveHelper(std::shared_ptr<IJsonParseHelper> helper) {
		return _helpers.Remove(helper);
	}

	bool JsonParseCoordinator::RemoveHelper(RTTI::IdType helperClass) {
		for (auto helper : _helpers) {
			if (helper->Is(helperClass)) {
				return _helpers.Remove(helper);
			}
		}
		return false;
	}

	void JsonParseCoordinator::SetWrapper(std::shared_ptr<Wrapper> wrapper) {
#ifdef USE_EXCEPTIONS
		if (wrapper == nullptr) throw std::runtime_error("Cannot have a null wrapper.");
#endif // USE_EXCEPTIONS
		_wrapper = wrapper;
	}

	void JsonParseCoordinator::Initialize() {
		_wrapper->Initialize();
		for (auto helper : _helpers) {
			helper->Initialize();
		}
	}

	void JsonParseCoordinator::Cleanup() {
		_wrapper->Cleanup();
		for (auto helper : _helpers) {
			helper->Cleanup();
		}
	}
	
	void JsonParseCoordinator::DeserializeObject(const std::string& jsonString) {
		Json::CharReaderBuilder builder;
		std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
		Json::Value object;
		reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.length(), &object, nullptr);

		Initialize();
		ParseMembers(object);
		Cleanup();
	}
	
	void JsonParseCoordinator::DeserializeObject(std::istream& inStream) {
#ifdef USE_EXCEPTIONS
		if (!inStream.good()) throw std::runtime_error("Cannot deserialize bad data.");
#endif // USE_EXCEPTIONS

		std::string str = "";
		std::ostringstream os;
		os << inStream.rdbuf();
		str = os.str();
		DeserializeObject(str);
	}
	
	void JsonParseCoordinator::DeserializeObjectFromFile(const std::string& fileName) {
		std::ifstream ifs;
		ifs.open(fileName, std::ifstream::in);
		if (ifs.is_open()) {
			DeserializeObject(ifs);
		}
		ifs.close();
	}

	bool JsonParseCoordinator::Equals(const RTTI* rhs) const {
		if (rhs == nullptr) return false;

		const JsonParseCoordinator* other = rhs->As<JsonParseCoordinator>();
		if (other == nullptr) return false;

		bool wrapperEquals = (_wrapper->TypeIdInstance() == other->_wrapper->TypeIdInstance());
		bool helpersEquals = (_helpers.Size() == other->_helpers.Size());
		if (helpersEquals) {
			for (std::size_t i = 0; i < _helpers.Size(); ++i) {
				if (_helpers[i]->TypeIdInstance() != other->_helpers[i]->TypeIdInstance()) {
					helpersEquals = false;
					break;
				}
			}
		}
		
		return (wrapperEquals && helpersEquals);
	}

	std::string JsonParseCoordinator::ToString() const {
		return "Parse Coordinator with wrapper: " + _wrapper->ToString();
	}

	void JsonParseCoordinator::ParseMembers(const Json::Value& jsonObject) {
		for (auto iter = jsonObject.begin(); iter != jsonObject.end(); ++iter) {
			assert(iter.key().isString());
			_wrapper->IncrementDepth();
			Parse(iter.key().asString(), *iter, iter->isArray());
			_wrapper->DecrementDepth();
		}
	}

	void JsonParseCoordinator::Parse(const std::string& jsonKey, const Json::Value& jsonValue, bool isArray) {
		std::shared_ptr<IJsonParseHelper> helperInUse;
		if(!isArray) helperInUse = StartDelegateWork(jsonKey, jsonValue);

		if (isArray) {
			for (auto& val : jsonValue) {
				if (val.isObject()) ParseMembers(val);
				else Parse(jsonKey, val, val.isArray());
			}
		}
		else if(jsonValue.isObject()) {
			ParseMembers(jsonValue);
		}

		if(helperInUse.get()) EndDelegateWork(helperInUse, jsonKey);
	}

	std::shared_ptr<IJsonParseHelper> JsonParseCoordinator::StartDelegateWork(const std::string& jsonKey, const Json::Value& jsonObject) {
		for (auto helper : _helpers) {
			bool helperUsed = helper->StartHandler(*_wrapper, jsonKey, jsonObject);
			if (helperUsed) return helper;
		}
		return std::shared_ptr<IJsonParseHelper>(nullptr);
	}

	void JsonParseCoordinator::EndDelegateWork(std::shared_ptr<IJsonParseHelper> helper, const std::string& jsonKey) {
		bool helperUsed = helper->EndHandler(*_wrapper, jsonKey);
#ifdef _DEBUG
		assert(helperUsed);
#else
		UNREFERENCED_LOCAL(helperUsed);
#endif
	}
#pragma endregion ParseCoordinator
}

