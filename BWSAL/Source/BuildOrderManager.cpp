#include <BWSAL/BuildOrderManager.h>
#include <BWSAL/BuildUnitManager.h>
#include <BWSAL/BuildEventTimeline.h>
#include <BWSAL/Task.h>
#include <BWSAL/TaskScheduler.h>
#include <BWSAL/TaskExecutor.h>
#include <BWSAL/BuildState.h>
#include <BWSAL/MacroTask.h>
#include <BWSAL/BuildUnit.h>
#include <BWSAL/Util.h>
namespace BWSAL
{
  BuildOrderManager* BuildOrderManager::s_buildOrderManager = NULL;
  int planDistance = 24*30;
  BuildOrderManager* BuildOrderManager::create( TaskScheduler* taskScheduler, TaskExecutor* taskExecutor, BuildUnitManager* buildUnitManager )
  {
    if ( s_buildOrderManager )
    {
      return s_buildOrderManager;
    }
    s_buildOrderManager = new BuildOrderManager();
    s_buildOrderManager->m_taskScheduler = taskScheduler;
    s_buildOrderManager->m_taskExecutor = taskExecutor;
    s_buildOrderManager->m_buildUnitManager = buildUnitManager;
    return s_buildOrderManager;
  }

  BuildOrderManager* BuildOrderManager::getInstance()
  {
    return s_buildOrderManager;
  }

  void BuildOrderManager::destroy()
  {
    if ( s_buildOrderManager )
    {
      delete s_buildOrderManager;
    }
  }

  BuildOrderManager::BuildOrderManager()
  {
    m_debugLevel = 0;
  }

  BuildOrderManager::~BuildOrderManager()
  {
    s_buildOrderManager = NULL;
  }

  void BuildOrderManager::resolveDependencies( int insufficientTypes, int priority )
  {
    for( BuildType bt : BuildTypes::allRequiredBuildTypes( BWAPI::Broodwar->self()->getRace() ) )
    {
      if ( ( insufficientTypes & bt.getMask() ) > 0)
      {
        m_totalNeededTypeCount[bt].first = max( m_totalNeededTypeCount[bt].first, m_totalScheduledTypeCount[bt] + 1 );
        m_totalNeededTypeCount[bt].second = max( m_totalNeededTypeCount[bt].second, priority + 1 );
        if ( m_debugLevel >= 10 )
        {
          log( "I may need to build a %s. Current scheduled count: %d", bt.getName().c_str(), m_totalScheduledTypeCount[bt] );
        }
      }
    }
  }

