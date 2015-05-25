#pragma once
#include <BWSAL/BuildType.h>
#include <BWSAL/TaskState.h>
#include <BWAPI.h>
#include <BWSAL/BuildEvent.h>
namespace BWSAL
{
  class BuildUnit;
  class MetaUnit;
  class MetaUnitVariable;
  class TaskStream;
  class BuildingPlacer;
  class Task
  {
    friend class TaskScheduler;
    friend class TaskExecutor;
    public:
      Task( BuildType type = BuildTypes::None );
      int getID() const;
      std::string toString() const;
      BuildType getBuildType() const;
      MetaUnitVariable* getBuilder() const;
      MetaUnitVariable* getSecondBuilder() const;
      BuildUnit* getCreatedUnit() const;
      BuildUnit* getSecondCreatedUnit() const;
      BWAPI::TilePosition getBuildLocation() const;
      BWAPI::TilePosition getSeedLocation() const;
      BuildEvent getReserveBuilderEvent() const;
      BuildEvent getReserveResourcesEvent() const;
      BuildEvent getReleaseBuilderEvent() const;
      BuildEvent getCompleteBuildTypeEvent() const;
      // returns true if this task has been scheduled on this frame
      bool isScheduledThisFrame() const;
      // returns true if the start time has occurred, but the completion time has not
      bool isWaiting() const;
      bool isRunning() const;
      bool isCompleted() const;
      TaskState getState() const;
      void setState( TaskState state );
      int getEarliestStartTime() const;
      void setEarliestStartTime( int time );
      int getRunTime() const;
      void setRunTime( int time );
      int getExecuteTime() const;
      void setExecuteTime( int time );
      int getCompletionTime() const;
      void setCompletionTime( int time );
      int getLastOrderTime() const;
      void setLastOrderTime();
      bool isRelocatable() const;
      void setRelocatable( bool relocatable );
      // sets scheduled to true for the current frame
      void setScheduledThisFrame();
      void setBuildLocation( BWAPI::TilePosition buildLocation );
      void setSeedLocation( BWAPI::TilePosition seedLocation );
      void assignBuilder( MetaUnit* builder );
      void assignBuilders( MetaUnit* builder, MetaUnit* secondBuilder );
      void assignBuilders( std::set< MetaUnit* > builders );
      void useAnyBuilder();
      bool canUseAnyBuilder() const;

    protected:
      BuildType m_type;
      int m_lastScheduledTime;
      int m_earliestStartTime;
      int m_runTime;
      int m_executeTime;
      int m_completionTime;
      int m_lastOrderTime;
      bool m_relocatable;
      int id;
      TaskState m_state;
      MetaUnitVariable* m_builder;
      MetaUnitVariable* m_secondBuilder;
      BuildUnit* m_createdUnit;
      BuildUnit* m_secondCreatedUnit;
      std::set< MetaUnit* > m_possibleBuilders;
      bool m_useAnyBuilder;
      BWAPI::TilePosition m_buildLocation;
      BWAPI::TilePosition m_seedLocation;
      BuildingPlacer* m_buildingPlacer;
      static int s_nextFreeTaskID;
  };
}