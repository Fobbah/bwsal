#include <BWSAL/WorkerManager.h>
#include <BWSAL/BaseManager.h>
#include <BWSAL/Base.h>
#include <BWSAL/RectangleArray.h>
#include <BWSAL/UnitGroupManager.h>
#include <BWSAL/Util.h>
#include <BWSAL/Types.h>
#include <algorithm>
namespace BWSAL
{
  WorkerManager* WorkerManager::s_workerManager = NULL;
  WorkerManager* WorkerManager::create( UnitArbitrator* arbitrator, BaseManager* baseManager )
  {
    if ( s_workerManager != NULL )
    {
      return s_workerManager;
    }
    s_workerManager = new WorkerManager();
    s_workerManager->m_arbitrator = arbitrator;
    s_workerManager->m_baseManager = baseManager;
    return s_workerManager;
  }

  WorkerManager* WorkerManager::getInstance()
  {
    return s_workerManager;
  }

  void WorkerManager::destroy()
  {
    if ( s_workerManager )
    {
      delete s_workerManager;
    }
  }

  WorkerManager::WorkerManager()
  {
    m_lastSCVBalance = 0;
    m_workersPerGas = 3;
    m_mineralRate = 0;
    m_gasRate = 0;
    m_autoBuild = false;
    m_autoBuildPriority = 80;
    m_arbitrator = NULL;
    m_baseManager = NULL;
    m_mineralWorkers = 0;
    m_gasWorkers = 0;
  }

  WorkerManager::~WorkerManager()
  {
    s_workerManager = NULL;
  }

  void WorkerManager::onOffer( std::set< BWAPI::Unit > units )
  {
    for( BWAPI::Unit u : units )
    {
      if ( u->getType().isWorker() && !m_mineralOrder.empty() )
      {
        m_arbitrator->accept( this, u );
        WorkerData temp;
        m_desiredWorkerCount[m_mineralOrder[m_mineralOrderIndex].first]++;
        m_currentWorkers[m_mineralOrder[m_mineralOrderIndex].first].insert( u );
        temp.m_newResource = m_mineralOrder[m_mineralOrderIndex].first;
        m_mineralOrderIndex = ( m_mineralOrderIndex + 1 ) % m_mineralOrder.size();
        m_workers.insert( std::make_pair( u, temp ) );
      }
      else
      {
        m_arbitrator->decline( this, u, 0 );
      }
    }
  }
  void WorkerManager::onRevoke( BWAPI::Unit unit, double bid )
  {
    // Sanity check
    if ( unit == NULL )
    {
      return;
    }

    if ( unit->getType().isWorker() )
    {
      // One of our workers got revoked :( 
      m_workers.erase( unit );
    }

    if ( unit->getType().isResourceContainer() )
    {
      std::map< BWAPI::Unit, std::set< BWAPI::Unit > >::iterator c = m_currentWorkers.find( unit );
      if ( c == m_currentWorkers.end() )
      {
        return;
      }
      for( BWAPI::Unit i : c->second )
      {
        std::map< BWAPI::Unit, WorkerData >::iterator j = m_workers.find( i );
        if ( j != m_workers.end() )
        {
          if ( j->second.m_resource == unit )
          {
            j->second.m_resource = NULL;
          }
          if ( j->second.m_newResource == unit )
          {
            j->second.m_newResource = NULL;
          }
        }
      }
    }
  }

