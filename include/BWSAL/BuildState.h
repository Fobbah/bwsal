#pragma once
#include <BWSAL/BuildEvent.h>
#include <BWSAL/Types.h>
#include <map>
namespace BWSAL
{
  class WorkerManager;
  class BuildUnit;
  class TaskExecutor;
  class TaskScheduler;
  class Task;
  class BuildState
  {
    friend class TaskExecutor;
    friend class TaskScheduler;
    public:
      BuildState();

      void continueToTime( int time );
      void doEvent( BuildEvent& e );
      void updateWithCurrentGameState();
      void createUnclaimedBuildUnits();
      int getNextTimeWithMinimumResources( int minerals, int gas ) const;
      int getInsufficientTypes( BuildType buildType ) const;
      bool isSupplyBlocked( Task* t ) const;
      int getTime() const;
      double getMinerals() const;
      double getGas() const;
      int getSupplyAvailable() const;
      int getSupplyTotal() const;
      int getSupplyUsed() const;
      int getMineralWorkers() const;
      int getGasWorkers() const;
      unsigned int getCompletedBuildTypes() const;

      int reservedMinerals;
      int reservedGas;
    private:
      unsigned int m_completedBuildTypes;
      int m_time;
      double m_minerals;
      double m_gas;
      int m_supplyTotal;
      int m_supplyUsed;
      int m_mineralWorkers;
      int m_gasWorkers;
  };
}