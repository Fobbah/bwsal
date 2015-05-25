#pragma once
#include <BWAPI.h>
#include <BWSAL/BuildUnit.h>
namespace BWSAL
{
  class BuildUnitManager : public BWAPI::AIModule
  {
    friend BuildUnit;
    public:
      static BuildUnitManager* create();
      static BuildUnitManager* getInstance();
      static void destroy();
      virtual void onUnitEvade( BWAPI::Unit* unit );
      std::set< BuildUnit* >& getUnits();
      void resetPlanningData();

    private:
      BuildUnitManager();
      ~BuildUnitManager();
      void addUnit( BuildUnit* unit );
      std::set< BuildUnit* > m_buildUnits;
      static BuildUnitManager* s_BuildUnitManager;
  };
}
