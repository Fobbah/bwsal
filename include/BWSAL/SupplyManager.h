#pragma once
#include <BWAPI.h>
#include <BWSAL/MacroTask.h>
namespace BWSAL
{
  class MacroTask;
  class Task;
  class BuildOrderManager;
  class TaskScheduler;
  class SupplyManager : public BWAPI::AIModule
  {
    public:
      static SupplyManager* create( BuildOrderManager* buildOrderManager, TaskScheduler* taskScheduler );
      static SupplyManager* getInstance();
      static void destroy();
      virtual void onFrame();
    private:
      SupplyManager();
      ~SupplyManager();
      std::list< MacroTask* > m_incompleteTasks;
      BuildOrderManager* m_buildOrderManager;
      TaskScheduler* m_taskScheduler;
      BuildType m_supplyProviderType;
      int m_buildTime;
      int m_initialSupplyProviderCount;
      static SupplyManager* s_supplyManager;
  };
}