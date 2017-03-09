#pragma once
#include <string>

namespace BWSAL {
	struct BWSALVersionInfo
	{
		std::string mBWSAL_Version;
		std::string mBWAPI_Version;
		std::string mBWTA2_Version;
	};

	BWSALVersionInfo GetBWSALVersionInfo();
}