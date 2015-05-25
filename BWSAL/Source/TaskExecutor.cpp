#include <BWSAL/TaskExecutor.h>
#include <BWSAL/Task.h>
#include <BWSAL/MetaUnitVariable.h>
#include <BWSAL/BuildUnit.h>
#include <BWSAL/BuildType.h>
#include <BWSAL/BuildingPlacer.h>
#include <BWSAL/ReservedMap.h>
#include <BWSAL/BuildEventTimeline.h>
#include <BWSAL/Util.h>
#include <BWAPI.h>
#include <algorithm>
namespace BWSAL
{
  int TaskExecutorBidLevel = 50;
  TaskExecutor* TaskExecutor::s_taskExecutor = NULL;
  TaskExecutor* TaskExecutor::create( UnitArbitrator* arbitrator, BuildEventTimeline* timeline, ReservedMap* reservedMap, BuildingPlacer* defaultBuildingPlacer )
  {
    if ( s_taskExecutor )
    {
      return s_taskExecutor;
    }
    s_taskExecutor = new TaskExecutor();
    s_taskExecutor->m_arbitrator = arbitrator;
    s_taskExecutor->m_reservedMap = reservedMap;
    s_taskExecutor->m_defaultBuildingPlacer = defaultBuildingPlacer;
    s_taskExecutor->m_timeline = timeline;
    return s_taskExecutor;
  }

  TaskExecutor* TaskExecutor::getInstance()
  {
    return s_taskExecutor;
  }

  void TaskExecutor::destroy()
  {
    if ( s_taskExecutor )
    {
      delete s_taskExecutor;
    }
  }

  TaskExecutor::TaskExecutor()
  {
  }

  TaskExecutor::~TaskExecutor()
  {
    s_taskExecutor = NULL;
  }

  void TaskExecutor::run( Task* t )
  {
    // Sanity check
    if ( t == NULL )
    {
      return;
    }
    t->setState( TaskStates::Aquiring );
    // Reserve the minerals and gas
    m_timeline->m_initialState.reservedMinerals += t->getBuildType().mineralPrice();
    m_timeline->m_initialState.reservedGas += t->getBuildType().gasPrice();
    m_runningTasks.push_back( t );
    m_runningCount[t->getBuildType()]++;
  }

  int TaskExecutor::getRunningCount( BuildType type ) const
  {
    std::map<BuildType, int>::const_iterator i = m_runningCount.find(type);
    if ( i != m_runningCount.end() )
    {
      return i->second;
    }
    return 0;
  }

  void TaskExecutor::onFrame()
  {
    // Update the runnng tasks
    std::list< Task* >::iterator i = m_runningTasks.begin();
    std::list< Task* >::iterator i2 = i;
    for (; i != m_runningTasks.end(); i = i2 )
    {
      i2++;
      updateTask( *i );
      if ( (*i)->isWaiting() || (*i)->isCompleted() )
      {
		  m_runningCount[(*i)->getBuildType()]--;
		  m_runningTasks.erase( i );
      }
    }
  }

