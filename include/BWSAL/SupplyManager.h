#pragma once
#include <BWAPI.h>
#include <BWSAL/MacroTask.h>
namespace BWSAL
{
  class MacroTask;
  class Task;
  class BuildOrderManager;
  class TaskScheduler;

  /*! \brief Automatically interacts with a BuildOrderManager to build an appropriate amount of supply bulidings/units to execute the queue of MacroTask jobs.
   */
  class SupplyManager : public BWAPI::AIModule
  {
    public:
      //! Create the SupplyManager instance, given a BuildOrderManager and TaskScheduler
      static SupplyManager* create( BuildOrderManager* buildOrderManager, TaskScheduler* taskScheduler );

	  //! Return the SupplyManager instance
      static SupplyManager* getInstance();

	  //! Destroy the SupplyManager instance
      static void destroy();

	  //! Must be called every frame during the BWAPI::AIModule onFrame() callback
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