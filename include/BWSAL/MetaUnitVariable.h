#pragma once
#include <BWSAL/BuildType.h>
#include <BWSAL/MetaUnit.h>
namespace BWAPI
{
  class Unit;
}
namespace BWSAL
{
  class Task;
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