  void WorkerManager::updateWorkerAssignments()
  {
    // determine current worker assignments
    // also workers that are mining from resources that dont belong to any of our bases will be set to free
    for ( std::map< BWAPI::Unit, WorkerData >::iterator w = m_workers.begin(); w != m_workers.end(); w++ )
    {
      if ( w->second.m_newResource != NULL )
      {
        if ( m_resourceBase.find( w->second.m_newResource ) == m_resourceBase.end() )
        {
          w->second.m_newResource = NULL;
        }
        else
        {
          m_currentWorkers[w->second.m_newResource].insert( w->first );
        }
      }
    }

    // get free workers
    std::set< BWAPI::Unit > freeWorkers;

    for ( std::map< BWAPI::Unit, WorkerData >::iterator w = m_workers.begin(); w != m_workers.end(); w++ )
    {
      if ( w->second.m_newResource == NULL )
      {
        freeWorkers.insert( w->first );
      }
      else
      {
        // free workers that are too far away from their resources
        if ( w->first->getDistance( w->second.m_newResource ) > TILE_SIZE * 10 )
        {
          freeWorkers.insert( w->first );
          // erase worker from resource's current workers set
          m_currentWorkers[w->second.m_newResource].erase( w->first );
        }
      }
    }

    // free workers from resources with too many workers
    for ( std::map< BWAPI::Unit, int >::iterator i = m_desiredWorkerCount.begin(); i != m_desiredWorkerCount.end(); i++ )
    {
      if ( i->second < (int)m_currentWorkers[i->first].size() )
      {
        // desired worker count is less than the current worker count for this resource, so lets remove some workers.
        int amountToRemove = m_currentWorkers[i->first].size() - i->second;
        for ( int j = 0; j < amountToRemove; j++ )
        {
          BWAPI::Unit worker = *m_currentWorkers[i->first].begin();
          freeWorkers.insert( worker );
          m_workers[worker].m_newResource = NULL;
          m_currentWorkers[i->first].erase( worker );
        }
      }
    }

    std::vector< BWAPI::Unit > workerUnit;
    std::vector< BWAPI::Unit > taskUnit;
    std::map< int, int > assignment;

    for( BWAPI::Unit i : freeWorkers )
    {
      workerUnit.push_back( i );
    }

    // assign workers to resources that need more workers
    for ( std::map< BWAPI::Unit, int >::iterator i = m_desiredWorkerCount.begin(); i != m_desiredWorkerCount.end(); i++ )
    {
      if ( i->second > (int)m_currentWorkers[i->first].size() )
      {
        for ( int j = (int)m_currentWorkers[i->first].size(); j < i->second; j++ )
        {
          taskUnit.push_back( i->first );
        }
      }
    }

    // construct cost matrix
    // currently just uses euclidean distance, but walking distance would be more accurate
    RectangleArray< double > cost( workerUnit.size(), taskUnit.size() );
    for ( int w = 0; w < (int)workerUnit.size(); w++ )
    {
      for ( int t = 0; t < (int)taskUnit.size(); t++ )
      {
        cost[w][t] = workerUnit[w]->getDistance( taskUnit[t] );
      }
    }

    // calculate assignment for workers and tasks ( resources )
    assignment = computeAssignments( cost );

    // use assignment
    for ( std::map< int, int >::iterator a = assignment.begin(); a != assignment.end(); a++ )
    {
      BWAPI::Unit worker = workerUnit[a->first];
      BWAPI::Unit resource = taskUnit[a->second];
      m_workers[worker].m_newResource = resource;
      m_currentWorkers[resource].insert( worker );
    }
  }

  bool mineralCompare ( const std::pair< BWAPI::Unit, int > i, const std::pair< BWAPI::Unit, int > j )
  {
    return i.second > j.second;
  }

  void WorkerManager::rebalanceWorkers()
  {
    m_mineralOrder.clear();
    m_desiredWorkerCount.clear();
    m_currentWorkers.clear();
    m_resourceBase.clear();
    int remainingWorkers = m_workers.size();
    m_optimalWorkerCount = 0;
    
    // iterate over all the resources of each active base
    for( Base* b : m_basesCache )
    {
      const BWAPI::Unitset baseMinerals = b->getMinerals();
      std::vector< std::pair< BWAPI::Unit, int > > baseMineralOrder;
      for( BWAPI::Unit m : baseMinerals )
      {
        m_resourceBase[m] = b;
        m_desiredWorkerCount[m] = 0;
        baseMineralOrder.push_back( std::make_pair( m, m->getResources() - 2*(int)m->getPosition().getDistance( b->getBaseLocation()->getPosition() ) ) );
        m_optimalWorkerCount += 2;
      }
      std::sort( baseMineralOrder.begin(), baseMineralOrder.end(), mineralCompare );
      for ( int i = 0; i < (int)baseMineralOrder.size(); i++ )
      {
        BWAPI::Unit mineral = baseMineralOrder[i].first;
        m_mineralOrder.push_back( std::make_pair( mineral, mineral->getResources() - 2*(int)mineral->getPosition().getDistance( b->getBaseLocation()->getPosition() ) - 3000*i ) );
      }
      const BWAPI::Unitset baseGeysers = b->getGeysers();
      for( BWAPI::Unit g : baseGeysers )
      {
        m_optimalWorkerCount += 3;
        m_resourceBase[g] = b;
        m_desiredWorkerCount[g] = 0;

        if( remainingWorkers < 4 )// always save some workers for minerals
        {
          continue;
        }

        if ( g->getType().isRefinery() && g->getPlayer() == BWAPI::Broodwar->self() && resourceDepotIsCompleted( g ) )
        {
          for ( int w = 0; w < m_workersPerGas && remainingWorkers > 0; w++ )
          {
            m_desiredWorkerCount[g]++;
            remainingWorkers--;
          }
        }
      }
    }

    // if no resources exist, return
    if ( !m_mineralOrder.empty() )
    {

      // order minerals by score ( based on distance and resource amount )
      std::sort( m_mineralOrder.begin(), m_mineralOrder.end(), mineralCompare );

      // calculate optimal worker count for - each mineral patch
      m_mineralOrderIndex = 0;
      while ( remainingWorkers > 0 )
      {
        m_desiredWorkerCount[m_mineralOrder[m_mineralOrderIndex].first]++;
        remainingWorkers--;
        m_mineralOrderIndex = ( m_mineralOrderIndex + 1 ) % m_mineralOrder.size();
      }
    }

    // update the worker assignments so the actual workers per resource is the same as the desired workers per resource
    updateWorkerAssignments();
    if ( m_autoBuild )
    {
      BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    }
  }

