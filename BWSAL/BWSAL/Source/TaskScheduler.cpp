#include <BWSAL/TaskScheduler.h>
#include <BWSAL/Task.h>
#include <BWSAL/BuildState.h>
#include <BWSAL/BuildUnit.h>
#include <BWSAL/BuildUnitManager.h>
#include <BWSAL/MetaUnitVariable.h>
#include <BWSAL/BuildEventTimeline.h>
#include <BWSAL/Util.h>
#include <list>
#include <BWAPI.h>
namespace BWSAL
{
  TaskScheduler* TaskScheduler::s_taskScheduler = NULL;
  TaskScheduler* TaskScheduler::create( BuildEventTimeline* timeline, BuildUnitManager* buildUnitManager )
  {
    if ( s_taskScheduler )
    {
      return s_taskScheduler;
    }
    s_taskScheduler = new TaskScheduler();
    s_taskScheduler->m_timeline = timeline;
    s_taskScheduler->m_buildUnitManager = buildUnitManager;
    return s_taskScheduler;
  }

  TaskScheduler* TaskScheduler::getInstance()
  {
    return s_taskScheduler;
  }

  void TaskScheduler::destroy()
  {
    if ( s_taskScheduler )
    {
      delete s_taskScheduler;
    }
  }

  TaskScheduler::TaskScheduler()
  {
    m_debugLevel = 0;
    resetSupplyBlockTime();
    resetLastMineralBlockTime();
    resetLastGasBlockTime();
  }

  TaskScheduler::~TaskScheduler()
  {
    s_taskScheduler = NULL;
  }

  bool TaskScheduler::finalizeSchedule()
  {
    Task* t = m_candidateTask;
    BuildUnit* builder = m_candidatePlan.getBuilder();
    if ( t == NULL || builder == NULL )
    {
      return false;
    }
    int runTime = m_candidatePlan.getRunTime();
    t->m_builder->assign( ( MetaUnit* )builder );
    t->setRunTime( runTime );
    t->setScheduledThisFrame();
    t->setState( TaskStates::Tentatively_Scheduled );
    if ( m_debugLevel >= 1 )
    {
      logTask(t, " SCHEDULED------------" );
    }

    int executeTime = runTime + t->getBuildType().prepTime();
    int builderReleaseTime = executeTime + t->getBuildType().builderTime();
    int completeTime = executeTime + t->getBuildType().buildUnitTime();
    std::list< std::pair< int, BuildEvent > >::iterator timelineIter = m_timeline->begin();
    if ( t->getBuildType().requiresLarva() )
    {
      BuildEvent e( t->getBuildType() );
      e.useLarva( builder );
      timelineIter = m_timeline->addEvent( runTime, e, timelineIter );
    }
    else
    {
      timelineIter = m_timeline->addEvent( runTime, t->getReserveBuilderEvent(), timelineIter );
    }
    timelineIter = m_timeline->addEvent( executeTime, t->getReserveResourcesEvent(), timelineIter );
    if ( t->getBuildType().requiresLarva() == false )
    {
      timelineIter = m_timeline->addEvent( builderReleaseTime, t->getReleaseBuilderEvent(), timelineIter );
    }
    timelineIter = m_timeline->addEvent( completeTime, t->getCompleteBuildTypeEvent(), timelineIter );
    return true;
  }

  inline bool TaskScheduler::canCompleteWithUnitBeforeNextEvent( int validBuildTypeSince,
                                                                 BuildUnit* unit,
                                                                 const Task* t,
                                                                 const std::list< std::pair< int, BuildEvent > >::const_iterator nextEvent )
  {
    BuildType type(t->getBuildType());
    if ( type.requiredAddon() != BuildTypes::None && type.requiredAddon() != unit->m_planningData.m_addon )
    {
      // Builder doesn't have required addon
      return false;
    }
    if ( type.getUnitType().isAddon() && unit->m_planningData.m_addon != BuildTypes::None )
    {
      // Builder already has addon
      return false;
    }
    bool found = t->m_useAnyBuilder;
    if ( !found )
    {
      for( MetaUnit* mu : t->m_possibleBuilders )
      {
        if ( mu->getBuildUnit() == unit )
        {
          found = true;
          break;
        }
      }
    }
    if ( !found )
    {
      // Can't use this build unit
      return false;
    }
    int availableSince = unit->m_planningData.m_availableSince;
    if ( availableSince != NEVER )
    {
      int buildTime = max( type.builderTime(), type.buildUnitTime() );
      int canBuildSince = max( availableSince, validBuildTypeSince - type.prepTime() );
      if ( nextEvent == m_timeline->end() || canBuildSince + buildTime < nextEvent->first )
      {
        return true;
      }
    }
    return false;
  }

