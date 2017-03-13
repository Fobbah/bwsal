#pragma once
#include "BWSAL/BuildType.h"
namespace BWAPI
{
	typedef UnitInterface *Unit;
}
namespace BWSAL
{
  class BuildUnit;

  /*! \brief (Internal class) Meta Unit interface. Implemented by BuildUnit and MetaUnitVariable
  *
  * FIXME: remove from public API?
  */
  class MetaUnit
  {
    public:
      virtual BuildUnit* getBuildUnit() = 0;
      virtual BuildType getType() const = 0;
  };
}