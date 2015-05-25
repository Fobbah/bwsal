#include <BWSAL/InformationManager.h>
#include <BWSAL/Types.h>

namespace BWSAL
{
  InformationManager* InformationManager::s_informationManager = NULL;

  InformationManager* InformationManager::create()
  {
    if ( s_informationManager )
    {
      return s_informationManager;
    }
    s_informationManager = new InformationManager();
    return s_informationManager;
  }

  InformationManager* InformationManager::getInstance()
  {
    return s_informationManager;
  }

  void InformationManager::destroy()
  {
    if ( s_informationManager )
    {
      delete s_informationManager;
    }
  }

  InformationManager::InformationManager()
  {
    // We always know the enemy has a center and worker
    m_buildTime[BWAPI::Broodwar->enemy()->getRace().getCenter()] = 0;
    m_buildTime[BWAPI::Broodwar->enemy()->getRace().getWorker()] = 0;

    // And an Overlord and Larva, if they're Zerg
    if ( BWAPI::Broodwar->enemy()->getRace() == BWAPI::Races::Zerg )
    {
      m_buildTime[BWAPI::UnitTypes::Zerg_Larva] = 0;
      m_buildTime[BWAPI::UnitTypes::Zerg_Overlord] = 0;
    }

    // The enemy could have spawned at any start location
    m_startLocationCouldContainEnemy = BWTA::getStartLocations();

    // Well, anywhere except where we spawned
    m_startLocationCouldContainEnemy.erase( BWTA::getStartLocation( BWAPI::Broodwar->self() ) );

    m_scoutedAnEnemyBase = false;

    // If there's only 1 other possible start location, it's a 2 player map
    if ( m_startLocationCouldContainEnemy.size() == 1 )
    {
      // So we know where they spawned
      m_enemyBases.insert( *m_startLocationCouldContainEnemy.begin() );
      m_scoutedAnEnemyBase = true;
    }
  }

  InformationManager::~InformationManager()
  {
    s_informationManager = NULL;
  }

  void InformationManager::onUnitDiscover( BWAPI::Unit unit )
  {
    // Sanity check
    if ( unit == NULL )
    {
      return;
    }

    m_savedData[unit].m_exists = true;

    // If this is an enemy unit, try to infer build time and start location, and base location
    if ( BWAPI::Broodwar->self()->isEnemy( unit->getPlayer() ) )
    {
      int time = BWAPI::Broodwar->getFrameCount();
      BWAPI::UnitType type = unit->getType();
      updateBuildTime( type, time - type.buildTime() );

      if ( m_scoutedAnEnemyBase == false && unit->getType().isBuilding() )
      {
        // We haven't scouted the enemy base yet, but this is a building
        BWTA::Region* r = BWTA::getRegion( unit->getTilePosition() );
        if ( r->getBaseLocations().size() == 1 )
        {

          // So the enemy probably spawned here.
          BWTA::BaseLocation* b = *( r->getBaseLocations().begin() );
          m_enemyBases.insert( b );
          m_scoutedAnEnemyBase = true;
        }
      }

      if ( unit->getType().isResourceDepot() )
      {

        // This is a center, so we know its a base location
        BWTA::BaseLocation* b = BWTA::getNearestBaseLocation( unit->getTilePosition() );
        m_enemyBases.insert( b );
        m_enemyBaseCenters[b] = unit;
        m_scoutedAnEnemyBase = true;
      }
    }
  }

  void InformationManager::onUnitEvade( BWAPI::Unit unit )
  {
    // Sanity check
    if ( unit == NULL )
    {
      return;
    }

    // Save what we know about the unit before we lose access to it
    m_savedData[unit].m_player = unit->getPlayer();
    m_savedData[unit].m_type = unit->getType();
    m_savedData[unit].m_position = unit->getPosition();
    m_savedData[unit].m_lastSeenTime = BWAPI::Broodwar->getFrameCount();
  }

  void InformationManager::onUnitDestroy( BWAPI::Unit unit )
  {
    // Sanity check
    if ( unit == NULL )
    {
      return;
    }

    onUnitEvade( unit );
    m_savedData[unit].m_exists = false;
    if ( BWAPI::Broodwar->self()->isEnemy( unit->getPlayer() ) )
    {
      // If this is an enemy unit we may need to remove a base location
      if ( unit->getType().isResourceDepot() )
      {
        BWTA::BaseLocation* b = BWTA::getNearestBaseLocation( unit->getTilePosition() );
        if ( m_enemyBaseCenters[b] == unit )
        {
          m_enemyBases.erase( b );
          m_enemyBaseCenters.erase( b );
        }
      }
    }
  }