  TaskScheduler::HLHPlanData::HLHPlanData()
  {
    candidateNextLarvaSpawnTime = NEVER;
    candidateLarvaCount = 0;
    candidateMorphTime = NEVER;
    candidateMorphed = false;
  }

  void TaskScheduler::resetCandidates( std::map< BuildUnit*, HLHPlanData > *hlhPlans, BuildState* state )
  {
    for ( std::map< BuildUnit*, HLHPlanData >::iterator h = hlhPlans->begin(); h != hlhPlans->end(); h++ )
    {
      h->second.candidateNextLarvaSpawnTime = h->first->m_planningData.m_nextLarvaSpawnTime;
      h->second.candidateLarvaCount = h->first->m_planningData.m_larvaCount;
      h->second.candidateMorphTime = NEVER;
      h->second.candidateMorphed = false;
    }
  }

  void TaskScheduler::initializeHLHPlanData(std::map< BuildUnit*, HLHPlanData > *hlhPlans )
  {
    // Initialize HLH data
    for( BuildUnit* bu : m_buildUnitManager->getUnits() )
    {
      if ( bu->getType().getUnitType().producesLarva() )
      {
        ( *hlhPlans )[bu].candidateNextLarvaSpawnTime = bu->m_currentState.m_nextLarvaSpawnTime;
        ( *hlhPlans )[bu].candidateLarvaCount = bu->m_currentState.m_larvaCount;
      }
    }
  }

  void TaskScheduler::continueToTimeWithLarvaSpawns( BuildState* state, std::map< BuildUnit*, HLHPlanData > *hlhPlans, int time )
  {
    // Continue build state to current time
    state->continueToTime( time );

    // Update HLHPlanData for each HLH
    for ( std::map< BuildUnit*, HLHPlanData >::iterator h = hlhPlans->begin(); h != hlhPlans->end(); h++ )
    {
      // if its time to test the candidate morph
      if ( h->second.candidateMorphTime < time && h->second.candidateMorphed == false )
      {
        while ( h->second.candidateNextLarvaSpawnTime <= h->second.candidateMorphTime )
        {
          h->second.candidateLarvaCount++;

          // If we have three larva, next larva NEVER spawns
          if ( h->second.candidateLarvaCount == 3 )
          {
            h->second.candidateNextLarvaSpawnTime = NEVER;
            break;
          }
          else
          {
            h->second.candidateNextLarvaSpawnTime += LARVA_SPAWN_TIME;
          }
        }

        // try our candidate morph time
        h->second.candidateLarvaCount--;
        h->second.candidateMorphed = true;

        // detect invalid time line
        if ( h->second.candidateLarvaCount < 0 )
        {
          // revert to existing timeline
          h->second.candidateLarvaCount = h->first->m_planningData.m_larvaCount;
          h->second.candidateNextLarvaSpawnTime = h->first->m_planningData.m_nextLarvaSpawnTime;

          // assume we can morph with the next larva
          h->second.candidateMorphTime = h->first->m_planningData.m_nextLarvaSpawnTime;
          h->second.candidateMorphed = false;
        }
      }

      // Continue candidate timeline to current time
      while ( h->second.candidateNextLarvaSpawnTime <= state->getTime() )
      {
        h->second.candidateLarvaCount++;

        // We have three larva, next larva NEVER spawns
        if ( h->second.candidateLarvaCount == 3 )
        {
          h->second.candidateNextLarvaSpawnTime = NEVER;
          break;
        }
        else
        {
          h->second.candidateNextLarvaSpawnTime += LARVA_SPAWN_TIME;
        }
      }
    }
  }

