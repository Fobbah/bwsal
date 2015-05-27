#pragma once
#include "BWSAL/BuildType.h"
namespace BWAPI
{
	typedef UnitInterface *Unit;
}
namespace BWSAL
{
  // Meta Unit interface. Implemented by BuildUnits and MetaUnitVariables
  class BuildUnit;
  class MetaUnit
  {
    public:
      virtual BuildUnit* getBuildUnit() = 0;
      virtual BuildType getType() const = 0;
  };
}