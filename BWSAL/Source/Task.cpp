#include <BWSAL/Task.h>
#include <BWSAL/MetaUnit.h>
#include <BWSAL/BuildUnit.h>
#include <BWSAL/BuildEvent.h>
#include <BWSAL/MetaUnitVariable.h>
#include <BWAPI.h>
#include <BWSAL/TaskExecutor.h>
#include <BWSAL/Types.h>
#include <sstream>

namespace BWSAL
{
  int Task::s_nextFreeTaskID = 1;
  Task::Task( BuildType type )
  {
    m_earliestStartTime = 0;
    m_runTime = NEVER;
    m_executeTime = NEVER;
    m_completionTime = NEVER;
    m_lastScheduledTime = NEVER;
    m_relocatable = true;
    m_lastOrderTime = -1;

    m_state = TaskStates::Not_Scheduled;
    m_builder = new MetaUnitVariable();
    if ( type.whatBuilds().second == 2 )
    {
      m_secondBuilder = new MetaUnitVariable();
    }
    else
    {
      m_secondBuilder = NULL;
    }
    if ( type.createsUnit() || type.requiresLarva() )
    {
      m_createdUnit = new BuildUnit( this );
    }
    else
    {
      m_createdUnit = NULL;
    }
    if ( type.createsUnit() && type.requiresLarva() )
    {
      m_secondCreatedUnit = new BuildUnit( this );
    }
    else
    {
      m_secondCreatedUnit = NULL;
    }
    m_type = type;
    m_useAnyBuilder = true;
    m_buildLocation = BWAPI::TilePositions::None;
    m_seedLocation = BWAPI::Broodwar->self()->getStartLocation();
    m_buildingPlacer = NULL;
    id = s_nextFreeTaskID++;
  }

  int Task::getID() const
  {
    return id;
  }

  std::string Task::toString() const
  {
    std::stringstream ss;
    ss << "[Task ";
    if ( id < 100 )
    {
      ss << " ";
      if ( id < 10 )
      {
        ss << " ";
      }
    }
    ss << id << "] ";
    if ( m_type.isUnitType() )
    {
      ss << "Build ";
    }
    else if ( m_type.isTechType() )
    {
      ss << "Research ";
    }
    else if ( m_type.isUpgradeType() )
    {
      ss << "Upgrade ";
    }
    ss << m_type.getName().c_str();
    ss << " with builder " << m_builder->getBuildUnit();
    if ( m_secondBuilder != NULL )
    {
      ss << " and second builder " << m_secondBuilder->getBuildUnit();
    }
    ss << " [RT = ";
    if ( m_runTime == NEVER )
    {
      ss << "NEVER";
    }
    else
    {
      ss << m_runTime;
    }
    ss << ", ET = ";
    if ( m_executeTime == NEVER )
    {
      ss << "NEVER";
    }
    else
    {
      ss << m_executeTime;
    }
    ss << ", CT = ";
    if ( m_completionTime == NEVER )
    {
      ss << "NEVER";
    }
    else
    {
      ss << m_completionTime;
    }
    ss << "] ";
    ss << "STATE = " << m_state.getName().c_str();
    return ss.str();
  }

  BuildType Task::getBuildType() const
  {
    return m_type;
  }

  bool Task::isScheduledThisFrame() const
  {
    return m_lastScheduledTime == BWAPI::Broodwar->getFrameCount();
  }

  void Task::setScheduledThisFrame()
  {
    m_lastScheduledTime = BWAPI::Broodwar->getFrameCount();
  }

  int Task::getEarliestStartTime() const
  {
    return m_earliestStartTime;
  }

  void Task::setEarliestStartTime( int time )
  {
    m_earliestStartTime = time;
  }

  int Task::getRunTime() const
  {
    return m_runTime;
  }

  void Task::setRunTime( int time )
  {
    m_runTime = time;
  }

  int Task::getExecuteTime() const
  {
    return m_executeTime;
  }

  void Task::setExecuteTime( int time )
  {
    m_executeTime = time;
  }

  int Task::getCompletionTime() const
  {
    return m_completionTime;
  }

  void Task::setCompletionTime( int time )
  {
    m_completionTime = time;
  }

  int Task::getLastOrderTime() const
  {
    return m_lastOrderTime;
  }

  void Task::setLastOrderTime()
  {
    m_lastOrderTime = BWAPI::Broodwar->getFrameCount();
  }

  bool Task::isRelocatable() const
  {
    return m_relocatable;
  }

  void Task::setRelocatable( bool relocatable )
  {
    m_relocatable = relocatable;
  }

