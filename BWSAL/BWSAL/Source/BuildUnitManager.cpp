#include <BWSAL/BuildUnitManager.h>
namespace BWSAL
{
  BuildUnitManager* BuildUnitManager::s_BuildUnitManager = NULL;

  BuildUnitManager* BuildUnitManager::create()
  {
    if ( s_BuildUnitManager )
    {
      return s_BuildUnitManager;
    }
    s_BuildUnitManager = new BuildUnitManager();
    return s_BuildUnitManager;
  }

  BuildUnitManager* BuildUnitManager::getInstance()
  {
    return s_BuildUnitManager;
  }

  void BuildUnitManager::destroy()
  {
    if ( s_BuildUnitManager )
    {
      delete s_BuildUnitManager;
    }
  }

  BuildUnitManager::BuildUnitManager()
  {
  }

  BuildUnitManager::~BuildUnitManager()
  {
    s_BuildUnitManager = NULL;
  }

  std::set< BuildUnit* >& BuildUnitManager::getUnits()
  {
    return m_buildUnits;
  }

  void BuildUnitManager::addUnit( BuildUnit* unit )
  {
    m_buildUnits.insert( unit );
  }

  void BuildUnitManager::onUnitEvade( BWAPI::Unit unit )
  {
    m_buildUnits.erase( ( BuildUnit* )unit->getClientInfo() );
  }

  void BuildUnitManager::resetPlanningData()
  {
    for each( BuildUnit* u in m_buildUnits ) 
    {
      u->m_planningData = u->m_currentState;
    }
  }
}