  void TaskScheduler::findCandidateMorphTimes( std::map< BuildUnit*, HLHPlanData > *hlhPlans, int validBuildTimeSince )
  {  
    for ( std::map< BuildUnit*, HLHPlanData >::iterator h = hlhPlans->begin(); h != hlhPlans->end(); h++ )
    {
      // if we don't yet have a candidate morph time
      if ( h->second.candidateMorphTime == NEVER )
      {
        if ( h->first->m_planningData.m_larvaCount > 0 )
        {
          h->second.candidateMorphTime = validBuildTimeSince;
          h->second.candidateLarvaCount = h->first->m_planningData.m_larvaCount;
          h->second.candidateNextLarvaSpawnTime = h->first->m_planningData.m_nextLarvaSpawnTime;
          if ( h->first->m_planningData.m_larvaCount == 3 ) // Decreasing from 3 larva - start making the next larva
          {
            h->second.candidateNextLarvaSpawnTime = h->second.candidateMorphTime + LARVA_SPAWN_TIME;
          }
          h->second.candidateLarvaCount--;
          h->second.candidateMorphed = true;
        }
        else
        {
          h->second.candidateMorphTime = max( validBuildTimeSince, h->first->m_planningData.m_nextLarvaSpawnTime );
          h->second.candidateLarvaCount = h->first->m_planningData.m_larvaCount;
          h->second.candidateNextLarvaSpawnTime = h->first->m_planningData.m_nextLarvaSpawnTime;
          h->second.candidateMorphed = false;
        }
        if ( m_debugLevel >= 10)
        {
          log( "set candidate morph time to %d", h->second.candidateMorphTime );
        }
      }
      else
      {
        if ( m_debugLevel >= 10)
        {
          log( "candidate morph time is already %d", h->second.candidateMorphTime );
        }
      }
    }
  }

