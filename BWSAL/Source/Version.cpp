#include <BWSAL/Version.h>

namespace BWSAL {
	BWSALVersionInfo GetBWSALVersionInfo()
	{
		BWSALVersionInfo info;
		info.mBWSAL_Version = "2.1";
		info.mBWAPI_Version = "4.1.2";
		info.mBWTA2_Version = "2.2";
		return info;
	}
}