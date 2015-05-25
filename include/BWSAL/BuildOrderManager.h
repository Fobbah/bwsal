#pragma once
#include <BWAPI.h>
#include <BWSAL/BuildType.h>
#include <map>
#include <list>
namespace BWSAL
{
  class Task;
  class TaskScheduler;
  class TaskExecutor;
  class BuildUnitManager;
  class MacroTask;
  class BuildOrderManager : public BWAPI::AIModule
  {
    public:
      static BuildOrderManager* create( TaskScheduler* taskScheduler, TaskExecutor* taskExecutor, BuildUnitManager* buildUnitManager );
      static BuildOrderManager* getInstance();
      static void destroy();
      void onFrame();
      void draw( int x, int y );
      MacroTask* build( int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedLocation = BWAPI::TilePositions::None );
      MacroTask* buildAdditional( int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedLocation = BWAPI::TilePositions::None );
      MacroTask* buildAdditional( int count, BuildType t, int priority, BWAPI::TilePosition seedLocation = BWAPI::TilePositions::None );
      MacroTask* research( BWAPI::TechType t, int priority );
      MacroTask* upgrade( int level, BWAPI::UpgradeType t, int priority );
      void deleteMacroTask( MacroTask* mt );

    private:
      class PriorityLevel
      {
        public:
          std::list< MacroTask* > techAndUpgradeMacroTasks;
          std::list< MacroTask* > unitMacroTasks;
      };
      void resolveDependencies( int insufficientTypes, int priority );
      void computeTotalPlannedCounts();
      BuildOrderManager();
      ~BuildOrderManager();
      typedef std::map< int, PriorityLevel, std::greater<int> > PMTMap;
      std::map< BuildType, int > m_totalPlannedTypeCount;
      std::map< BuildType, std::pair< int, int > > m_totalNeededTypeCount;
      std::map< BuildType, int > m_totalScheduledTypeCount;
      PMTMap m_prioritizedMacroTasks;
      TaskScheduler* m_taskScheduler;
      TaskExecutor* m_taskExecutor;
      BuildUnitManager* m_buildUnitManager;
      std::vector< MacroTask* > m_newMacroTasks;
      int m_debugLevel;
      static BuildOrderManager* s_buildOrderManager;
  };
}