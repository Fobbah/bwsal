#pragma once
#include <BWSAL/Arbitrator.h>
#include <BWAPI.h>
namespace BWSAL
{
  class Base;
  class BaseManager;

  /*! \brief The worker manager ensures that workers are mining minerals and gas at maximum capacity at each base. Transfers workers to new expansions and balances worker amounts across expansions as needed.
   *
   * This class interacts closely wth the BaseManager to identify the list of active bases to assign workers to gather resources from.
   */
  class WorkerManager : public UnitController, public BWAPI::AIModule
  {
    public:
	  //! Create the WorkerManager singleton instance
      static WorkerManager* create( UnitArbitrator* aribtrator, BaseManager* baseManager );

	  //! Return the WorkerManager singleton instance
      static WorkerManager* getInstance();

	  //! Destroy the WorkerManager singleton instance
      static void destroy();

	  //! UnitController notification, the WorkerManager has been offered these units by the Arbitrator
      virtual void onOffer( std::set< BWAPI::Unit > units );

	  //! UnitController notification, the Arbitrator has revoked access to these units, and the WorkerManagrer has been outbid with the supplied bid
      virtual void onRevoke( BWAPI::Unit unit, double bid );

	  //! Should be called every frame during AIModule::onFrame()
      virtual void onFrame();

	  //! Should be called during AIModule::OnUnitComplete()
      virtual void onUnitComplete( BWAPI::Unit unit );

	  //! Retrieve the name of this component
      virtual std::string getName() const;

	  //! Used to set how many workers should be used per refinery. The default value is 3 and generally this is the optimal number, but some times in the early game you may want to have only 1 worker on gas.
      void setWorkersPerGas( int count );

	  //! Returns the rate at which the worker manager is gathering minerals, in minerals per frame.
      double getMineralRate() const;

	  //! Returns the rate at which the worker manager is gathering gas, in gas per frame.
      double getGasRate() const;

	  //! Returns the optimal worker count to saturate the current set of active bases
      int getOptimalWorkerCount() const;

	  //! Returns the number of workers assigned to mining minerals
      int mineralWorkerCount() const;

	  //! Returns the number of workers assigned to gas
      int gasWorkerCount() const;

	  //! Enables auto-build, which automatically builds workers when needed to keep all mining bases gathering at maximum capacity.
      void enableAutoBuild();

	  //! Disables auto-build.
      void disableAutoBuild();

	  //! Sets the auto-build priority. Default priority level is 80.
      void setAutoBuildPriority( int priority );

    private:
      class WorkerData
      {
        public:
          WorkerData() : m_resource( NULL ), m_newResource( NULL ), m_lastFrameSpam( NULL ) {}
          BWAPI::Unit m_resource;
          BWAPI::Unit m_newResource;
          int m_lastFrameSpam;
      };
      WorkerManager();
      ~WorkerManager();

      UnitArbitrator* m_arbitrator;
      BaseManager* m_baseManager;
      static WorkerManager* s_workerManager;

      void rebalanceWorkers();
      void updateWorkerAssignments();
      std::map< BWAPI::Unit, WorkerData > m_workers;
      std::map< BWAPI::Unit, std::set< BWAPI::Unit > > m_currentWorkers;
      std::map< BWAPI::Unit, Base* > m_resourceBase;
      std::map< BWAPI::Unit, int > m_desiredWorkerCount;
      std::vector< std::pair< BWAPI::Unit, int > > m_mineralOrder;
      int m_mineralOrderIndex;
      int m_lastSCVBalance;
      std::set< Base* > m_basesCache;
      int m_workersPerGas;
      double m_mineralRate;
      double m_gasRate;
      int m_mineralWorkers;
      int m_gasWorkers;
      bool m_autoBuild;
      int m_autoBuildPriority;
      int m_optimalWorkerCount;
  };
}