  BWAPI::Player InformationManager::getPlayer( BWAPI::Unit unit ) const
  {
    // Sanity check
    if ( unit == NULL )
    {
      return NULL;
    }
    if ( unit->exists() )
    {
      return unit->getPlayer();
    }
    std::map< BWAPI::Unit, UnitData >::const_iterator i = m_savedData.find( unit );
    if ( i == m_savedData.end() )
    {
      return NULL;
    }
    return ( *i ).second.m_player;
  }

  BWAPI::UnitType InformationManager::getType( BWAPI::Unit unit ) const
  {
    // Sanity check
    if ( unit == NULL )
    {
      return BWAPI::UnitTypes::None;
    }

    if ( unit->exists() )
    {
      return unit->getType();
    }
    std::map< BWAPI::Unit, UnitData >::const_iterator i = m_savedData.find( unit );
    if ( i == m_savedData.end() )
    {
      return BWAPI::UnitTypes::None;
    }
    return ( *i ).second.m_type;
  }

  BWAPI::Position InformationManager::getLastPosition( BWAPI::Unit unit ) const
  {
    // Sanity check
    if ( unit == NULL )
    {
      return BWAPI::Positions::None;
    }

    if ( unit->exists() )
    {
      return unit->getPosition();
    }
    std::map< BWAPI::Unit, UnitData >::const_iterator i = m_savedData.find( unit );
    if ( i == m_savedData.end() )
    {
      return BWAPI::Positions::None;
    }
    return ( *i ).second.m_position;
  }

  int InformationManager::getLastSeenTime( BWAPI::Unit unit ) const
  {
    // Sanity check
    if ( unit == NULL )
    {
      return NEVER;
    }

    if ( unit->exists() )
    {
      return BWAPI::Broodwar->getFrameCount();
    }
    std::map< BWAPI::Unit, UnitData >::const_iterator i = m_savedData.find( unit );
    if ( i == m_savedData.end() )
    {
      return NEVER;
    }
    return ( *i ).second.m_lastSeenTime;
  }

  bool InformationManager::exists( BWAPI::Unit unit ) const
  {
    // Sanity check
    if ( unit == NULL )
    {
      return false;
    }

    if ( unit->exists() )
    {
      return true;
    }
    std::map< BWAPI::Unit, UnitData >::const_iterator i = m_savedData.find( unit );
    if ( i == m_savedData.end() )
    {
      return false;
    }
    return ( *i ).second.m_exists;
  }

  bool InformationManager::enemyHasBuilt( BWAPI::UnitType type ) const
  {
    return m_buildTime.find( type ) != m_buildTime.end();
  }

  int InformationManager::getBuildTime( BWAPI::UnitType type ) const
  {
    std::map< BWAPI::UnitType, int >::const_iterator i = m_buildTime.find( type );
    if ( i == m_buildTime.end() )
    {
      return NEVER;
    }
    return i->second;
  }

  const std::set< BWTA::BaseLocation* >& InformationManager::getEnemyBases() const
  {
    return m_enemyBases;
  }

  void InformationManager::setBaseEmpty( BWTA::BaseLocation* base )
  {
    m_startLocationCouldContainEnemy.erase( base );

    if ( m_startLocationCouldContainEnemy.size() == 1 )
    {
      // Process of elimination, we know where the enemy spawned
      m_enemyBases.insert( *m_startLocationCouldContainEnemy.begin() );
      m_scoutedAnEnemyBase = true;
    }
  }

  void InformationManager::updateBuildTime( BWAPI::UnitType type, int time )
  {
    std::map< BWAPI::UnitType, int >::iterator i = m_buildTime.find( type );
    if ( i != m_buildTime.end() && ( i->second <= time || i->second == 0 ) )
    {
      // We have already determined an earlier build time
      return;
    }

    m_buildTime[type] = time;

    // Sanity check
    if ( time < 0 )
    {
      return;
    }

    // Update earliest known build times of required units
    for ( std::map< BWAPI::UnitType, int >::const_iterator r = type.requiredUnits().begin(); r != type.requiredUnits().end(); r++ )
    {
      updateBuildTime( r->first, time - r->first.buildTime() );
    }
  }

  InformationManager::UnitData::UnitData()
  {
    m_position = BWAPI::Positions::Unknown;
    m_type = BWAPI::UnitTypes::Unknown;
    m_player = NULL;
    m_lastSeenTime = NEVER;
    m_exists = false;
  }

}