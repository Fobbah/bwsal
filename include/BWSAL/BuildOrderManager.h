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

  //! Manages and automatically schedules a task list of units, upgrades and tech to produce.
  class BuildOrderManager : public BWAPI::AIModule
  {
    public:
	  //! Create the BuildOrderManager singleton and return it
      static BuildOrderManager* create( TaskScheduler* taskScheduler, TaskExecutor* taskExecutor, BuildUnitManager* buildUnitManager );

	  //! Return the BuildOrderManager singleton
      static BuildOrderManager* getInstance();

	  //! Destory the BuildOrderManager singleton
      static void destroy();

	  //! Must be called every frame during the BWAPI::AIModule onFrame() callback
      void onFrame();

	  //! Draw debug text information at the specified screen coordinates
      void draw( int x, int y );

	  //! Create, schedule and reutrn a MacroTask to bulid more of the specified unit type until the desired unit count is achieved, at a given priority level. If the unit is building, a seedLocation should be specified as a TilePosition for building placement.
      MacroTask* build( int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedLocation = BWAPI::TilePositions::None );

	  //! Create, schedule and return a MacroTask to bulid the specified unit type & count, at a given priority level. If the unit is building, a seedLocation should be specified as a TilePosition for building placement.
      MacroTask* buildAdditional( int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedLocation = BWAPI::TilePositions::None );

	  //! Create, schedule and return a MacroTask to bulid the specified BuildType & count, at a given priority level. If the BuildType is building unit, a seedLocation should be specified as a TilePosition for building placement.
      MacroTask* buildAdditional( int count, BuildType t, int priority, BWAPI::TilePosition seedLocation = BWAPI::TilePositions::None );

	  //! Create, schedule and return a MacroTask to research the specified TechType at a given priority level
      MacroTask* research( BWAPI::TechType t, int priority );

	  //! Create, schedule and return a MacroTask to upgrade the specified UpgradeType at a given priority level
      MacroTask* upgrade( int level, BWAPI::UpgradeType t, int priority );

	  //! Cancel (deschedule) a MacroTask
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