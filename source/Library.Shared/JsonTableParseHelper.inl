#pragma once
#include "JsonTableParseHelper.h"

namespace FIEAGameEngine
{
	inline void JsonTableParseHelper::SetIntData(Datum& datum, const Json::Value& val, std::size_t index) const {
		datum.Set(val.asInt(), index);
	}

	inline void JsonTableParseHelper::SetFloatData(Datum& datum, const Json::Value& val, std::size_t index) const {
		datum.Set(val.asFloat(), index);
	}

	inline void JsonTableParseHelper::SetStringData(Datum& datum, const Json::Value& val, std::size_t index) const {
		datum.SetFromString(val.asString(), index);
	}

	inline void JsonTableParseHelper::PushIntData(Datum& datum, const Json::Value& val, std::size_t) const {
		datum.PushBack(val.asInt());
	}

	inline void JsonTableParseHelper::PushFloatData(Datum& datum, const Json::Value& val, std::size_t) const {
		datum.PushBack(val.asFloat());
	}

	inline void JsonTableParseHelper::PushStringData(Datum& datum, const Json::Value& val, std::size_t) const {
		datum.PushBackFromString(val.asString());
	}
}
