#pragma once
#include <map>
#include <BWSAL/BuildEvent.h>
#include <BWSAL/Types.h>
namespace BWSAL
{
  class BuildEventTimeline;
  class Task;
  class BuildUnit;
  class BuildUnitManager;
  class TaskScheduler;
  class TaskPlan
  {
    friend class TaskScheduler;
    public:
      int getRunTime() const { return m_runTime; }
      BuildUnit* getBuilder() const { return m_builder; }
      BuildUnit* getSecondBuilder() const { return m_secondBuilder; }
    private:
      int m_runTime;
      BuildUnit* m_builder;
      BuildUnit* m_secondBuilder; //not used yet (only needed for archon and dark archon)
  };
  class TaskScheduler : public BWAPI::AIModule
  {
    public:
      static TaskScheduler* create( BuildEventTimeline* timeline, BuildUnitManager* buildUnitManager );
      static TaskScheduler* getInstance();
      static void destroy();
      TaskPlan scheduleTask( Task* t );
      bool finalizeSchedule();
      int getInsufficientTypes();
      int getSupplyBlockTime() const;
      void resetSupplyBlockTime();
      int getLastMineralBlockTime() const;
      void resetLastMineralBlockTime();
      int getLastGasBlockTime() const;
      void resetLastGasBlockTime();
      BuildEventTimeline* getTimeline() const;
    private:
      TaskScheduler();
      ~TaskScheduler();
      TaskPlan scheduleLarvaUsingTask( Task* t );
      bool canCompleteWithUnitBeforeNextEvent( int validBuildTypeSince,
                                               BuildUnit* unit,
                                               const Task* t,
                                               const std::list< std::pair< int, BuildEvent > >::const_iterator nextEvent );
      // Planning data associated with a particular HLH ( Hatchery, Lair, or Hive )
      class HLHPlanData
      {
        public:
          HLHPlanData();
          int candidateNextLarvaSpawnTime;
          int candidateLarvaCount;
          int candidateMorphTime;
          bool candidateMorphed;
      };
      void resetCandidates( std::map< BuildUnit*, HLHPlanData > *hlhPlans, BuildState* state );
      void initializeHLHPlanData( std::map< BuildUnit*, HLHPlanData > *hlhPlans );
      void continueToTimeWithLarvaSpawns( BuildState* state, std::map< BuildUnit*, HLHPlanData > *hlhPlans, int time );
      void findCandidateMorphTimes( std::map< BuildUnit*, HLHPlanData > *hlhPlans, int validBuildTimeSince );
      void updateLastBlockTimes( BuildState* state, BuildType type );
      BuildEventTimeline* m_timeline;
      int m_supplyBlockTime;
      int m_insufficientTypes;
      int m_debugLevel;
      int m_lastMineralBlockTime;
      int m_lastGasBlockTime;
      TaskPlan m_candidatePlan;
      Task* m_candidateTask;
      BuildUnitManager* m_buildUnitManager;
      static TaskScheduler* s_taskScheduler;
  };
}