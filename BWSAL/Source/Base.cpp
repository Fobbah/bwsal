#include <BWSAL/Base.h>
#include <BWSAL/Util.h>
#include <BWSAL/MacroTask.h>
#include <BWSAL/BuildUnit.h>
#include <BWSAL/MetaUnitVariable.h>
#include <BWSAL/MetaUnit.h>
#include <BWSAL/BuildOrderManager.h>
#include <BWAPI.h>
namespace BWSAL
{
  BWAPI::Unitset emptySet;
  Base::Base( BWTA::BaseLocation* b, BWAPI::Unit resourceDepot )
  {
    m_baseLocation = b;
    m_resourceDepot = resourceDepot;
    m_refinery = NULL;
    m_ready = false;
    m_paused = false;
    m_macroTask = NULL;
  }
  Base* Base::CreateBaseNow( BWTA::BaseLocation* bl, bool getGas )
  {
    Base* b = new Base( bl );
    b->m_macroTask = BuildOrderManager::getInstance()->buildAdditional( 1, BWAPI::Broodwar->self()->getRace().getCenter(), 1000, bl->getTilePosition() );
    b->m_macroTask->getTasks().front()->setBuildLocation( bl->getTilePosition() );
    b->m_macroTask->getTasks().front()->setRelocatable( false );
    return b;
  }
  Base* Base::CreateBaseWhenPossible( BWTA::BaseLocation* bl, bool getGas )
  {
    Base* b = new Base( bl );
    b->m_macroTask = BuildOrderManager::getInstance()->buildAdditional( 1, BWAPI::Broodwar->self()->getRace().getCenter(), 1, bl->getTilePosition() );
    b->m_macroTask->getTasks().front()->setBuildLocation( bl->getTilePosition() );
    b->m_macroTask->getTasks().front()->setRelocatable( false );
    return b;
  }
  Base* Base::CreateBaseAtFrame( BWTA::BaseLocation* bl, int frame, bool getGas )
  {
    Base* b = new Base( bl );
    b->m_macroTask = BuildOrderManager::getInstance()->buildAdditional( 1, BWAPI::Broodwar->self()->getRace().getCenter(), 1000, bl->getTilePosition() );
    b->m_macroTask->getTasks().front()->setEarliestStartTime( frame );
    b->m_macroTask->getTasks().front()->setBuildLocation( bl->getTilePosition() );
    b->m_macroTask->getTasks().front()->setRelocatable( false );
    return b;
  }

  BWTA::BaseLocation* Base::getBaseLocation() const
  {
    return m_baseLocation;
  }

  BWAPI::Unit Base::getResourceDepot() const
  {
    return m_resourceDepot;
  }

  BWAPI::Unit Base::getRefinery() const
  {
    return m_refinery;
  }

  const BWAPI::Unitset Base::getMinerals() const
  {
    if ( m_baseLocation == NULL )
    {
      return emptySet;
    }
    return m_baseLocation->getMinerals();
  }

  const BWAPI::Unitset Base::getGeysers() const
  {
    if ( m_baseLocation == NULL )
    {
      return emptySet;
    }
    return m_baseLocation->getGeysers();
  }

  void Base::setPaused( bool paused )
  {
    m_paused = paused;
  }

  bool Base::isPaused() const
  {
    return m_paused;
  }

  bool Base::isReady() const
  {
    return m_ready;
  }

  bool Base::isActive() const
  {
    return !m_paused && m_ready;
  }

  void Base::update()
  {
    if ( m_resourceDepot == NULL)
    {
      if ( m_macroTask != NULL )
      {
        Task* t = m_macroTask->getTasks().front();
        if ( t->getCreatedUnit() && t->getCreatedUnit()->isReal() && t->getCreatedUnit()->getUnit()->exists() )
        {
          m_resourceDepot = t->getCreatedUnit()->getUnit();
        }
        else if ( t->getBuilder() &&
                  t->getBuilder()->getBuildUnit() &&
                  t->getBuilder()->getBuildUnit()->isReal() &&
                  t->getBuilder()->getBuildUnit()->getUnit()->exists() &&
                  t->getBuilder()->getBuildUnit()->getUnit()->getType().isResourceDepot() )
        {
          m_resourceDepot = t->getBuilder()->getBuildUnit()->getUnit();
        }

      }
    }
    m_ready = ( m_resourceDepot != NULL &&
                m_resourceDepot->exists() &&
                ( resourceDepotIsCompleted( m_resourceDepot ) || m_resourceDepot->getRemainingBuildTime() < 300 ) );
  }

  void Base::onUnitDestroy( BWAPI::Unit u )
  {
    if ( u == m_refinery )
    {
      m_refinery = NULL;
    }
    else if ( u == m_resourceDepot )
    {
      m_resourceDepot = NULL;
    }
  }
}