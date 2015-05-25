#pragma once
#include <BWSAL/Arbitrator.h>
#include <BWAPI.h>
namespace BWSAL
{
  class Base;
  class BaseManager;
  class WorkerManager : public UnitController, public BWAPI::AIModule
  {
    public:
      static WorkerManager* create( UnitArbitrator* aribtrator, BaseManager* baseManager );
      static WorkerManager* getInstance();
      static void destroy();
      virtual void onOffer( std::set< BWAPI::Unit* > units );
      virtual void onRevoke( BWAPI::Unit* unit, double bid );
      virtual void onFrame();
      virtual void onUnitComplete( BWAPI::Unit* unit );
      virtual std::string getName() const;

      void setWorkersPerGas( int count );
      double getMineralRate() const;
      double getGasRate() const;
      int getOptimalWorkerCount() const;
      int mineralWorkerCount() const;
      int gasWorkerCount() const;
      void enableAutoBuild();
      void disableAutoBuild();
      void setAutoBuildPriority( int priority );

    private:
      class WorkerData
      {
        public:
          WorkerData() : m_resource( NULL ), m_newResource( NULL ), m_lastFrameSpam( NULL ) {}
          BWAPI::Unit* m_resource;
          BWAPI::Unit* m_newResource;
          int m_lastFrameSpam;
      };
      WorkerManager();
      ~WorkerManager();

      UnitArbitrator* m_arbitrator;
      BaseManager* m_baseManager;
      static WorkerManager* s_workerManager;

      void rebalanceWorkers();
      void updateWorkerAssignments();
      std::map< BWAPI::Unit*, WorkerData > m_workers;
      std::map< BWAPI::Unit*, std::set< BWAPI::Unit* > > m_currentWorkers;
      std::map< BWAPI::Unit*, Base* > m_resourceBase;
      std::map< BWAPI::Unit*, int > m_desiredWorkerCount;
      std::vector< std::pair< BWAPI::Unit*, int > > m_mineralOrder;
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