  void WorkerManager::onFrame()
  {
    // bid a constant value of 10 on all completed workers
    std::set< BWAPI::Unit > w = SelectAll()( isCompleted )( isWorker );
    for( BWAPI::Unit u : w )
    {
      m_arbitrator->setBid( this, u, 10 );
    }
    // rebalance workers when necessary
    std::set< Base* > bases = m_baseManager->getActiveBases();
    if ( BWAPI::Broodwar->getFrameCount() > m_lastSCVBalance + 20 * 24 || bases != m_basesCache || m_lastSCVBalance == 0 )
    {
      m_basesCache = bases;
      m_lastSCVBalance = BWAPI::Broodwar->getFrameCount();
      rebalanceWorkers();
    }
    for ( std::map< BWAPI::Unit, WorkerData >::iterator w = m_workers.begin(); w != m_workers.end(); w++ )
    {
      if ( w->second.m_resource != NULL )
      {
        BWAPI::Broodwar->drawLineMap( w->first->getPosition().x,
                                      w->first->getPosition().y,
                                      w->second.m_resource->getPosition().x,
                                      w->second.m_resource->getPosition().y,
                                      BWAPI::Colors::White );
      }
    }

    
    // order workers to gather from their assigned resources
    m_mineralRate = 0;
    m_gasRate = 0;
    m_mineralWorkers = 0;
    m_gasWorkers = 0;

    for ( std::map< BWAPI::Unit, WorkerData >::iterator w = m_workers.begin(); w != m_workers.end(); w++ )
    {
      BWAPI::Unit i = w->first;
      if ( w->second.m_resource != NULL )
      {
        if ( w->second.m_resource->getType() == BWAPI::UnitTypes::Resource_Mineral_Field )
        {
          m_mineralRate += MINERALS_PER_WORKER_PER_FRAME;
          m_mineralWorkers++;
        }
        else
        {
          m_gasRate += GAS_PER_WORKER_PER_FRAME;
          m_gasWorkers++;
        }
      }
      
      // switch current resource to newResource when appropiate
      if ( w->second.m_resource == NULL || ( i->getTarget() != NULL && !i->getTarget()->getType().isResourceDepot() ) )
      {
        w->second.m_resource = w->second.m_newResource;
      }

      if ( i->getOrder() == BWAPI::Orders::MoveToMinerals || 
           i->getOrder() == BWAPI::Orders::WaitForMinerals || 
           i->getOrder() == BWAPI::Orders::MoveToGas || 
           i->getOrder() == BWAPI::Orders::WaitForGas || 
           i->getOrder() == BWAPI::Orders::PlayerGuard )
      {
        if ( ( i->getTarget() == NULL || !i->getTarget()->exists() || !i->getTarget()->getType().isResourceDepot() ) && i->getTarget() != w->second.m_resource )
        {
          i->rightClick( w->second.m_resource );
        }
      }
      if ( i->isCarryingGas() || i->isCarryingMinerals() )
      {
        if ( i->getOrder() == BWAPI::Orders::ReturnGas ||
             i->getOrder() == BWAPI::Orders::ReturnMinerals ||
           ( i->getOrder() == BWAPI::Orders::PlayerGuard &&
             BWAPI::Broodwar->getFrameCount() > w->second.m_lastFrameSpam + BWAPI::Broodwar->getLatency()*2 ) )
        {
          w->second.m_lastFrameSpam = BWAPI::Broodwar->getFrameCount();
          Base* b = m_baseManager->getBase( BWTA::getNearestBaseLocation( i->getPosition() ) );
          if ( b != NULL )
          {
            BWAPI::Unit center = b->getResourceDepot();
            if ( i->getTarget() == NULL ||
                !i->getTarget()->exists() ||
                 i->getTarget() != center ||
                ( resourceDepotIsCompleted( center ) && i->getOrder() == BWAPI::Orders::PlayerGuard ) )
            {
              i->rightClick( center );
            }
          }
        }
      }
    }
  }

  void WorkerManager::onUnitComplete( BWAPI::Unit unit )
  {
    if ( unit->getType().isRefinery() )
    {
      rebalanceWorkers();
    }
  }

  std::string WorkerManager::getName() const
  {
    return "Worker Manager";
  }

  void WorkerManager::setWorkersPerGas( int count )
  {
    m_workersPerGas = count;
  }

  double WorkerManager::getMineralRate() const
  {
    return m_mineralRate;
  }

  double WorkerManager::getGasRate() const
  {
    return m_gasRate;
  }

  int WorkerManager::getOptimalWorkerCount() const
  {
    return m_optimalWorkerCount;
  }

  int WorkerManager::mineralWorkerCount() const
  {
    return m_mineralWorkers;
  }

  int WorkerManager::gasWorkerCount() const
  {
    return m_gasWorkers;
  }

  void WorkerManager::enableAutoBuild()
  {
    m_autoBuild = true;
  }

  void WorkerManager::disableAutoBuild()
  {
    m_autoBuild = false;
  }

  void WorkerManager::setAutoBuildPriority( int priority )
  {
    m_autoBuildPriority = priority;
  }
}