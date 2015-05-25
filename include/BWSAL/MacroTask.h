#pragma once
#include <list>
#include <BWAPI.h>
#include <BWSAL/Task.h>
namespace BWSAL
{
  class Task;
  class BuildOrderManager;
  class MacroTask
  {
    public:
      const std::list< Task* >& getTasks() const;
      BuildType getType() const;
      bool isAdditional() const;
      bool isCompleted() const;
      int getCount() const;
      BWAPI::TilePosition getSeedLocation() const;
      int getPriority() const;
      int getRemainingCount() const;
      int getWaitingCount() const;
      int getIncompleteCount() const;
      int computeRemainingCount();
      void setRemainingCount( int remainingCount );
      Task* getNextUnscheduledTask() const;
      MacroTask(BuildType type, int priority, bool isAdditional, int count, BWAPI::TilePosition seedLocation = BWAPI::TilePositions::None );
    private:
      std::list< Task* > m_tasks;
      BuildType m_type;
      bool m_isAdditional;
      bool m_isCompleted;
      int m_count;
      int m_remainingCount;
      int m_priority;
      BWAPI::TilePosition m_seedLocation;
  };
}