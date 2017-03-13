#pragma once
#include <BWSAL/BuildType.h>
#include <BWSAL/MetaUnit.h>
namespace BWAPI
{
	typedef UnitInterface *Unit;
}
namespace BWSAL
{
  class Task;

  /*! \brief (Internal class) extension of a MetaUnit
  *
  * TODO: remove from public API?
  */
  class MetaUnitVariable : public MetaUnit
  {
    public:
      virtual BuildUnit* getBuildUnit();
      virtual BuildType getType() const;
      MetaUnitVariable();
      void assign( MetaUnit* metaUnit );
    private:
      MetaUnit* m_metaUnit;
  };
}