  void BuildOrderManager::onFrame()
  {
    for ( int i = 0; i < (int)m_newMacroTasks.size(); i++ )
    {
      MacroTask* mt = m_newMacroTasks[i];
      if ( mt->getType().isUnitType() )
      {
        m_prioritizedMacroTasks[mt->getPriority()].unitMacroTasks.push_back( mt );
      }
      else
      {
        m_prioritizedMacroTasks[mt->getPriority()].techAndUpgradeMacroTasks.push_back( mt );
      }
    }
    m_newMacroTasks.clear();
    computeTotalPlannedCounts();
    for each( BuildType t in BuildTypes::allBuildTypes( BWAPI::Broodwar->self()->getRace() ) )
    {
      m_totalScheduledTypeCount[t] = BWAPI::Broodwar->self()->completedUnitCount( t.getUnitType() ) + m_taskExecutor->getRunningCount( t );
      m_totalNeededTypeCount[t].first = 0;
      m_totalNeededTypeCount[t].second = 0;
    }
    m_taskScheduler->resetLastMineralBlockTime();
    m_taskScheduler->resetLastGasBlockTime();

    // Schedule tasks
    bool tooFarIntoTheFuture = false;
    if (true || BWAPI::Broodwar->getFrameCount()%2 == 0)
    {
      Heap< MacroTask*, int> macroTaskHeap;
      bool supplyIncrease = true;
      while ( supplyIncrease && tooFarIntoTheFuture == false)
      {
        supplyIncrease = false;
        for ( PMTMap::iterator pmt = m_prioritizedMacroTasks.begin(); pmt != m_prioritizedMacroTasks.end() && supplyIncrease == false && tooFarIntoTheFuture == false; pmt++ )
        {
          for( MacroTask* mt : pmt->second.techAndUpgradeMacroTasks )
          {
            for( Task* t : mt->getTasks())
            {
              if ( t->isWaiting() && t->isScheduledThisFrame() == false )
              {
                TaskPlan plan = m_taskScheduler->scheduleTask( t );
                if ( plan.getRunTime() < BWAPI::Broodwar->getFrameCount() + planDistance*2)
                {
                  m_taskScheduler->finalizeSchedule();
                  m_totalScheduledTypeCount[t->getBuildType()]++;
                }
                else if ( plan.getRunTime() == NEVER )
                {
                  int insufficientTypes = ( m_taskScheduler->getInsufficientTypes() & ~( BuildTypes::SupplyMask | BuildTypes::WorkerMask ) );
                  if ( insufficientTypes != 0 )
                  {
                    if ( m_debugLevel >= 1 )
                    {
                      log( "Found dependencies while scheduling %s", t->getBuildType().getName().c_str() );
                      if ( m_debugLevel >= 5 )
                      {
                        log( "Timeline:" );
                        log( m_taskScheduler->getTimeline()->toString().c_str() );
                      }
                    }
                    resolveDependencies( insufficientTypes, pmt->first );
                  }
                }
                if ( (m_taskScheduler->getLastMineralBlockTime() > BWAPI::Broodwar->getFrameCount() + planDistance &&
                     m_taskScheduler->getLastMineralBlockTime() != NEVER) )
                {
                  tooFarIntoTheFuture = true;
                  break;
                }
              }
            }
          }
          macroTaskHeap.clear();
          for( MacroTask* mt : pmt->second.unitMacroTasks )
          {
            if ( mt->isAdditional() )
            {
              mt->computeRemainingCount();
            }
            else
            {
              mt->setRemainingCount( mt->getCount() - m_totalScheduledTypeCount[mt->getType()] );
            }
            if ( mt->getRemainingCount() > 0 )
            {
              macroTaskHeap.set( mt, mt->getRemainingCount() );
            }
          }
          if ( macroTaskHeap.empty() == false )
          {
            while ( macroTaskHeap.top().second > 0 )
            {
              MacroTask* mt = macroTaskHeap.top().first;
              Task* t = mt->getNextUnscheduledTask();
              if ( t != NULL )
              {
                TaskPlan plan = m_taskScheduler->scheduleTask( t );
                macroTaskHeap.set( mt, macroTaskHeap.top().second - 1 );
                if ( m_debugLevel >= 10 )
                {
                  logTask( t, "Scheduling" );
                }
                if ( plan.getRunTime() < BWAPI::Broodwar->getFrameCount() + planDistance*2 || ( plan.getRunTime() != NEVER && t->getBuildType().supplyProvided() > 0 ) )
                {
                  m_taskScheduler->finalizeSchedule();
                  if ( m_debugLevel >= 10 )
                  {
                    logTask( t, "Finalizing Schedule" );
                  }
                  m_totalScheduledTypeCount[t->getBuildType()]++;
                  if ( t->getBuildType().supplyProvided() > 0 )
                  {
                    supplyIncrease = true;
                    break;
                  }
                }
                else if ( plan.getRunTime() == NEVER )
                {
                  int insufficientTypes = ( m_taskScheduler->getInsufficientTypes() & ~( BuildTypes::SupplyMask | BuildTypes::WorkerMask ) );
                  if ( insufficientTypes != 0 )
                  {
                    if ( m_debugLevel >= 1 )
                    {
                      log( "Found dependencies while scheduling %s", t->getBuildType().getName().c_str() );
                      if ( m_debugLevel >= 5 )
                      {
                        log( "Timeline:" );
                        log( m_taskScheduler->getTimeline()->toString().c_str() );
                      }
                    }
                    resolveDependencies( insufficientTypes, pmt->first );
                  }
                }
                if ( ( m_taskScheduler->getLastMineralBlockTime() > BWAPI::Broodwar->getFrameCount() + planDistance &&
                       m_taskScheduler->getLastMineralBlockTime() != NEVER ) )
                {
                  tooFarIntoTheFuture = true;
                  break;
                }
              }
              else
              {
                macroTaskHeap.set( mt, 0 );
              }
            }
          }
        }
      }
    }
    // Plan dependencies
    for ( std::map< BuildType, std::pair< int, int > >::iterator i = m_totalNeededTypeCount.begin(); i != m_totalNeededTypeCount.end(); i++ )
    {
      BuildType bt = i->first;
      int neededCount = i->second.first - m_totalPlannedTypeCount[bt];
      int priority = i->second.second;
      if ( neededCount > 0 )
      {
        if ( m_debugLevel >= 1 )
        {
          log( "I need %d %s but I have planned only %d. Building %d additional...", i->second.first, i->first.getName().c_str(), m_totalPlannedTypeCount[bt], neededCount );
        }
        buildAdditional( neededCount, bt, priority );
      }
    }

    // Execute tasks and clean up completed MacroTasks and empty priority levels
    std::set< int > emptyPriorities;
    for ( PMTMap::iterator pmt = m_prioritizedMacroTasks.begin(); pmt != m_prioritizedMacroTasks.end(); pmt++ )
    {
      std::list< MacroTask* >::iterator i2 = pmt->second.techAndUpgradeMacroTasks.begin();
      for ( std::list< MacroTask* >::iterator i = i2; i != pmt->second.techAndUpgradeMacroTasks.end(); i = i2 )
      {
        i2++;
        int incompleteCount = 0;
        for( Task* t : (*i)->getTasks() )
        {
          if ( !t->isCompleted() )
          {
            incompleteCount++;
            if ( t->isWaiting() )
            {
              if ( t->getRunTime() <= BWAPI::Broodwar->getFrameCount() )
              {
                m_taskExecutor->run( t );
              }
            }
          }
        }
        if ( incompleteCount == 0)
        {
          pmt->second.techAndUpgradeMacroTasks.erase( i );
        }
      }

      i2 = pmt->second.unitMacroTasks.begin();
      for ( std::list< MacroTask* >::iterator i = i2; i != pmt->second.unitMacroTasks.end(); i = i2 )
      {
        i2++;
        int incompleteCount = 0;
        for( Task* t : (*i)->getTasks() )
        {
          if ( !t->isCompleted() )
          {
            incompleteCount++;
            if ( t->isWaiting() )
            {
              if ( t->getRunTime() <= BWAPI::Broodwar->getFrameCount() )
              {
                m_taskExecutor->run( t );
              }
            }
          }
        }
        if ( incompleteCount == 0)
        {
          pmt->second.unitMacroTasks.erase( i );
        }
      }
      if ( pmt->second.techAndUpgradeMacroTasks.empty() &&
           pmt->second.unitMacroTasks.empty() )
      {
        emptyPriorities.insert( pmt->first );
      }
    }
    for( int priority : emptyPriorities)
    {
      m_prioritizedMacroTasks.erase( priority );
    }
  }
  void BuildOrderManager::computeTotalPlannedCounts()
  {
    for each( BuildType t in BuildTypes::allBuildTypes( BWAPI::Broodwar->self()->getRace() ) )
    {
      m_totalPlannedTypeCount[t] = BWAPI::Broodwar->self()->completedUnitCount( t.getUnitType() ) + m_taskExecutor->getRunningCount( t );
    }
    for ( PMTMap::iterator pmt = m_prioritizedMacroTasks.begin(); pmt != m_prioritizedMacroTasks.end(); pmt++ )
    {
      for( MacroTask* mt : pmt->second.techAndUpgradeMacroTasks )
      {
        m_totalPlannedTypeCount[mt->getType()] += mt->getWaitingCount();
      }
      for( MacroTask* mt : pmt->second.unitMacroTasks )
      {
        m_totalPlannedTypeCount[mt->getType()] += mt->getWaitingCount();
      }
    }
  }

