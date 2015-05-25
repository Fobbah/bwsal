#include <BWSAL/BuildState.h>
#include <BWSAL/MetaUnit.h>
#include <BWSAL/BuildUnit.h>
#include <BWSAL/BuildUnitManager.h>
#include <BWSAL/WorkerManager.h>
#include <BWSAL/Task.h>
#include <BWSAL/Util.h>
#include <BWAPI.h>
namespace BWSAL
{
  BuildState::BuildState()
  {
    m_time = 0;
    m_minerals = 0;
    m_gas = 0;
    m_supplyTotal = 0;
    m_supplyUsed = 0;
    m_mineralWorkers = 0;
    m_gasWorkers = 0;
    reservedMinerals = 0;
    reservedGas = 0;
  }

  void BuildState::continueToTime( int time )
  {
    int deltaTime = time - m_time;
    // Sanity check, can't go back in time!
    if ( deltaTime < 0 )
    {
      return;
    }
    m_minerals += deltaTime * m_mineralWorkers * MINERALS_PER_WORKER_PER_FRAME;
    m_gas += deltaTime * m_gasWorkers * GAS_PER_WORKER_PER_FRAME;
    m_time = time;

    for( BuildUnit* larvaProducer : BuildUnitManager::getInstance()->getUnits() )
    {
      if ( larvaProducer->getType().getUnitType().producesLarva() )
      {
        // Continue current timeline to current time
        while ( larvaProducer->m_planningData.m_nextLarvaSpawnTime <= time )
        {
          larvaProducer->m_planningData.m_larvaCount++;

          if ( larvaProducer->m_planningData.m_larvaCount == 3 )
          {
            // If we have three larva, next larva NEVER spawns
            larvaProducer->m_planningData.m_nextLarvaSpawnTime = NEVER;
            break;
          }
          else
          {
            larvaProducer->m_planningData.m_nextLarvaSpawnTime += LARVA_SPAWN_TIME;
          }
        }
      }
    }
  }

  int BuildState::getNextTimeWithMinimumResources( int minerals, int gas ) const
  {
    int time = m_time;
    if ( m_minerals < minerals )
    {
      if ( m_mineralWorkers == 0 )
      {
        // We need minerals but have no workers on minerals, so we'll never have enough minerals!
        return NEVER;
      }
      time = m_time + (int)( ( minerals - m_minerals ) / ( m_mineralWorkers * MINERALS_PER_WORKER_PER_FRAME ) ) + 1; // + 1 -> ceil
    }
    if ( m_gas < gas )
    {
      if ( m_gasWorkers == 0 )
      {
        // We need gas but have no workers on gas, so we'll never have enough gas!
        return NEVER;
      }
      time = max( time, m_time + (int)( ( gas - m_gas ) / ( m_gasWorkers * GAS_PER_WORKER_PER_FRAME ) ) ) + 1; // + 1 -> ceil
    }
    return time;
  }

  int BuildState::getInsufficientTypes( BuildType buildType ) const
  {
    int result = (buildType.getRequiredMask() & ~m_completedBuildTypes);
    if ( m_supplyTotal < m_supplyUsed + buildType.supplyRequired() )
    {
      result |= BuildTypes::SupplyMask;
    }
    return result;
  }

  bool BuildState::isSupplyBlocked( Task* t ) const
  {
    return ( m_minerals >= t->getBuildType().mineralPrice() &&
             m_gas >= t->getBuildType().gasPrice() &&
             m_time >= t->getEarliestStartTime() &&
             ( ( t->getBuildType().getRequiredMask() & ~m_completedBuildTypes ) == 0 ) &&
             m_supplyTotal < m_supplyUsed + t->getBuildType().supplyRequired() );
  }

