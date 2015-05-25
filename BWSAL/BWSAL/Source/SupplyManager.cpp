#include <BWSAL/SupplyManager.h>
#include <BWSAL/BuildOrderManager.h>
#include <BWSAL/TaskScheduler.h>
#include <BWSAL/BuildEventTimeline.h>
#include <BWSAL/MacroTask.h>
#include <BWSAL/Task.h>
#include <BWSAL/util.h>
namespace BWSAL
{
  SupplyManager* SupplyManager::s_supplyManager = NULL;
  SupplyManager* SupplyManager::create( BuildOrderManager* buildOrderManager, TaskScheduler* taskScheduler )
  {
    if ( s_supplyManager )
    {
      return s_supplyManager;
    }
    s_supplyManager = new SupplyManager();
    s_supplyManager->m_buildOrderManager = buildOrderManager;
    s_supplyManager->m_taskScheduler = taskScheduler;
    return s_supplyManager;
  }

  SupplyManager* SupplyManager::getInstance()
  {
    return s_supplyManager;
  }

  void SupplyManager::destroy()
  {
    if ( s_supplyManager )
    {
      delete s_supplyManager;
    }
  }

  SupplyManager::SupplyManager()
  {
    m_supplyProviderType = BuildType( BWAPI::Broodwar->self()->getRace().getSupplyProvider() );
    m_buildTime = m_supplyProviderType.buildUnitTime() + m_supplyProviderType.prepTime();
    m_initialSupplyProviderCount = BWAPI::Broodwar->self()->completedUnitCount( m_supplyProviderType.getUnitType() );
  }

  SupplyManager::~SupplyManager()
  {
    s_supplyManager = NULL;
  }
  void SupplyManager::onFrame()
  {
    if ( BWAPI::Broodwar->self()->completedUnitCount( m_supplyProviderType.getUnitType() ) > m_initialSupplyProviderCount ||
         BWAPI::Broodwar->getFrameCount() > 24 * 120 )
    {

      std::list< MacroTask* >::iterator i = m_incompleteTasks.begin();
      std::list< MacroTask* >::iterator i2 = i;
      for( ; i != m_incompleteTasks.end(); i = i2 )
      {
        i2++;
        if ( (*i)->getTasks().front()->isCompleted() )
        {
          m_incompleteTasks.erase( i );
        }
      }
      int supplyBlockTime = m_taskScheduler->getSupplyBlockTime();
      // State/Planning information is stored in BuildState and in the BuildUnits themselves
      if ( supplyBlockTime != NEVER )
      {
        bool resolvedSupplyBlock = false;
        for( MacroTask* mt : m_incompleteTasks )
        {
          Task* t = mt->getTasks().front();
          if ( t->getEarliestStartTime() > supplyBlockTime - m_buildTime )
          {
            t->setEarliestStartTime( supplyBlockTime - m_buildTime );
            resolvedSupplyBlock = true;
            break;
          }
        }

        if ( !resolvedSupplyBlock )
        {
          bool allScheduledBefore = true;
          for( MacroTask* mt : m_incompleteTasks )
          {
            Task* t = mt->getTasks().front();
            if ( t->getRunTime() + m_buildTime >= supplyBlockTime )
            {
              allScheduledBefore = false;
            }
            logTask(t," ");
          }
          if ( allScheduledBefore )
          {
            log("[SupplyManager] Creating another supply provider to resolve supply block at time %d",m_taskScheduler->getSupplyBlockTime());
            MacroTask* mt = m_buildOrderManager->buildAdditional(1, BWAPI::Broodwar->self()->getRace().getSupplyProvider(), 10000);
            mt->getTasks().front()->setEarliestStartTime( m_taskScheduler->getSupplyBlockTime() - m_buildTime );
            m_incompleteTasks.push_back( mt );
          }
        }
      }
    }
    m_taskScheduler->resetSupplyBlockTime();
  }
}