  TaskPlan TaskScheduler::scheduleLarvaUsingTask( Task* t )
  {
    // State/Planning information is stored in BuildState and in the BuildUnits themselves
    BuildState state = m_timeline->m_initialState;

    BuildType buildType = t->getBuildType();
    // reset assignments and run time for this task
    t->getBuilder()->assign( NULL );
    if ( t->getSecondBuilder() != NULL )
    {
      t->getSecondBuilder()->assign( NULL );
    }
    t->setRunTime( NEVER );
    std::map< BuildUnit*, HLHPlanData > hlhPlans;

    initializeHLHPlanData( &hlhPlans );
    if ( m_debugLevel >= 5)
    {
      logTask(t, "scheduleLarvaUsingTask" );
      for ( std::map<BuildUnit*, HLHPlanData>::iterator h = hlhPlans.begin(); h != hlhPlans.end(); h++ )
      {
        BuildUnit* bu = h->first;
        log("[%d] LC=%d, NLST=%d",state.getTime(), bu->m_planningData.m_larvaCount, bu->m_planningData.m_nextLarvaSpawnTime );
      }
    }

    int validBuildTypeSince = NEVER;
    std::list< std::pair< int, BuildEvent > >::iterator nextEvent = m_timeline->begin();

    updateLastBlockTimes( &state, buildType );

    if ( state.getInsufficientTypes( buildType ) == 0 )
    {
      if ( state.getMinerals() >= buildType.mineralPrice() && state.getGas() >= buildType.gasPrice() && state.getTime() >= t->getEarliestStartTime() )
      {
        validBuildTypeSince = state.getTime();
      }
      else
      {
        if ( m_debugLevel >= 10 )
        {
          if ( state.getMinerals() < buildType.mineralPrice() )
          {
            log( "resetting candidates because of insufficient minerals: %f < %d", state.getMinerals(), buildType.mineralPrice() );
          }
          else if ( state.getGas() < buildType.gasPrice() )
          {
            log( "resetting candidates because of insufficient gas: %f < %d", state.getGas(), buildType.gasPrice() );
          }
          else
          {
            log( "resetting candidates because too early: %d < %d", state.getTime(), t->getEarliestStartTime() );
          }
        }
        int nextSatisfiedTime = max( t->getEarliestStartTime(), state.getNextTimeWithMinimumResources( buildType. mineralPrice(), buildType.gasPrice() ) );
        if ( nextEvent == m_timeline->end() || nextSatisfiedTime < nextEvent->first )
        {
          if ( m_debugLevel >= 10 )
          {
            log( "continuing to next satisfied time which is at %d", nextSatisfiedTime );
          }

          state.continueToTime( nextSatisfiedTime );
          validBuildTypeSince = state.getTime();
        }
      }
    }
    else
    {
      if ( state.isSupplyBlocked( t ) )
      {
        m_supplyBlockTime = min( m_supplyBlockTime, state.getTime() );
      }
    }


    if ( validBuildTypeSince != NEVER )
    {
      findCandidateMorphTimes( &hlhPlans, validBuildTypeSince );
    }
    // Iterate over the timeline of events
    while ( nextEvent != m_timeline->end() )
    {
      continueToTimeWithLarvaSpawns( &state, &hlhPlans, nextEvent->first );
      state.doEvent( nextEvent->second );
      updateLastBlockTimes( &state, buildType );
      if ( m_debugLevel >= 10)
      {
        log( "[%d] m=%f, g=%f, s=%d", state.getTime(), state.getMinerals(), state.getGas(), state.getSupplyAvailable() ); 
        for ( std::map<BuildUnit*, HLHPlanData>::iterator h = hlhPlans.begin(); h != hlhPlans.end(); h++ )
        {
          BuildUnit* bu = h->first;
          log( "[%d] AS=%d, LC=%d, NLST=%d, CMT=%d", state.getTime(), bu->m_planningData.m_availableSince, bu->m_planningData.m_larvaCount, bu->m_planningData.m_nextLarvaSpawnTime,h->second.candidateMorphTime );
        }
      }

      // Handle use larva events
      if ( nextEvent->second.getUseLarva() != NULL )
      {
        BuildUnit* h = nextEvent->second.getUseLarva();
        if ( hlhPlans[h].candidateLarvaCount == 3 )
        {
          hlhPlans[h].candidateNextLarvaSpawnTime = state.getTime() + LARVA_SPAWN_TIME;
        }
        hlhPlans[h].candidateLarvaCount--;
        if ( hlhPlans[h].candidateLarvaCount < 0 )
        {
          hlhPlans[h].candidateMorphTime = h->m_planningData.m_nextLarvaSpawnTime;
          hlhPlans[h].candidateNextLarvaSpawnTime = h->m_planningData.m_nextLarvaSpawnTime;
          hlhPlans[h].candidateLarvaCount = h->m_planningData.m_larvaCount;
          hlhPlans[h].candidateMorphed = false;
        }
      }
      nextEvent++;
      if ( state.getInsufficientTypes( buildType ) != 0 )
      {
        if ( state.isSupplyBlocked( t ) )
        {
          m_supplyBlockTime = min( m_supplyBlockTime, state.getTime() );
        }
        validBuildTypeSince = NEVER;
        // Throw away our candidate solutions
        resetCandidates( &hlhPlans, &state );
        if ( m_debugLevel >= 10)
        {
          log( "resetting candidates because of insufficient build types: %d", state.getInsufficientTypes( buildType ) );
        }
        continue;
      }

      // If we don't have enough minerals or gas on this event
      if ( state.getMinerals() < buildType.mineralPrice() || state.getGas() < buildType.gasPrice() || state.getTime() < t->getEarliestStartTime() )
      {
        validBuildTypeSince = NEVER;
        // Throw away our candidate solutions
        resetCandidates( &hlhPlans, &state );
        if ( m_debugLevel >= 10 )
        {
          if ( state.getMinerals() < buildType.mineralPrice() )
          {
            log( "resetting candidates because of insufficient minerals: %f < %d", state.getMinerals(), buildType.mineralPrice() );
          }
          else if ( state.getGas() < buildType.gasPrice() )
          {
            log( "resetting candidates because of insufficient gas: %f < %d", state.getGas(), buildType.gasPrice() );
          }
          else
          {
            log( "resetting candidates because too early: %d < %d", state.getTime(), t->getEarliestStartTime() );
          }
        }

        // See if we will have enough minerals and gas before the next event
        int nextSatisfiedTime = max( t->getEarliestStartTime(), state.getNextTimeWithMinimumResources( buildType.mineralPrice(), buildType.gasPrice() ) );
        if ( nextEvent == m_timeline->end() || nextSatisfiedTime < nextEvent->first )
        {
          if ( nextSatisfiedTime == NEVER )
          {
            // Right now it doesn't look like we'll ever have enough resources
            // But continue to the next event. Who knows, maybe we've planned a worker or refinery
            if ( m_debugLevel >= 10 )
            {
              log( "continuing to next event to find a satisfied time" );
            }
            continue;
          }
          if ( m_debugLevel >= 10 )
          {
            log( "continuing to next satisfied time which is at %d", nextSatisfiedTime );
          }
          // If so, continue to that point and time and update validBuildTypeSince
          continueToTimeWithLarvaSpawns( &state, &hlhPlans, nextSatisfiedTime );
        }
        else
        {
          if ( m_debugLevel >= 10 )
          {
            log( "nextSatisfiedTime occurs after next event: %d >= %d", nextSatisfiedTime, nextEvent->first );
          }
          // Otherwise, we will need to continue processing events until we find a valid time
          continue;
        }
      }
      // If we get to here, we have a valid build time, so set it if its not set
      if ( validBuildTypeSince == NEVER )
      {
        validBuildTypeSince = state.getTime();
      }
      if ( m_debugLevel >= 10 )
      {
        log( "validBuildTypeSince = %d", validBuildTypeSince );
        log( "findCandidateMorphTimes" );
      }
      findCandidateMorphTimes( &hlhPlans, validBuildTypeSince );
    }

    // Pick the candidate with the earliest morph time to schedule with
    BuildUnit* candidateUnit = NULL;
    int candidateTime = NEVER;
    for ( std::map< BuildUnit*, HLHPlanData >::iterator h = hlhPlans.begin(); h != hlhPlans.end(); h++ )
    {
      if ( h->second.candidateMorphTime < candidateTime )
      {
        candidateTime = h->second.candidateMorphTime;
        candidateUnit = h->first;
      }
    }
    if ( m_debugLevel >= 10 )
    {
     log( "final candidate morph time is %d", candidateTime );
     log( "final candidate builder is %x", candidateUnit );
    }
    if ( candidateUnit != NULL )
    {
      m_candidateTask = t;
      m_candidatePlan.m_builder = candidateUnit;
      m_candidatePlan.m_runTime = candidateTime;
    }
    if ( candidateTime == NEVER )
    {
      //We're going to return NEVER, record reason
      m_insufficientTypes = state.getInsufficientTypes( buildType );
      if ( state.getGas() < buildType.gasPrice() && state.getGasWorkers() == 0 )
      {
        if ( ( state.getCompletedBuildTypes() & BuildTypes::RefineryMask ) == 0 )
        {
          m_insufficientTypes |= BuildTypes::RefineryMask;
        }
        else
        {
          m_insufficientTypes |= BuildTypes::WorkerMask;
        }
      }
      if ( m_insufficientTypes == 0 &&
           state.getMinerals() >= buildType.mineralPrice() &&
           state.getGas() >= buildType.gasPrice() &&
           state.getTime() >= t->getEarliestStartTime() )
      {
        // If we have enough build types, minerals, gas, and supply, but still can't build
        // then we need a builder
        m_insufficientTypes = buildType.whatBuilds().first.getMask();
      }
    }

    return m_candidatePlan;
  }