  void BuildState::doEvent( BuildEvent& e )
  {
    m_minerals += e.m_deltaMinerals;
    m_gas += e.m_deltaGas;
    if ( e.m_deltaSupply >= 0 )
    {
      m_supplyTotal += e.m_deltaSupply;
    }
    else
    {
      m_supplyUsed += -e.m_deltaSupply;
    }
    if ( e.m_nowUnavailable1.second != NULL )
    {
      e.m_nowUnavailable1.second->m_planningData.m_type = e.m_nowUnavailable1.first;
      e.m_nowUnavailable1.second->m_planningData.m_availableSince = NEVER;
      if ( e.m_nowUnavailable1.first.getUnitType().isWorker() )
      {
        m_mineralWorkers--;
      }
    }
    if ( e.m_nowUnavailable2.second != NULL )
    {
      e.m_nowUnavailable2.second->m_planningData.m_type = e.m_nowUnavailable2.first;
      e.m_nowUnavailable2.second->m_planningData.m_availableSince = NEVER;
    }
    if ( e.m_nowAvailable1.second != NULL )
    {
      e.m_nowAvailable1.second->m_planningData.m_type = e.m_nowAvailable1.first;
      e.m_nowAvailable1.second->m_planningData.m_availableSince = m_time;
      if ( e.m_nowAvailable1.first.getUnitType().isWorker() && e.m_nowAvailable1.first.getRace() != BWAPI::Races::Zerg )
      {
        m_mineralWorkers++;
      }
    }
    if ( e.m_nowAvailable2.second != NULL )
    {
      e.m_nowAvailable2.second->m_planningData.m_type = e.m_nowAvailable2.first;
      e.m_nowAvailable2.second->m_planningData.m_availableSince = m_time;
    }
    if ( e.m_completingBuildTypes & BuildTypes::RefineryMask )
    {
      // We're completing a refinery, transfer up to 3 workers
      int transferAmt = max( min( 3, m_mineralWorkers - 1 ), 0 );
      m_mineralWorkers -= transferAmt;
      m_gasWorkers += transferAmt;
    }
    else if ( e.m_completingBuildTypes & BuildTypes::WorkerMask )
    {
      // We're completing a worker, assume it will be used to mine minerals
      m_mineralWorkers++;
    }
    // Update the completed build types in the BuildState with the build types that are completing in this event
    m_completedBuildTypes |= e.m_completingBuildTypes;

    if ( e.m_useLarva != NULL )
    {
      // Event is using a larva owned by e.m_useLarva
      e.m_useLarva->m_planningData.m_larvaCount--;

      // Set the next larva spawn time, if applicable
      if ( e.m_useLarva->m_planningData.m_larvaCount == 2 )
      {
        e.m_useLarva->m_planningData.m_nextLarvaSpawnTime = m_time + LARVA_SPAWN_TIME;
      }
    }
    if ( e.m_addonOfUnit != NULL )
    {
      // Event is making an addon for e.m_addonOfUnit
      e.m_addonOfUnit->m_planningData.m_addon = e.m_buildType.getUnitType();
    }
  }

