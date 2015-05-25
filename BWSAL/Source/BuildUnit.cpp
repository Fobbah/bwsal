#include <BWSAL/BuildUnit.h>
#include <BWSAL/BuildUnitManager.h>
#include <BWSAL/Task.h>
#include <BWSAL/Types.h>
#include <BWAPI.h>
namespace BWSAL
{
  BuildUnit* BuildUnit::getBuildUnit()
  {
    return this;
  }

  BuildUnit::BuildUnit( Task* task )
  {
    m_task = task;
    m_unit = NULL;
    initializeInformation();
    BuildUnitManager::getInstance()->addUnit(this);
  }

  BuildUnit::BuildUnit( BWAPI::Unit unit )
  {
    m_unit = unit;
    m_task = NULL;
    initializeInformation();
    BuildUnitManager::getInstance()->addUnit(this);
  }

  void BuildUnit::initializeInformation()
  {
    m_currentState.m_availableSince = 0;
    m_currentState.m_type = getType();
    m_currentState.m_nextLarvaSpawnTime = NEVER;
    m_currentState.m_larvaCount = 0;
    m_currentState.m_addon = BWAPI::UnitTypes::None;
    m_planningData = m_currentState;
  }

  bool BuildUnit::isReal() const
  {
    return m_unit != NULL;
  }

  BuildType BuildUnit::getType() const
  {
    if ( m_unit != NULL )
    {
      return m_unit->getType();
    }
 
    if ( m_task != NULL )
    {
      return m_task->getBuildType();
    }
    return BuildTypes::None;
  }

  BWAPI::Unit BuildUnit::getUnit() const
  {
    return m_unit;
  }

  Task* BuildUnit::getTask() const
  {
    return m_task;
  }

  void BuildUnit::setUnit( BWAPI::Unit unit )
  {
    m_unit = unit;
  }

  void BuildUnit::setTask( Task* task )
  {
    m_task = task;
  }

  BuildUnit* BuildUnit::getBuildUnit( BWAPI::Unit unit )
  {
    // Sanity check
    if ( unit == NULL )
    {
      return NULL;
    }

    BuildUnit* buildUnit = ( BuildUnit* )unit->getClientInfo();
    if ( buildUnit == NULL )
    {
      buildUnit = new BuildUnit( unit );
      unit->setClientInfo( buildUnit );
      BuildUnitManager::getInstance()->addUnit( buildUnit );
    }
    return buildUnit;
  }

  BuildUnit* BuildUnit::getBuildUnitIfExists( BWAPI::Unit unit )
  {
    // Sanity check
    if ( unit == NULL )
    {
      return NULL;
    }

    return ( BuildUnit* )unit->getClientInfo();
  }

  void BuildUnit::setBuildUnit( BWAPI::Unit unit, BuildUnit* buildUnit )
  {
    // Sanity check
    if ( unit == NULL || buildUnit == NULL )
    {
      return;
    }
    unit->setClientInfo( buildUnit );
    buildUnit->m_unit = unit;
  }
}