#include <BWSAL/BaseManager.h>
#include <BWSAL/Base.h>
#include <BWSAL/BorderManager.h>
#include <BWAPI.h>
namespace BWSAL
{
  BaseManager* BaseManager::s_baseManager = NULL;

  BaseManager* BaseManager::create( BorderManager* borderManager )
  {
    if ( s_baseManager )
    {
      return s_baseManager;
    }
    s_baseManager = new BaseManager();
    s_baseManager->m_borderManager = borderManager;
    return s_baseManager;
  }

  BaseManager* BaseManager::getInstance()
  {
    return s_baseManager;
  }

  void BaseManager::destroy()
  {
    if ( s_baseManager )
    {
      delete s_baseManager;
    }
  }

  BaseManager::BaseManager()
  {
  }

  BaseManager::~BaseManager()
  {
    s_baseManager = NULL;
  }

  void BaseManager::onFrame()
  {
    // check to see if any new base locations need to be added
	  for (BWTA::BaseLocation* location : BWTA::getBaseLocations())
    {
      if ( m_location2base.find( location ) == m_location2base.end() )
      {
        // There is no base currently associated with this base location
        // so look for a resource depot on it
        BWAPI::TilePosition tile = location->getTilePosition();
		for (BWAPI::Unit u : BWAPI::Broodwar->getUnitsOnTile(tile.x, tile.y))
        {
          if ( u->getPlayer() == BWAPI::Broodwar->self() && u->getType().isResourceDepot() )
          {
            // Found a resource depot, create a base location
            Base* mb = new Base( location, u );
            m_location2base[location] = mb;
            m_allBases.insert( mb );
            m_borderManager->addMyBase( location );
            break;
          }
        }
      }
    }
    // Update each base
    for( Base* mb : m_allBases )
    {
      mb->update();
      if ( mb->isActive() )
      {
        m_activeBases.insert( mb );
      }
      else
      {
        m_activeBases.erase( mb );
      }
      if ( mb->isReady() )
      {
        m_readyBases.insert( mb );
      }
      else
      {
        m_readyBases.erase( mb );
      }
    }
  }

  Base* BaseManager::getBase( BWTA::BaseLocation* location ) const
  {
    std::map< BWTA::BaseLocation*, Base* >::const_iterator i = m_location2base.find( location );
    if ( i == m_location2base.end() )
    {
      return NULL;
    }
    return i->second;
  }
  BWTA::BaseLocation* BaseManager::decideWhereToExpand() const
  {
    BWTA::BaseLocation* location = NULL;
    BWTA::BaseLocation* home = BWTA::getStartLocation( BWAPI::Broodwar->self() );
    double minDist = -1;
	for (BWTA::BaseLocation* bl : BWTA::getBaseLocations())
    {
      double dist = home->getGroundDistance( bl );
      if (dist > 0 && getBase( bl ) == NULL )
      {
        if ( minDist == -1 || dist < minDist )
        {
          minDist = dist;
          location = bl;
        }
      }
    }
    return location;
  }
  Base* BaseManager::expandNow(BWTA::BaseLocation* location, bool getGas)
  {
    if (location == NULL)
    {
      location = decideWhereToExpand();
      if ( location == NULL )
      {
        // can't decide where to expand
        return NULL;
      }
    }
    Base* b = Base::CreateBaseNow( location, getGas );
    m_location2base[location] = b;
    m_allBases.insert( b );
    m_borderManager->addMyBase(location);
    return b;
  }
  Base* BaseManager::expandWhenPossible(BWTA::BaseLocation* location, bool getGas)
  {
    if (location == NULL)
    {
      location = decideWhereToExpand();
      if ( location == NULL )
      {
        // can't decide where to expand
        return NULL;
      }
    }
    Base* b = Base::CreateBaseWhenPossible( location, getGas );
    m_location2base[location] = b;
    m_allBases.insert( b );
    m_borderManager->addMyBase(location);
    return b;
  }
  Base* BaseManager::expandAtFrame(int frame, BWTA::BaseLocation* location, bool getGas)
  {
    if (location == NULL)
    {
      location = decideWhereToExpand();
      if ( location == NULL )
      {
        // can't decide where to expand
        return NULL;
      }
    }
    Base* b = Base::CreateBaseAtFrame( location, frame, getGas );
    m_location2base[location] = b;
    m_allBases.insert( b );
    m_borderManager->addMyBase(location);
    return b;
  }

  const std::set< Base* >& BaseManager::getActiveBases() const
  {
    return m_activeBases;
  }

  const std::set< Base* >& BaseManager::getReadyBases() const
  {
    return m_readyBases;
  }

  const std::set< Base* >& BaseManager::getAllBases() const
  {
    return m_allBases;
  }

  const std::set< Base* >& BaseManager::getDestroyedBases() const
  {
    return m_destroyedBases;
  }

  std::string BaseManager::getName() const
  {
    return "BaseManager";
  }

  void BaseManager::onUnitDestroy( BWAPI::Unit unit )
  {
    // Sanity check
    if ( unit == NULL )
    {
      return;
    }

    // Tell all our bases that this unit has been destroyed
	for (Base* b : m_allBases)
    {
      b->onUnitDestroy( unit );
    }
  }
}