  void BuildState::updateWithCurrentGameState()
  {
    // Update the build state so it represents the current BWAPI::Game state

    // Set time, minerals, gas, and supply
    m_time = BWAPI::Broodwar->getFrameCount();
    m_minerals = (double)( BWAPI::Broodwar->self()->gatheredMinerals() + BWAPI::Broodwar->self()->refundedMinerals() - BWAPI::Broodwar->self()->repairedMinerals() - reservedMinerals );
    m_gas = (double)( BWAPI::Broodwar->self()->gatheredGas() + BWAPI::Broodwar->self()->refundedGas() - BWAPI::Broodwar->self()->repairedGas() - reservedGas );
    m_supplyTotal = BWAPI::Broodwar->self()->supplyTotal();
    m_supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
    BWAPI::Race r = BWAPI::Broodwar->self()->getRace();

    // Update data for all existing build units
    for( BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits() )
    {
      BuildUnit *bu = BuildUnit::getBuildUnitIfExists( u );
      if ( bu == NULL )
      {
        continue;
      }

      // set time to m_time + max of these 4 remaining times:
      int time = m_time + max( max( u->getRemainingBuildTime(), u->getRemainingTrainTime() ),
                               max( u->getRemainingResearchTime(), u->getRemainingUpgradeTime() ) );
      bu->m_currentState.m_type = bu->getType();
      bu->m_currentState.m_availableSince = time;
      if ( u->getAddon() != NULL )
      {
        bu->m_currentState.m_addon = u->getAddon()->getType();
      }
      else
      {
        bu->m_currentState.m_addon = BWAPI::UnitTypes::None;
      }
      if ( u->getType().producesLarva() )
      {
        if ( !resourceDepotIsCompleted( u ) )
        {
          bu->m_currentState.m_nextLarvaSpawnTime = m_time + u->getRemainingBuildTime() + 5;
          bu->m_currentState.m_larvaCount = 0;
        }
        else
        {
          // Compute unclaimed larva count
          int count = 0;
          const BWAPI::Unitset larva = u->getLarva();
          for( BWAPI::Unit l : larva )
          {
            if ( BuildUnit::getBuildUnitIfExists( l ) == NULL )
            {
              count++;
            }
          }
          bu->m_currentState.m_larvaCount = count;
          if ( count < 3 )
          {
            if ( bu->m_currentState.m_nextLarvaSpawnTime == NEVER )
            {
              bu->m_currentState.m_nextLarvaSpawnTime = m_time + u->getRemainingTrainTime() + 5;
            }
            else
            {
              bu->m_currentState.m_nextLarvaSpawnTime = max( bu->m_currentState.m_nextLarvaSpawnTime, m_time + u->getRemainingTrainTime() + 5 );
            }
          }
          else
          {
            bu->m_currentState.m_nextLarvaSpawnTime = NEVER;
          }
        }
      }
    }

    // Update data for no-longer-existing build units
    for( BuildUnit* bu : BuildUnitManager::getInstance()->getUnits() )
    {
      if ( bu->getUnit() != NULL && bu->getUnit()->exists() == false )
      {
        bu->m_currentState.m_availableSince = NEVER;
        bu->m_currentState.m_addon = BWAPI::UnitTypes::None;
      }
    }
    m_mineralWorkers = WorkerManager::getInstance()->mineralWorkerCount();
    m_gasWorkers = WorkerManager::getInstance()->gasWorkerCount();
    m_completedBuildTypes = 0;
    // Update completed build types
    for( BuildType t : BuildTypes::allBuildTypes() )
    {
      if ( t.isTechType() )
      {
        if ( BWAPI::Broodwar->self()->hasResearched( t.getTechType() ) )
        {
          m_completedBuildTypes |= t.getMask();
        }
      }
      else if ( t.isUnitType() )
      {
        if ( BWAPI::Broodwar->self()->completedUnitCount( t.getUnitType() ) > 0 )
        {
          m_completedBuildTypes |= t.getMask();
        }
      }
      else if ( t.isUpgradeType() )
      {
        if ( BWAPI::Broodwar->self()->getUpgradeLevel( t.getUpgradeType() ) >= t.getUpgradeLevel() )
        {
          m_completedBuildTypes |= t.getMask();
        }
      }
    }
    if ( BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg )
    {
      if ( BWAPI::Broodwar->self()->allUnitCount( BWAPI::UnitTypes::Zerg_Lair ) > 0 )
      {
        m_completedBuildTypes |= BuildTypes::Zerg_Hatchery.getMask();
      }
      if ( BWAPI::Broodwar->self()->allUnitCount( BWAPI::UnitTypes::Zerg_Hive ) > 0 )
      {
        m_completedBuildTypes |= BuildTypes::Zerg_Hatchery.getMask();
        m_completedBuildTypes |= BuildTypes::Zerg_Lair.getMask();
      }
    }
  }

  void BuildState::createUnclaimedBuildUnits()
  {
    for( BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits() )
    {
      // Don't make build units for larva
      // Tasks will claim them as they execute
      if ( u->getType() != BWAPI::UnitTypes::Zerg_Larva )
      {
        BuildUnit *bu = BuildUnit::getBuildUnit( u );

        // set time to m_time + max of these 4 remaining times:
        int time = m_time + max( max( u->getRemainingBuildTime(), u->getRemainingTrainTime() ),
                                 max( u->getRemainingResearchTime(), u->getRemainingUpgradeTime() ) );
        if ( bu->m_currentState.m_availableSince < time )
        {
          bu->m_currentState.m_availableSince = time;
        }
      }
    }
  }

  int BuildState::getTime() const
  {
    return m_time;
  }

  double BuildState::getMinerals() const
  {
    return m_minerals;
  }

  double BuildState::getGas() const
  {
    return m_gas;
  }

  int BuildState::getSupplyAvailable() const
  {
    return m_supplyTotal - m_supplyUsed;
  }

  int BuildState::getSupplyTotal() const
  {
    return m_supplyTotal;
  }

  int BuildState::getSupplyUsed() const
  {
    return m_supplyUsed;
  }

  int BuildState::getMineralWorkers() const
  {
    return m_mineralWorkers;
  }

  int BuildState::getGasWorkers() const
  {
    return m_gasWorkers;
  }

  unsigned int BuildState::getCompletedBuildTypes() const
  {
    return m_completedBuildTypes;
  }
}