  bool Task::isWaiting() const
  {
    return m_state.isWaiting();
  }

  bool Task::isRunning() const
  {
    return m_state.isRunning();
  }

  bool Task::isCompleted() const
  {
    return m_state.isCompleted();
  }

  TaskState Task::getState() const
  {
    return m_state;
  }

  void Task::setState( TaskState state )
  {
    m_state = state;
    if ( m_state == TaskStates::Not_Scheduled )
    {
      m_runTime = NEVER;
    }
  }

  BWAPI::TilePosition Task::getBuildLocation() const
  {
    return m_buildLocation;
  }

  void Task::setBuildLocation( BWAPI::TilePosition buildLocation )
  {
    m_buildLocation = buildLocation;
  }

  BWAPI::TilePosition Task::getSeedLocation() const
  {
    return m_seedLocation;
  }

  void Task::setSeedLocation( BWAPI::TilePosition seedLocation )
  {
    m_seedLocation = seedLocation;
  }

  MetaUnitVariable* Task::getBuilder() const
  {
    return m_builder;
  }

  MetaUnitVariable* Task::getSecondBuilder() const
  {
    return m_secondBuilder;
  }

  BuildUnit* Task::getCreatedUnit() const
  {
    return m_createdUnit;
  }

  BuildUnit* Task::getSecondCreatedUnit() const
  {
    return m_secondCreatedUnit;
  }

  BuildEvent Task::getReserveBuilderEvent() const
  {
    BuildEvent e( m_type );
    e.setBuildUnitUnavailable( m_type.whatBuilds().first, m_builder->getBuildUnit() );
    if ( m_type.whatBuilds().second == 2 )
    {
      e.setBuildUnitUnavailable( m_type.whatBuilds().first, m_secondBuilder->getBuildUnit() );
    }
    return e;
  }

  BuildEvent Task::getReserveResourcesEvent() const
  {
    return BuildEvent( m_type, -m_type.mineralPrice(), -m_type.gasPrice(), -m_type.supplyRequired() );
  }

  BuildEvent Task::getReleaseBuilderEvent() const
  {
    BuildEvent e( m_type );
    if ( m_type.morphsBuilder() )
    {
      e.setBuildUnitAvailable( m_type, m_builder->getBuildUnit() );
    }
    else
    {
      e.setBuildUnitAvailable( m_type.whatBuilds().first, m_builder->getBuildUnit() );
    }
    return e;
  }

  BuildEvent Task::getCompleteBuildTypeEvent() const
  {
    BuildEvent e( m_type, 0, 0, m_type.supplyProvided() );
    if ( m_type.morphsBuilder() )
    {
      if ( m_createdUnit != NULL )
      {
        e.setBuildUnitAvailable( m_type, m_createdUnit );
        e.setCompletedBuildType( m_type );
      }
      if ( m_secondCreatedUnit != NULL )
      {
        // zerglings && sourge
        e.setBuildUnitAvailable( m_type, m_secondCreatedUnit );
        e.setCompletedBuildType( m_type );
      }
      e.setCompletedBuildType( m_type );
    }
    else
    {
      // tech/research/construction/production
      e.setCompletedBuildType( m_type );
      if ( m_createdUnit != NULL )
      {
        // normal production/construction
        e.setBuildUnitAvailable( m_type, m_createdUnit );
        if ( m_type.getUnitType().isAddon() )
        {
          e.setAddon( m_builder->getBuildUnit() );
        }
      }
    }
    return e;
  }

  void Task::assignBuilder( MetaUnit* builder )
  {
    m_possibleBuilders.clear();
    m_possibleBuilders.insert( builder );
    m_builder->assign( builder );
    m_useAnyBuilder = false;
  }

  void Task::assignBuilders( MetaUnit* builder, MetaUnit* secondBuilder )
  {
    m_possibleBuilders.clear();
    m_possibleBuilders.insert( builder );
    m_possibleBuilders.insert( secondBuilder );

    // If we need a builder and secondBuilder, assign both of them
    if ( m_secondBuilder != NULL )
    {
      m_builder->assign( builder );
      m_secondBuilder->assign( secondBuilder );
    }
    // Otherwise, assume we just have 2 possibilities for the first builder
    m_useAnyBuilder = false;
  }

  void Task::assignBuilders( std::set< MetaUnit* > builders )
  {
    m_possibleBuilders = builders;
    m_useAnyBuilder = false;
  }

  void Task::useAnyBuilder()
  {
    m_possibleBuilders.clear();
    m_useAnyBuilder = true;
  }

  bool Task::canUseAnyBuilder() const
  {
    return m_useAnyBuilder;
  }
}