  TaskPlan TaskScheduler::scheduleTask( Task* t )
  {
    // Reset insufficient types bitfield
    m_insufficientTypes = 0;
    // Reset build unit planning data
    m_buildUnitManager->resetPlanningData();
    m_candidateTask = t;
    m_candidatePlan.m_builder = NULL;
    m_candidatePlan.m_runTime = NEVER;

    // Sanity check
    if ( t == NULL )
    {
      return m_candidatePlan;
    }

    if ( t->isScheduledThisFrame() )
    {
      // Already scheduled, return the time
      return m_candidatePlan;
    }

    // Tasks that use larva are very different from other tasks
    if ( t->getBuildType().requiresLarva() )
    {
      return scheduleLarvaUsingTask( t );
    }

    // State/Planning information is stored in BuildState and in the BuildUnits themselves
    BuildState state = m_timeline->m_initialState;

    BuildType buildType = t->getBuildType();
    BuildType builderType = buildType.whatBuilds().first;


    int buildTime = buildType.buildUnitTime();

    // Reset assignments and run time for this task
    t->getBuilder()->assign( NULL );
    if ( t->getSecondBuilder() != NULL )
    {
      t->getSecondBuilder()->assign( NULL );
    }
    t->setRunTime( NEVER );

    int validBuildTypeSince = NEVER;
    std::list< std::pair< int, BuildEvent > >::iterator nextEvent = m_timeline->begin();

    // The set of units that can execute this task
    // ( except in the case of add - ons, but we take care of that later )

    BuildUnit* candidateUnit = NULL;
    int candidateTime = NEVER;

    updateLastBlockTimes( &state, buildType );

    if ( state.getInsufficientTypes( buildType ) == 0 )
    {
      if ( state.getMinerals() >= buildType.mineralPrice() && state.getGas() >= buildType.gasPrice() && state.getTime() >= t->getEarliestStartTime() )
      {
        validBuildTypeSince = state.getTime();
      }
      else
      {
        int nextSatisfiedTime = max( t->getEarliestStartTime(), state.getNextTimeWithMinimumResources( buildType.mineralPrice(), buildType.gasPrice() ) );
        if ( nextEvent == m_timeline->end() || nextSatisfiedTime < nextEvent->first )
        {
          state.continueToTime( nextSatisfiedTime );
          validBuildTypeSince = state.getTime();
        }
      }
    }
    else
    {
      if ( state.isSupplyBlocked( t ) )
      {
        m_supplyBlockTime = min( m_supplyBlockTime, state.getTime() );
      }
    }
    if ( validBuildTypeSince != NEVER )
    {
      for ( BuildUnit* bu : m_buildUnitManager->getUnits() )
      {
        if ( bu->m_planningData.m_type == t->getBuildType().whatBuilds().first &&
             canCompleteWithUnitBeforeNextEvent( validBuildTypeSince, bu, t, nextEvent ) )
        {
          int availableSince = bu->m_planningData.m_availableSince;
          int canBuildSince = max( availableSince, validBuildTypeSince - t->getBuildType().prepTime() );
          if ( canBuildSince < candidateTime && canBuildSince < NEVER )
          {
            candidateUnit = bu;
            candidateTime = canBuildSince;
          }
        }
      }
    }
    // Iterate over the timeline of events
    while ( nextEvent != m_timeline->end() )
    {
      state.continueToTime( nextEvent->first );
      state.doEvent( nextEvent->second );
      updateLastBlockTimes( &state, buildType );
      nextEvent++;
      if ( state.getInsufficientTypes( buildType ) != 0 )
      {
        if ( state.isSupplyBlocked( t ) )
        {
          m_supplyBlockTime = min( m_supplyBlockTime, state.getTime() );
        }
        validBuildTypeSince = NEVER;
        candidateUnit = NULL;
        candidateTime = NEVER;
        continue;
      }
      // Take into accout the fact that pulling a worker off minerals to build something will
      // reduce the amount of minerals we're going to have because that worker is not mining
      double candidateMinerals = state.getMinerals();
      if ( candidateTime != NEVER )
      {
        if ( state.getTime() > candidateTime + buildType.prepTime() + buildType.builderTime() && buildType.getRace() != BWAPI::Races::Zerg )
        {
          candidateMinerals -= ( buildType.prepTime() + buildType.builderTime() ) * MINERALS_PER_WORKER_PER_FRAME;
        }
        else
        {
          candidateMinerals -= ( state.getTime() - candidateTime ) * MINERALS_PER_WORKER_PER_FRAME;
        }
      }
      // If we don't have enough minerals or gas on this event
      if ( candidateMinerals < buildType.mineralPrice() || state.getGas() < buildType.gasPrice() || state.getTime() < t->getEarliestStartTime() )
      {
        // Throw away our candidate unit
        candidateUnit = NULL;
        candidateTime = NEVER;
        validBuildTypeSince = NEVER;

        // See if we will have enough minerals and gas before the next event
        int nextSatisfiedTime = max( t->getEarliestStartTime(), state.getNextTimeWithMinimumResources( buildType.mineralPrice(), buildType.gasPrice() ) );
        if ( nextEvent == m_timeline->end() || nextSatisfiedTime < nextEvent->first )
        {
          if ( nextSatisfiedTime == NEVER )
          {
            // Right now it doesn't look like we'll ever have enough resources
            // But continue to the next event. Who knows, maybe we've planned a worker or refinery
            continue;
          }
          // if so, continue to that point and time and update validBuildTypeSince
          state.continueToTime( nextSatisfiedTime );
        }
        else
        {
          // Otherwise, we will need to continue processing events until we find a valid time
          continue;
        }
      }

      // If we get to here, we have a valid build time, so set it if its not set
      if ( validBuildTypeSince == NEVER )
      {
        validBuildTypeSince = state.getTime();
      }
      if ( candidateUnit == NULL )
      {
        // We need to find a candidate unit
        for ( BuildUnit* bu : m_buildUnitManager->getUnits() )
        {
          if ( bu->m_planningData.m_type == t->getBuildType().whatBuilds().first )
          {
            if ( canCompleteWithUnitBeforeNextEvent( validBuildTypeSince, bu, t, nextEvent ) )
            {
              int availableSince = bu->m_planningData.m_availableSince;
              int canBuildSince = max( availableSince, validBuildTypeSince - t->getBuildType().prepTime() );
              if ( canBuildSince < candidateTime && canBuildSince < NEVER )
              {
                candidateUnit = bu;
                candidateTime = canBuildSince;
              }
            }
          }
        }
      }
    }

    // Now that we've traversed the entire timeline we can safely schedule our task if we still have a candidate
    if ( candidateUnit != NULL )
    {
      m_candidateTask = t;
      m_candidatePlan.m_builder = candidateUnit;
      m_candidatePlan.m_runTime = candidateTime;
    }

    if ( candidateTime == NEVER )
    {
      //We're going to return NEVER, record reason
      m_insufficientTypes = state.getInsufficientTypes( buildType );
      if ( state.getGas() < buildType.gasPrice() && state.getGasWorkers() == 0 )
      {
        if ( ( state.getCompletedBuildTypes() & BuildTypes::RefineryMask ) == 0 )
        {
          m_insufficientTypes |= BuildTypes::RefineryMask;
        }
        else
        {
          m_insufficientTypes |= BuildTypes::WorkerMask;
        }
      }
      if ( m_insufficientTypes == 0 &&
           state.getMinerals() >= buildType.mineralPrice() &&
           state.getGas() >= buildType.gasPrice() &&
           state.getTime() >= t->getEarliestStartTime() )
      {
        // If we have enough build types, minerals, gas, and supply, but still can't build
        // then we need a builder
        m_insufficientTypes = buildType.whatBuilds().first.getMask();
      }
    }

    return m_candidatePlan;
  }

