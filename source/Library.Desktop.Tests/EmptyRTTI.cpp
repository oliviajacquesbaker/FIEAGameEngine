/// <summary>
/// The definition of the class EmptyRTTI.
/// </summary>

#include "pch.h"
#include "EmptyRTTI.h"

using namespace std;

namespace UnitTests
{
	RTTI_DEFINITIONS(EmptyRTTI);

	bool EmptyRTTI::operator==(const EmptyRTTI& rhs) {
		UNREFERENCED_LOCAL(rhs);
		return false;
	}
}