  void TaskExecutor::updateTask( Task* t )
  {
    BuildUnit*         builder = NULL;
    BWAPI::Unit BWAPI_builder = NULL;
    BuildUnit*         secondBuilder = NULL;
    BWAPI::Unit BWAPI_secondBuilder = NULL;
    BuildUnit*         createdUnit = NULL;
    BWAPI::Unit BWAPI_createdUnit = NULL;
    BuildUnit*         secondCreatedUnit = NULL;
    BWAPI::Unit BWAPI_secondCreatedUnit = NULL;

    BuildType buildType = t->getBuildType();

    if ( t->getBuilder() != NULL )
    {
      builder = t->getBuilder()->getBuildUnit();
      if ( builder != NULL )
      {
        BWAPI_builder = builder->getUnit();
      }
    }
    if ( BWAPI_builder == NULL )
    {
      logTask( t, "Error: BWAPI_builder == NULL" );
    }
    if ( BWAPI_builder != NULL && buildType.requiresLarva() && BWAPI_builder->getType().producesLarva() )
    {
      // We need to find a larva
      const BWAPI::Unitset larva = BWAPI_builder->getLarva();
      for( BWAPI::Unit u : larva )
      {
        // We want a larva that hasn't been claimed by any other tasks
        if ( BuildUnit::getBuildUnitIfExists( u ) == NULL )
        {
          if ( larva.size() == 3 )
          {
            builder->m_currentState.m_nextLarvaSpawnTime = m_timeline->m_initialState.getTime() + LARVA_SPAWN_TIME;
          }
          builder->m_currentState.m_larvaCount--;
          BuildUnit::setBuildUnit( u, t->m_createdUnit );
          t->assignBuilder( t->m_createdUnit );
          builder = t->getBuilder()->getBuildUnit();
          BWAPI_builder = builder->getUnit();
          logTask( t, "Switching builder to larva" );
          break;
        }
      }
      if ( BWAPI_builder->getType().producesLarva() )
      {
        logTask( t, "-> Not Scheduled - Didn't find a larva!" );
        t->setState( TaskStates::Not_Scheduled );
        return;
      }
    }

    // if builder dies and we're making an extractor, find the extractor and use that as the builder
    if ( BWAPI_builder != NULL && BWAPI_builder->exists() == false && buildType == BuildTypes::Zerg_Extractor )
    {
      logTask( t, "Need to find extractor" );
      for each ( BWAPI::Unit u in BWAPI::Broodwar->getUnitsInRadius( BWAPI::Position( t->getBuildLocation() ), 100 ) )
      {
        if ( u->getType() == BWAPI::UnitTypes::Zerg_Extractor )
        {
          logTask( t, "Found extractor" );
          builder = BuildUnit::getBuildUnit( u );
          BWAPI_builder = builder->getUnit();
          t->assignBuilder( builder );
          m_arbitrator->setBid( this, BWAPI_builder, TaskExecutorBidLevel );
          break;
        }
      }
    }

    if ( t->getSecondBuilder() != NULL )
    {
      secondBuilder = t->getSecondBuilder()->getBuildUnit();
      if ( secondBuilder != NULL )
      {
        BWAPI_secondBuilder = secondBuilder->getUnit();
      }
    }
    if ( buildType.createsUnit() )
    {
      if ( buildType.requiresLarva() )
      {
        computeSecondCreatedUnit( t );
      }
      else
      {
        computeCreatedUnit( t );
      }
    }
    createdUnit = t->getCreatedUnit();
    if ( createdUnit != NULL )
    {
      BWAPI_createdUnit = createdUnit->getUnit();
    }
    secondCreatedUnit = t->getSecondCreatedUnit();
    if ( secondCreatedUnit != NULL )
    {
      BWAPI_secondCreatedUnit = createdUnit->getUnit();
    }

    BWAPI::UnitType builderType = buildType.whatBuilds().first.getUnitType();
    int now = BWAPI::Broodwar->getFrameCount();

    bool hasBuilder = ( m_builders.find( BWAPI_builder ) != m_builders.end() || buildType == BuildTypes::Zerg_Extractor ) &&
                      m_arbitrator->hasBid( BWAPI_builder ) &&
                      m_arbitrator->getHighestBidder( BWAPI_builder ).first == this;
    bool hasAllBuilders = hasBuilder;
    bool hasAtLeastOneBuilder = hasBuilder;
    bool doesNotHaveBuilders = BWAPI_builder == NULL ||
                               ( BWAPI_builder->isCompleted() == false &&
                                 BWAPI_builder->isConstructing() == false &&
                                 BWAPI_builder->isBeingConstructed() == false &&
                                 BWAPI_builder->isMorphing() == false ) ||
                               m_arbitrator->getHighestBidder( BWAPI_builder ).second > TaskExecutorBidLevel;

    if ( secondBuilder != NULL )
    {
      bool hasSecondBuilder = m_builders.find( BWAPI_secondBuilder ) != m_builders.end() &&
                              m_arbitrator->hasBid( BWAPI_secondBuilder ) &&
                              m_arbitrator->getHighestBidder( BWAPI_secondBuilder ).first == this;
      hasAllBuilders = hasAllBuilders && hasSecondBuilder;
      hasAtLeastOneBuilder = hasAtLeastOneBuilder || hasSecondBuilder;
      doesNotHaveBuilders = doesNotHaveBuilders ||
                            BWAPI_secondBuilder == NULL ||
                            BWAPI_secondBuilder->isCompleted() == false ||
                            m_arbitrator->getHighestBidder( BWAPI_secondBuilder ).second > TaskExecutorBidLevel;
    }
    bool createdUnitExists = BWAPI_createdUnit != NULL && BWAPI_createdUnit->exists();

    // Handle transitions
    if ( t->getState() == TaskStates::Aquiring )
    {
      if ( doesNotHaveBuilders )
      {
        logTask( t, "-> Not Scheduled" );
        t->setState( TaskStates::Not_Scheduled );

        // Return the minerals + gas we reserved, we won't need them after all :( 
        m_timeline->m_initialState.reservedMinerals -= t->getBuildType().mineralPrice();
        m_timeline->m_initialState.reservedGas -= t->getBuildType().gasPrice();

        // Log the reason we don't have builders
        if ( BWAPI_builder == NULL )
        {
          logTask( t, "Reason: BWAPI_builder == NULL" );
        }
        else if ( BWAPI_builder->isCompleted() == false &&
                  BWAPI_builder->isConstructing() == false &&
                  BWAPI_builder->isBeingConstructed() == false &&
                  BWAPI_builder->isMorphing() == false )
        {
          logTask( t, "Reason: Not completed, constructing, being constructed, or morphing" );
        }
        else if ( m_arbitrator->getHighestBidder( BWAPI_builder ).second > TaskExecutorBidLevel )
        {
          logTask( t, "Reason: Outbid! - %s stole our unit!", m_arbitrator->getHighestBidder( BWAPI_builder ).first->getName().c_str() );
        }
      }
      else if ( hasAllBuilders )
      {
        // TARGET AQUIRED! I mean builder. Builder( s ) aquired. Time to prepare to execute our task
        logTask( t, "-> Preparing" );
        t->setState( TaskStates::Preparing );
      }
    }
    else if ( t->getState() == TaskStates::Preparing )
    {
      if ( doesNotHaveBuilders )
      {
        logTask( t, "-> Not Scheduled" );
        t->setState( TaskStates::Not_Scheduled );

        // Return the minerals + gas we reserved, we won't need them after all :( 
        m_timeline->m_initialState.reservedMinerals -= t->getBuildType().mineralPrice();
        m_timeline->m_initialState.reservedGas -= t->getBuildType().gasPrice();
      }
      else if ( buildType.isBuilding( BWAPI_builder, BWAPI_secondBuilder, BWAPI_createdUnit ) )
      {
        // We've started building! Lets note the execute time and switch to TaskStates::Executing.
        t->setExecuteTime( now );
        logTask( t, "-> Executing" );
        logTask( t, "isMorphing = %d, isConstructing = %d", BWAPI_builder->isMorphing(), BWAPI_builder->isConstructing() );
        t->setState( TaskStates::Executing );
      }
    }
    else if ( t->getState() == TaskStates::Executing )
    {
      if ( !hasAtLeastOneBuilder )
      {
        logTask( t, "-> Not Scheduled ( no builder )" );
        logTask( t, "Build type %s, hasBid = %d, isHighestBidder = %x, this = %x",
                 buildType.getName().c_str(),
                 m_arbitrator->hasBid( BWAPI_builder ),
                 m_arbitrator->getHighestBidder( BWAPI_builder ).first,
                 this );
        t->setState( TaskStates::Not_Scheduled );
        // Can't return the minerals + gas we reserved since we already spent them :( 
      }
      else if ( builderType == BWAPI::UnitTypes::Protoss_Probe && BWAPI_builder->isIdle() )
      {
        // The probe has started the construction so we can release it and switch to TaskStates::Warping
        logTask( t, "-> Warping" );
        m_arbitrator->removeBid( this, BWAPI_builder );
        t->setState( TaskStates::Warping );
      }
      else if ( builderType == BWAPI::UnitTypes::Terran_SCV && !hasAllBuilders )
      {
        // We no longer have an SCV, switching to Halted.
        logTask( t, "-> Halted" );
        t->setState( TaskStates::Halted );
      }
      else if ( buildType.isCompleted( BWAPI_builder, BWAPI_secondBuilder, BWAPI_createdUnit, BWAPI_secondCreatedUnit ) )
      {
        // Task Complete! Release the builder( s ) and note the completion time
        logTask( t, "-> Completed" );
        t->setState( TaskStates::Completed );
        m_arbitrator->removeBid( this, BWAPI_builder );
        if ( BWAPI_secondBuilder != NULL )
        {
          m_arbitrator->removeBid( this, BWAPI_secondBuilder );
        }
        t->setCompletionTime( now );
      }
      else if ( ( !t->getBuildType().createsUnit() || !createdUnitExists ) &&
                !buildType.isBuilding( BWAPI_builder, BWAPI_secondBuilder, BWAPI_createdUnit ) )
      {
        if ( now > t->getExecuteTime() + 20 )
        {
          // Something went wrong, log stuff and give up
          logTask( t, "-> Not Scheduled: Created unit: %x Created unit exists: %d", BWAPI_createdUnit, createdUnitExists );
          logTask( t, "%s:  builder = %x, second builder = %x, created unit = %x",
                   buildType.getName().c_str(),
                   BWAPI_builder,
                   BWAPI_secondBuilder,
                   BWAPI_createdUnit );
          logTask( t, " isMorphing = %d, isConstructing = %d, isCompleted = %d, getBuildType() = %s",
                   BWAPI_builder->isMorphing(),
                   BWAPI_builder->isConstructing(),
                   BWAPI_builder->isCompleted(),
                   BWAPI_builder->getBuildType().getName().c_str() );
          t->setState( TaskStates::Not_Scheduled );
        }
      }
    }
    else if ( t->getState() == TaskStates::Warping )
    {
      if ( t->getBuildType().createsUnit() && !createdUnitExists )
      {
        // The building we were warping no longer exists
        logTask( t, "-> Not Scheduled" );
        t->setState( TaskStates::Not_Scheduled );
      }
      else if ( buildType.isCompleted( BWAPI_builder, BWAPI_secondBuilder, BWAPI_createdUnit, BWAPI_secondCreatedUnit ) )
      {
        // Task Complete! Note the completion time ( builder is already released )
        logTask( t, "-> Completed" );
        t->setState( TaskStates::Completed );
        t->setCompletionTime( now );
      }
    }
    else if ( t->getState() == TaskStates::Halted )
    {
      logTask( t, "Task Halted! Error: Halt state not implemented yet." );
      // Transition to Executing once we have a builter
      // Transition to Not_Scheduled if our created unit gets destroyed
    }

    // Process current state
    if ( t->getState() == TaskStates::Aquiring )
    /**************************************** AQUIRING ****************************************/
    {
      int expectedExecuteTime = max( t->getRunTime() + buildType.prepTime(), now );
      int expectedReleaseTime = expectedExecuteTime + buildType.builderTime();
      int expectedCompletionTime = expectedExecuteTime + buildType.buildUnitTime();
      if ( builder != NULL )
      {
        m_arbitrator->setBid( this, BWAPI_builder, TaskExecutorBidLevel );
        if ( buildType.morphsBuilder() )
        {
          builder->m_currentState.m_type = buildType;
        }
        builder->m_currentState.m_availableSince = expectedReleaseTime;
      }
      if ( secondBuilder != NULL )
      {
        m_arbitrator->setBid( this, BWAPI_secondBuilder, TaskExecutorBidLevel );
        secondBuilder->m_currentState.m_type = builderType;
        secondBuilder->m_currentState.m_availableSince = expectedReleaseTime;
      }
      if ( createdUnit != NULL )
      {
        createdUnit->m_currentState.m_type = buildType;
        createdUnit->m_currentState.m_availableSince = expectedCompletionTime;
      }
      m_timeline->addEvent( expectedCompletionTime, t->getCompleteBuildTypeEvent() );
    }
    else if ( t->getState() == TaskStates::Preparing )
    /**************************************** PREPARING ****************************************/
    {
      // If the task has specified a custom building placer, use that, otherwise use the default building placer
      BuildingPlacer* placer = t->m_buildingPlacer != NULL ? t->m_buildingPlacer : m_defaultBuildingPlacer;
      if ( t->getBuildLocation() == BWAPI::TilePositions::None && buildType.needsBuildLocation() )
      {
        if ( !buildType.requiresPsi() || BWAPI::Broodwar->self()->completedUnitCount( BWAPI::UnitTypes::Protoss_Pylon ) > 0 )
        {
          BWAPI::TilePosition buildLocation = placer->findBuildLocation( m_reservedMap, buildType.getUnitType(), t->getSeedLocation(), BWAPI_builder );
          if ( buildLocation != BWAPI::TilePositions::None )
          {
            t->setBuildLocation( buildLocation );
            m_reservedMap->reserveTiles( buildLocation, buildType.getUnitType() );
          }
        }
      }
      if ( t->getBuildLocation() != BWAPI::TilePositions::None &&
		  !BWAPI::Broodwar->canBuildHere(t->getBuildLocation(), buildType.getUnitType(), BWAPI_builder) &&
           t->isRelocatable() )
      {
        if ( !buildType.requiresPsi() || BWAPI::Broodwar->self()->completedUnitCount( BWAPI::UnitTypes::Protoss_Pylon ) > 0 )
        {
          BWAPI::TilePosition buildLocation = placer->findBuildLocation( m_reservedMap, buildType.getUnitType(), t->getBuildLocation(), BWAPI_builder );
          if ( buildLocation != BWAPI::TilePositions::None )
          {
            m_reservedMap->freeTiles( t->getBuildLocation(), buildType.getUnitType() );
            t->setBuildLocation( buildLocation );
            m_reservedMap->reserveTiles( buildLocation, buildType.getUnitType() );
          }
        }
      }
      if ( m_timeline->m_initialState.m_time > t->getLastOrderTime() + 6 )
      {
        if ( !buildType.isPreparing( BWAPI_builder, BWAPI_secondBuilder ) )
        {
          if ( !buildType.needsBuildLocation() )
          {
            buildType.build( BWAPI_builder, BWAPI_secondBuilder, t->getBuildLocation() );
            t->setLastOrderTime();
          }
          else
          {
            if ( t->getBuildLocation() != BWAPI::TilePositions::None )
            {
              t->setLastOrderTime();
              if ( buildType.build( BWAPI_builder, BWAPI_secondBuilder, t->getBuildLocation() ) == false )
              {
                BWAPI::Position buildPosition( t->getBuildLocation() );
                buildPosition.x += buildType.getUnitType().tileWidth() * 16;
                buildPosition.y += buildType.getUnitType().tileHeight() * 16;
                // Tell the builder to move to the build location
                BWAPI_builder->move( buildPosition );
              }
            }
          }
        }
      }
      int expectedExecuteTime = max( t->getRunTime() + buildType.prepTime(), now );
      int expectedReleaseTime = expectedExecuteTime + buildType.builderTime();
      int expectedCompletionTime = expectedExecuteTime + buildType.buildUnitTime();
      if ( builder != NULL )
      {
        m_arbitrator->setBid( this, BWAPI_builder, TaskExecutorBidLevel );
        if ( buildType.morphsBuilder() )
        {
          builder->m_currentState.m_type = buildType;
        }
        builder->m_currentState.m_availableSince = expectedReleaseTime;
      }
      if ( secondBuilder != NULL )
      {
        m_arbitrator->setBid( this, BWAPI_secondBuilder, TaskExecutorBidLevel );
        secondBuilder->m_currentState.m_type = builderType;
        secondBuilder->m_currentState.m_availableSince = expectedReleaseTime;
      }
      if ( createdUnit != NULL )
      {
        createdUnit->m_currentState.m_type = buildType;
        createdUnit->m_currentState.m_availableSince = expectedCompletionTime;
      }
      m_timeline->addEvent( expectedCompletionTime, t->getCompleteBuildTypeEvent() );
    }
    else if ( t->getState() == TaskStates::Executing )
    /**************************************** EXECUTING ****************************************/
    {
      int expectedReleaseTime = max( t->getExecuteTime() + buildType.builderTime(), now );
      int expectedCompletionTime = max( t->getExecuteTime() + buildType.buildUnitTime(), now + buildType.remainingTime( BWAPI_builder, BWAPI_secondBuilder, BWAPI_createdUnit ) );
      if ( builder != NULL )
      {
        if (BWAPI_builder != NULL && BWAPI_builder->getTrainingQueue().size()>1 && (m_timeline->m_initialState.m_time - t->getExecuteTime()) % 5 == 0)
        {
          BWAPI::UnitType ut = BWAPI_builder->getTrainingQueue().back();
          m_timeline->m_initialState.reservedMinerals += ut.mineralPrice();
          m_timeline->m_initialState.reservedGas += ut.gasPrice();
          BWAPI_builder->cancelTrain();
        }
        m_arbitrator->setBid( this, BWAPI_builder, TaskExecutorBidLevel );
        if ( buildType.morphsBuilder() )
        {
          builder->m_currentState.m_type = buildType;
        }
        builder->m_currentState.m_availableSince = expectedReleaseTime;
      }
      if ( secondBuilder != NULL )
      {
        m_arbitrator->setBid( this, BWAPI_secondBuilder, TaskExecutorBidLevel );
        secondBuilder->m_currentState.m_type = builderType;
        secondBuilder->m_currentState.m_availableSince = expectedReleaseTime;
      }
      if ( createdUnit != NULL )
      {
        createdUnit->m_currentState.m_type = buildType;
        createdUnit->m_currentState.m_availableSince = expectedCompletionTime;
      }
      m_timeline->addEvent( expectedCompletionTime, t->getCompleteBuildTypeEvent() );
    }
    else if ( t->getState() == TaskStates::Warping )
    /**************************************** WARPING ****************************************/
    {
      int expectedCompletionTime = max( t->getExecuteTime() + buildType.buildUnitTime(), now + buildType.remainingTime( BWAPI_builder, BWAPI_secondBuilder, BWAPI_createdUnit ) );

      createdUnit->m_currentState.m_type = buildType;
      createdUnit->m_currentState.m_availableSince = expectedCompletionTime;

      m_timeline->addEvent( expectedCompletionTime, t->getCompleteBuildTypeEvent() );
    }
    else if ( t->getState() == TaskStates::Halted )
    /**************************************** HALTED ****************************************/
    {
      // TODO: Implement Terran halted construction state
      // Find a nearby SCV and make it our builder.
    }
  }
  void TaskExecutor::computeCreatedUnit( Task* t )
  {
    // Sanity check
    if ( t == NULL ||
         t->getBuilder() == NULL ||
         t->getBuilder()->getBuildUnit() == NULL ||
         t->getBuilder()->getBuildUnit()->getUnit() == NULL ||
         t->getCreatedUnit() == NULL ||
         t->getBuildType().createsUnit() == false )
      return;

    BWAPI::Unit builder = t->getBuilder()->getBuildUnit()->getUnit();
    BWAPI::UnitType ut = t->getBuildType().getUnitType();
    BWAPI::Unit buildUnit = t->getCreatedUnit()->getUnit();

    // If the building dies, or isn't the right type, set it to null
    if ( !( buildUnit != NULL && buildUnit->exists() && ( buildUnit->getType() == ut || buildUnit->getBuildType() == ut ) ) )
    {
      buildUnit = NULL;
    }

    if ( t->getBuildLocation() != BWAPI::TilePositions::None )
    {
      if ( buildUnit == NULL && ut.isBuilding() ) // if we don't have a building yet, look for it
      {
        BWAPI::TilePosition bl = t->getBuildLocation();
        // Look at the units on the tile to see if it exists yet
        for each ( BWAPI::Unit u in BWAPI::Broodwar->getUnitsOnTile( bl.x, bl.y ) )
        {
          if ( u->getType() == ut && !u->isLifted() )
          {
            // We found the building
            buildUnit = u;
            break;
          }
        }
      }
      if ( buildUnit == NULL && ut.isAddon() ) // If we don't have a building yet, look for it
      {
        BWAPI::TilePosition bl = t->getBuildLocation();
        bl.x += 4;
        bl.y++;
        for each ( BWAPI::Unit u in BWAPI::Broodwar->getUnitsOnTile( bl.x, bl.y ) )
        {
          if ( u->getType() == ut && !u->isLifted() )
          {
            // we found the building
            buildUnit = u;
            break;
          }
        }
      }
    }

    if ( builder->exists() && builder->isCompleted() && builder->getBuildUnit() != NULL && builder->getBuildUnit()->exists() && ( builder->getBuildUnit()->getType() == ut || builder->getBuildUnit()->getBuildType() == ut ) )
    {
      buildUnit = builder->getBuildUnit();
    }

    if ( builder->getAddon() != NULL && builder->getAddon()->exists() && ( builder->getAddon()->getType() == ut || builder->getAddon()->getBuildType() == ut ) )
    {
      buildUnit = builder->getAddon();
    }

    // check to see if the worker is the right type
    // Zerg_Nydus_Canal is special since Zerg_Nydus_Canal can construct Zerg_Nydus_Canal
    BuildUnit* currentBuildUnit = BuildUnit::getBuildUnitIfExists( buildUnit );
    if ( currentBuildUnit == NULL )
    {
      BuildUnit::setBuildUnit( buildUnit, t->getCreatedUnit() );
    }
  }

