#pragma once
#include <string>

namespace BWSAL {

	/*! \brief Identifies a version of BWSAL and dependent libraries it was compiled with
	 */
	struct BWSALVersionInfo
	{
		std::string mBWSAL_Version;
		std::string mBWAPI_Version;
		std::string mBWTA2_Version;
	};

	//! Return the version of BWSAL and dependent libraries it was compiled with in this release
	BWSALVersionInfo GetBWSALVersionInfo();
}