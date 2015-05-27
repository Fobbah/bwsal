#pragma once
#include "BWSAL/BuildType.h"
#include "BWSAL/MetaUnit.h"
namespace BWAPI
{
	typedef UnitInterface *Unit;
}
namespace BWSAL
{
  class Task;
  class BuildUnit : public MetaUnit
  {
    public:
      virtual BuildUnit* getBuildUnit();
      virtual BuildType getType() const;
      BuildUnit( Task* task );
      BuildUnit( BWAPI::Unit unit );
      void setUnit( BWAPI::Unit unit );
      void setTask( Task* task );
      bool isReal() const;
      BWAPI::Unit getUnit() const;
      Task* getTask() const;
      static BuildUnit* getBuildUnit( BWAPI::Unit unit );
      static BuildUnit* getBuildUnitIfExists( BWAPI::Unit unit );
      static void setBuildUnit( BWAPI::Unit unit, BuildUnit* buildUnit );
      struct state
      {
        int m_availableSince;
        BuildType m_type;
        int m_nextLarvaSpawnTime;
        int m_larvaCount;
        BuildType m_addon;
      };
      state m_currentState;
      state m_planningData;
    private:
      void initializeInformation();
      BWAPI::Unit m_unit;
      Task* m_task;
  };
}