  int TaskScheduler::getInsufficientTypes()
  {
    return m_insufficientTypes;
  }

  int TaskScheduler::getSupplyBlockTime() const
  {
    return m_supplyBlockTime;
  }

  void TaskScheduler::resetSupplyBlockTime()
  {
    m_supplyBlockTime = NEVER;
  }

  int TaskScheduler::getLastMineralBlockTime() const
  {
    return m_lastMineralBlockTime;
  }

  void TaskScheduler::resetLastMineralBlockTime()
  {
    m_lastMineralBlockTime = NEVER;
  }

  int TaskScheduler::getLastGasBlockTime() const
  {
    return m_lastGasBlockTime;
  }

  void TaskScheduler::resetLastGasBlockTime()
  {
    m_lastGasBlockTime = NEVER;
  }
  BuildEventTimeline* TaskScheduler::getTimeline() const
  {
    return m_timeline;
  }

  void TaskScheduler::updateLastBlockTimes( BuildState* state, BuildType type )
  {
    if ( state->getMinerals() < 50 && type.mineralPrice() > 0 )
    {
      if ( m_lastMineralBlockTime == NEVER )
      {
        m_lastMineralBlockTime = state->getTime();
      }
      else
      {
        m_lastMineralBlockTime = max( m_lastMineralBlockTime, state->getTime() );
      }
    }
    if ( state->getGas() < 50 && type.gasPrice() > 0 )
    {
      if ( m_lastGasBlockTime == NEVER )
      {
        m_lastGasBlockTime = state->getTime();
      }
      else
      {
        m_lastGasBlockTime = max( m_lastGasBlockTime, state->getTime() );
      }
    }
  }
}