  void TaskExecutor::computeSecondCreatedUnit( Task* t )
  {
    // Sanity check
    if ( t == NULL ||
         t->getBuilder() == NULL ||
         t->getBuilder()->getBuildUnit() == NULL ||
         t->getBuilder()->getBuildUnit()->getUnit() == NULL ||
         t->getSecondCreatedUnit() == NULL ||
         t->getBuildType().createsUnit() == false )
      return;

    BWAPI::Unit builder = t->getBuilder()->getBuildUnit()->getUnit();
    BWAPI::UnitType ut = t->getBuildType().getUnitType();
    BWAPI::Unit buildUnit = t->getSecondCreatedUnit()->getUnit();

    // If the building dies, or isn't the right type, set it to null
    if ( !( buildUnit != NULL && buildUnit->exists() && ( buildUnit->getType() == ut || buildUnit->getBuildType() == ut ) ) )
    {
      buildUnit = NULL;
    }

    if ( buildUnit == NULL && t->getBuildType().createsUnit() && t->getBuildType().requiresLarva() )
    {
      BWAPI::Unit closestValidUnit = NULL;
      double closestDistance = 100000;
      // Not a perfect way of determining the build unit but good enough for now
      for each ( BWAPI::Unit u in BWAPI::Broodwar->getUnitsInRadius( builder->getPosition(), 200 ) )
      {
        if ( u->getType() == ut && u->exists() && u->isCompleted() && BuildUnit::getBuildUnitIfExists( u ) == NULL )
        {
          double dist = builder->getDistance( u );
          if ( dist < closestDistance )
          {
            closestDistance = dist;
            closestValidUnit = u;
          }
          break;
        }
      }
      if ( closestValidUnit != NULL )
      {
        buildUnit = closestValidUnit;
      }
    }

    // check to see if the worker is the right type
    // Zerg_Nydus_Canal is special since Zerg_Nydus_Canal can construct Zerg_Nydus_Canal
    BuildUnit* currentBuildUnit = BuildUnit::getBuildUnitIfExists( buildUnit );
    if ( currentBuildUnit == NULL )
    {
      BuildUnit::setBuildUnit( buildUnit, t->getSecondCreatedUnit() );
    }
  }

  void TaskExecutor::onOffer( std::set< BWAPI::Unit > units )
  {
    m_arbitrator->accept( this, units );
    for( BWAPI::Unit u : units )
    {
      m_builders.insert( u );
    }
  }

  void TaskExecutor::onRevoke( BWAPI::Unit unit, double bid )
  {
    m_builders.erase( unit );
  }

  std::string TaskExecutor::getName() const
  {
    return "Task Executor";
  }
}