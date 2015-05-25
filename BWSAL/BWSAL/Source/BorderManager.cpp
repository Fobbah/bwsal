#include <BWSAL/BorderManager.h>
#include <BWSAL/InformationManager.h>
namespace BWSAL
{
  BorderManager* BorderManager::s_borderManager = NULL;

  BorderManager* BorderManager::create( InformationManager* informationManager )
  {
    if ( s_borderManager )
    {
      return s_borderManager;
    }
    s_borderManager = new BorderManager();
    s_borderManager->m_informationManager = informationManager;
    return s_borderManager;
  }
  BorderManager* BorderManager::getInstance()
  {
    return s_borderManager;
  }

  void BorderManager::destroy()
  {
    if ( s_borderManager )
    {
      delete s_borderManager;
    }
  }

  BorderManager::BorderManager()
  {
  }

  BorderManager::~BorderManager()
  {
    s_borderManager = NULL;
  }

  void BorderManager::addMyBase( BWTA::BaseLocation* location )
  {
    m_myBases.insert( location );
    recalculateBorders();
  }

  void BorderManager::removeMyBase( BWTA::BaseLocation* location )
  {
    m_myBases.erase( location );
    recalculateBorders();
  }

  const std::set< BWTA::Chokepoint* >& BorderManager::getMyBorder() const
  {
    return m_myBorder;
  }

  const std::set< BWTA::Chokepoint* >& BorderManager::getEnemyBorder() const
  {
    return m_enemyBorder;
  }

  void BorderManager::onFrame()
  {
    if ( m_informationManager->getEnemyBases() != m_enemyBases )
    {
      // the set of enemy bases has changed, so we need to recalculate the borders
      m_enemyBases = m_informationManager->getEnemyBases();
      recalculateBorders();
    }
  }

  void BorderManager::draw()
  {

	for (BWTA::Chokepoint* c : m_myBorder)
    {
      BWAPI::Position point1 = c->getSides().first;
      BWAPI::Position point2 = c->getSides().second;
      BWAPI::Broodwar->drawLineMap( point1.x, point1.y, point2.x, point2.y, BWAPI::Colors::Red );
    }

	for (BWTA::Chokepoint* c : m_enemyBorder)
    {
      BWAPI::Position point1 = c->getSides().first;
      BWAPI::Position point2 = c->getSides().second;
      BWAPI::Broodwar->drawLineMap( point1.x, point1.y, point2.x, point2.y, BWAPI::Colors::Orange );
    }

  }

  void BorderManager::recalculateBorders()
  {
    m_myRegions.clear();
    m_myBorder.clear();
    m_enemyRegions.clear();
    m_enemyBorder.clear();

    // Set of regions that we can reach without going through an enemy region
    std::set< BWTA::Region* > canReachSelf;

    // Set of regions that the enemy can reach without going through one of our regions
    std::set< BWTA::Region* > canReachEnemy;

	for (BWTA::BaseLocation* b : m_myBases)
    {
      m_myRegions.insert( b->getRegion() );
      canReachSelf.insert( b->getRegion() );
    }

	for (BWTA::BaseLocation* b : m_enemyBases)
    {
      m_enemyRegions.insert( b->getRegion() );
      canReachEnemy.insert( b->getRegion() );
    }

    if ( m_enemyBases.empty() )
    {
	  for (BWTA::BaseLocation* b : BWTA::getBaseLocations())
      {
        if ( m_myBases.find( b ) == m_myBases.end() )
        {
          m_enemyRegions.insert( b->getRegion() );
          canReachEnemy.insert( b->getRegion() );
        }
      }
    }

    bool exploring = true;
    while ( exploring )
    {
      exploring = false;

      for( BWTA::Region* r : BWTA::getRegions() )
      {
        for( BWTA::Chokepoint* c : r->getChokepoints() )
        {
          BWTA::Region* r2 = c->getRegions().first;
          if ( r == r2 )
          {
            r2 = c->getRegions().second;
          }

          // If r can reach self, and r2 isn't an enemy region, then r2 can reach self
          if ( canReachSelf.find( r ) != canReachSelf.end() && m_enemyRegions.find( r2 ) == m_enemyRegions.end() && canReachSelf.find( r2 ) == canReachSelf.end() )
          {
            canReachSelf.insert( r2 );
            exploring = true;
          }

          // If r can reach enemy, and r2 isn't a self region, then r2 can reach enemy
          if ( canReachEnemy.find( r ) != canReachEnemy.end() && m_myRegions.find( r2 ) == m_myRegions.end() && canReachEnemy.find( r2 ) == canReachEnemy.end() )
          {
            canReachEnemy.insert( r2 );
            exploring = true;
          }

        }
      }
    }

	for (BWTA::Region* r : BWTA::getRegions())
    {
      // If we can reach r and not enemy, we control it
      if ( canReachSelf.find( r ) != canReachSelf.end() && canReachEnemy.find( r ) == canReachEnemy.end() )
      {
        m_myRegions.insert( r );
      }

      // If the enemy can reach r and not us, the enemy controls it
      if ( canReachSelf.find( r ) == canReachSelf.end() && canReachEnemy.find( r ) != canReachEnemy.end() )
      {
        m_enemyRegions.insert( r );
      }
    }
    // If both or neither of us can reach r, it is in no - mans land.

    // Compute our border from our regions
	for (BWTA::Region* r : m_myRegions)
    {
	  for (BWTA::Chokepoint* c : r->getChokepoints())
      {
        if ( m_myBorder.find( c ) == m_myBorder.end() )
        {
          m_myBorder.insert( c );
        }
        else
        {
          // We control both sides of this choke point so the choke point is not on our border
          m_myBorder.erase( c );
        }
      }
    }

    // Compute the enemy's border from the enemy's regions
	for (BWTA::Region* r : m_enemyRegions)
    {
      for (BWTA::Chokepoint* c : r->getChokepoints())
      {
        if ( m_enemyBorder.find( c ) == m_enemyBorder.end() )
        {
          m_enemyBorder.insert( c );
        }
        else
        {
          // Enemy controls both sides of this choke point so the choke point is not on the enemy's border
          m_enemyBorder.erase( c );
        }
      }
    }
  }
}