  MacroTask* BuildOrderManager::build( int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedLocation )
  {
    if ( seedLocation == BWAPI::TilePositions::None )
    {
      seedLocation = BWAPI::Broodwar->self()->getStartLocation();
    }
    MacroTask* mt = new MacroTask( BuildType( t ), priority, false, count, seedLocation );
    m_newMacroTasks.push_back( mt );
    return mt;
  }

  MacroTask* BuildOrderManager::buildAdditional( int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedLocation )
  {
    if ( seedLocation == BWAPI::TilePositions::None )
    {
      seedLocation = BWAPI::Broodwar->self()->getStartLocation();
    }
    MacroTask* mt = new MacroTask( BuildType( t ), priority, true, count, seedLocation );
    m_newMacroTasks.push_back( mt );
    return mt;
  }

  MacroTask* BuildOrderManager::buildAdditional( int count, BuildType t, int priority, BWAPI::TilePosition seedLocation )
  {
    if ( t.isUnitType() )
    {
      return buildAdditional( count, t.getUnitType(), priority, seedLocation );
    }
    else if ( t.isTechType() )
    {
      return research( t.getTechType(), priority );
    }
    else if ( t.isUpgradeType() )
    {
      return upgrade( t.getUpgradeLevel(), t.getUpgradeType(), priority );
    }
    return NULL;
  }

