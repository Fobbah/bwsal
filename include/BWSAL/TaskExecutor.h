#pragma once
#include <BWSAL/Arbitrator.h>
#include <BWSAL/BuildType.h>
#include <BWAPI.h>
namespace BWSAL
{
  class Task;
  class BuildingPlacer;
  class TaskScheduler;
  class ReservedMap;
  class BuildEventTimeline;
  class TaskExecutor : public UnitController, public BWAPI::AIModule
  {
    public:
      static TaskExecutor* create( UnitArbitrator* arbitrator, BuildEventTimeline* timeline, ReservedMap* reserveMap, BuildingPlacer* defaultBuildingPlacer );
      static TaskExecutor* getInstance();
      static void destroy();
      virtual void onOffer( std::set< BWAPI::Unit > units );
      virtual void onRevoke( BWAPI::Unit unit, double bid );
      virtual std::string getName() const;
      virtual void onFrame();
      void run( Task* t );
      int getRunningCount( BuildType type ) const;
    private:
      TaskExecutor();
      ~TaskExecutor();
      void updateTask( Task* t );
      UnitArbitrator* m_arbitrator;
      ReservedMap* m_reservedMap;
      BuildingPlacer* m_defaultBuildingPlacer;
      BuildEventTimeline* m_timeline;
      static TaskExecutor* s_taskExecutor;
      std::list< Task* > m_runningTasks;

      void computeCreatedUnit( Task* t );
      void computeSecondCreatedUnit( Task* t );
      void computeIsExecuting( Task* t );
      void computeIsCompleted( Task* t );
      void computeIsReady( Task* t );
      void getReady( Task* t );
      void execute( Task* t );
      std::set< BWAPI::Unit > m_builders;
      std::map< BuildType, int > m_runningCount;
  };
}