  MacroTask* BuildOrderManager::research( BWAPI::TechType t, int priority )
  {
    MacroTask* mt = new MacroTask( BuildType( t ), priority, true, 1 );
    m_newMacroTasks.push_back( mt );
    return mt;
  }

  MacroTask* BuildOrderManager::upgrade( int level, BWAPI::UpgradeType t, int priority )
  {
    MacroTask* mt = new MacroTask( BuildType( t, level ), priority, true, 1 );
    m_newMacroTasks.push_back( mt );
    return mt;
  }

  void BuildOrderManager::deleteMacroTask( MacroTask* mt )
  {
    // Sanity check
    if ( mt == NULL )
    {
      return;
    }
    // TODO: Implement
  }

  void BuildOrderManager::draw( int x, int y )
  {
    y -= 16;
    for ( PMTMap::iterator pmt = m_prioritizedMacroTasks.begin(); pmt != m_prioritizedMacroTasks.end(); pmt++ )
    {
      for( MacroTask* mt : pmt->second.techAndUpgradeMacroTasks )
      {
        for( Task* t : mt->getTasks() )
        {
          if ( t->getRunTime() < NEVER )
          {
            if ( t->getCompletionTime() > BWAPI::Broodwar->getFrameCount() - 4 * 24 )
            {
              BWAPI::Broodwar->drawTextScreen( x, y += 16, "[ %d ] Task: %s, S = %s, RT = %d",
              pmt->first,
              t->getBuildType().getName().c_str(),
              t->getState().getName().c_str(),
              t->getRunTime() );
            }
          }
        }
      }
      for( MacroTask* mt : pmt->second.unitMacroTasks )
      {
        for( Task* t : mt->getTasks() )
        {
          if ( t->getRunTime() < NEVER )
          {
            if ( t->getCompletionTime() > BWAPI::Broodwar->getFrameCount() - 4 * 24 )
            {
              BWAPI::Broodwar->drawTextScreen( x, y += 16, "[ %d ] Task: %s, S = %s, RT = %d",
              pmt->first,
              t->getBuildType().getName().c_str(),
              t->getState().getName().c_str(),
              t->getRunTime() );
            }
          }
        }
      }
    }
  }
}