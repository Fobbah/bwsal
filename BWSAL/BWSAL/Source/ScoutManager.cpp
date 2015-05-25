#include <BWTA.h>
#include <BWSAL/ScoutManager.h>
#include <BWSAL/UnitGroupManager.h>
#include <BWSAL/InformationManager.h>
namespace BWSAL
{
  ScoutManager* ScoutManager::s_scoutManager = NULL;

  ScoutManager* ScoutManager::create( UnitArbitrator* arbitrator, InformationManager* informationManager )
  {
    if ( s_scoutManager )
    {
      return s_scoutManager;
    }
    s_scoutManager = new ScoutManager();
    s_scoutManager->m_arbitrator = arbitrator;
    s_scoutManager->m_informationManager = informationManager;
    return s_scoutManager;
  }

  ScoutManager* ScoutManager::getInstance()
  {
    return s_scoutManager;
  }

  void ScoutManager::destroy()
  {
    if ( s_scoutManager )
    {
      delete s_scoutManager;
    }
  }

  ScoutManager::ScoutManager()
  {
  }

  ScoutManager::~ScoutManager()
  {
    s_scoutManager = NULL;
  }

  void ScoutManager::initialize()
  {
    m_desiredScoutCount = 0;

    m_myStartLocation = BWTA::getStartLocation( BWAPI::Broodwar->self() );
    std::set< BWTA::BaseLocation* > startLocations;
    for( BWTA::BaseLocation * bl : BWTA::getStartLocations() )
    {
      if ( m_myStartLocation->getGroundDistance( bl ) > 0 && bl != m_myStartLocation )
      {
        startLocations.insert( bl );
      }
    }

    std::list< BWTA::BaseLocation* > path = getScoutPath( startLocations ).first;
    for( BWTA::BaseLocation* bl : path )
    {
      m_baseLocationsToScout.push_back( bl );
    }
    m_baseLocationsExplored.insert( m_myStartLocation );
    m_debugMode = false;
  }

  std::pair< std::list< BWTA::BaseLocation* >, double > ScoutManager::getShortestPath( BWTA::BaseLocation* currentBaseLocation, std::set< BWTA::BaseLocation* > &baseLocations )
  {
    std::pair< std::list< BWTA::BaseLocation* >, double > shortestPath;
    shortestPath.second = 0;
    // base case
    if ( baseLocations.size() == 1 )
    {
      shortestPath.first.push_back( *baseLocations.begin() );
      shortestPath.second = currentBaseLocation->getGroundDistance( *baseLocations.begin() );
      return shortestPath;
    }
    // Try traveling to each base location first
    for( BWTA::BaseLocation* bl : baseLocations )
    {
      // Travel to bl first
      baseLocations.erase( bl );

      // Get the best path of the remaining base locations
      std::pair< std::list< BWTA::BaseLocation* >, double > pathResult = getShortestPath( bl, baseLocations );
      baseLocations.insert( bl );

      // Prepend the first base location of the path and add the distance to the first base location to the length
      pathResult.first.push_front( bl );
      pathResult.second += currentBaseLocation->getGroundDistance( bl );

      // Update shortest path if we've found a shorter path
      if ( pathResult.second < shortestPath.second || shortestPath.first.empty() )
      {
        shortestPath = pathResult;
      }
    }
    return shortestPath;
  }

  std::pair< std::list< BWTA::BaseLocation* >, double > ScoutManager::getScoutPath( std::set< BWTA::BaseLocation* > baseLocations )
  {
    std::pair< std::list< BWTA::BaseLocation* >, double > shortestPath;
    shortestPath.second = 0;

    BWTA::BaseLocation* start = BWTA::getStartLocation( BWAPI::Broodwar->self() );
    baseLocations.erase( start );

    if ( baseLocations.empty() )
    {
      // No base locations, the shortest path is empty
      return shortestPath;
    }

    return getShortestPath( start, baseLocations );
  }

  void ScoutManager::onOffer( std::set< BWAPI::Unit > units )
  {
    // First find an overlord
    std::set< BWAPI::Unit >::iterator u2;
    for ( std::set< BWAPI::Unit >::iterator u = units.begin(); u != units.end(); u = u2 )
    {
      u2 = u;
      u2++;
      // ignore if its already a scout
      if ( m_scouts.find( *u ) != m_scouts.end() )
      {
        m_arbitrator->accept( this, *u );
        units.erase( u );
      }
      else if ( ( *u )->getType() == BWAPI::UnitTypes::Zerg_Overlord && needMoreScouts() )
      {
        m_arbitrator->accept( this, *u );
        addScout( *u );
        units.erase( u );
      }
    }

    for ( std::set< BWAPI::Unit >::iterator u = units.begin(); u != units.end(); u = u2 )
    {
      u2 = u;
      u2++;
      // ignore if its already a scout
      if ( m_scouts.find( *u ) != m_scouts.end() )
      {
        m_arbitrator->accept( this, *u );
        units.erase( u );
      }
      else if ( ( *u )->getType().isWorker() && needMoreScouts() )
      {
        m_arbitrator->accept( this, *u );
        addScout( *u );
        units.erase( u );
      }
    }

    // decline remaining units
    for( BWAPI::Unit u : units )
    {
      m_arbitrator->decline( this, u, 0 );
    }
  }

  void ScoutManager::onRevoke( BWAPI::Unit unit, double bid )
  {
    if ( m_scouts.find( unit ) != m_scouts.end() )
    {
      BWTA::BaseLocation* lostTarget = m_scouts[unit].m_target;
      if ( m_baseLocationsExplored.find( lostTarget ) == m_baseLocationsExplored.end() )
      {
        m_baseLocationsToScout.push_back( lostTarget );
        if ( m_debugMode )
        {
          BWAPI::Broodwar->printf( "Reassigning ( %d, %d )", lostTarget->getPosition().x, lostTarget->getPosition().y );
        }
      }
      m_scouts.erase( unit );
    }
  }

  void ScoutManager::onFrame()
  {
    if ( needMoreScouts() )
    {
      requestScout( /* bid = */ 30 ); // Bid 30.
    }
    else
    {
      int sCount = m_desiredScoutCount;
      if ( m_baseLocationsExplored.size() == BWAPI::Broodwar->getStartLocations().size() )
      {
        sCount = 0;
      }
      while ( (int)m_scouts.size() > sCount )
      {
        m_arbitrator->removeBid( this, m_scouts.begin()->first );
        m_scouts.erase( m_scouts.begin() );
      }
    }
    updateScoutAssignments();
    if ( m_debugMode )
    {
      drawAssignments();
    }
  }

  std::string ScoutManager::getName() const
  {
    return "Scout Manager";
  }

  void ScoutManager::setScoutCount( int count )
  {
    m_desiredScoutCount = count;
  }

  void ScoutManager::setDebugMode( bool debugMode )
  {
    m_debugMode = debugMode;
  }

  void ScoutManager::drawAssignments()
  {
    // draw target vector for each scout
    for ( std::map< BWAPI::Unit, ScoutData >::iterator s = m_scouts.begin(); s != m_scouts.end(); s++ )
    {
      if ( s->second.m_mode != ScoutData::Idle )
      {
        BWAPI::Position scoutPos = s->first->getPosition();
        BWAPI::Position targetPos = s->second.m_target->getPosition();
        BWAPI::Broodwar->drawLineMap( scoutPos.x, scoutPos.y, targetPos.x, targetPos.y, BWAPI::Colors::Yellow );
        BWAPI::Broodwar->drawCircleMap( scoutPos.x, scoutPos.y, 6, BWAPI::Colors::Yellow );
        BWAPI::Broodwar->drawCircleMap( targetPos.x, targetPos.y, s->first->getType().sightRange(), BWAPI::Colors::Yellow );
      }
    }
  }

  bool ScoutManager::isScouting() const
  {
    return m_scouts.size() >= 1;
  }

  bool ScoutManager::needMoreScouts() const
  {
    int sCount = m_desiredScoutCount;
    if ( m_baseLocationsExplored.size() == BWAPI::Broodwar->getStartLocations().size() )
    {
      sCount = 0;
    }
    return (int)m_scouts.size() < sCount;
  }

  void ScoutManager::requestScout( double bid )
  {
    // Bid on all completed workers.
    std::set< BWAPI::Unit > usefulUnits = SelectAll()( isWorker, Overlord )( isCompleted ).not( isCarryingMinerals, isCarryingGas, isGatheringGas );
    m_arbitrator->setBid( this, usefulUnits, bid );
  }

  void ScoutManager::addScout( BWAPI::Unit u )
  {
    ScoutData temp;
    m_scouts.insert( std::make_pair( u, temp ) );
  }

  void ScoutManager::updateScoutAssignments()
  {
    // Remove scout positions if the enemy is not there.
    for ( std::map< BWAPI::Unit, ScoutData >::iterator s = m_scouts.begin(); s != m_scouts.end(); s++ )
    {
      if ( s->second.m_mode == ScoutData::Searching
        && s->first->getPosition().getDistance( s->second.m_target->getPosition() ) < s->first->getType().sightRange() )
      {
        bool empty = true;
        for ( int x = s->second.m_target->getTilePosition().x; x < s->second.m_target->getTilePosition().x + 4; x++ )
        {
          for ( int y = s->second.m_target->getTilePosition().y; y < s->second.m_target->getTilePosition().y + 3; y++ )
          {
            for( BWAPI::Unit u : BWAPI::Broodwar->getUnitsOnTile( x, y ) )
            {
              if ( u->getType().isResourceDepot() )
              {
                empty = false;
                break;
              }
            }
            if ( !empty ) break;
          }
          if ( !empty ) break;
        }
        if ( empty )
        {
          m_informationManager->setBaseEmpty( s->second.m_target );
        }
        BWTA::BaseLocation* exploredBaseLocation = s->second.m_target;
        m_baseLocationsToScout.remove( exploredBaseLocation );
        
        m_baseLocationsExplored.insert( exploredBaseLocation );
        s->second.m_mode = ScoutData::Idle;
        if ( m_debugMode )
        {
          BWAPI::Broodwar->printf( "Sucessfully scouted ( %d, %d )", exploredBaseLocation->getPosition().x, exploredBaseLocation->getPosition().y );
        }
      }
    }

    // Set scouts to scout.
    if ( m_baseLocationsToScout.size() > 0 ) // are there still positions to scout?
    {
      for ( std::map< BWAPI::Unit, ScoutData >::iterator s = m_scouts.begin(); s != m_scouts.end(); s++ )
      {
        if ( s->second.m_mode == ScoutData::Idle )
        {
          double minDist = 100000000;
          BWTA::BaseLocation* target = NULL;
          for( BWTA::BaseLocation* bl : m_baseLocationsToScout )
          {
            double distance = s->first->getPosition().getDistance( bl->getPosition() );
            if ( distance < minDist )
            {
              minDist = distance;
              target = bl;
            }
          }
          if ( target != NULL )
          {
            s->second.m_mode = ScoutData::Searching;
            s->first->rightClick( target->getPosition() );
            s->second.m_target = target;
            m_baseLocationsToScout.remove( target );
            if ( m_debugMode )
            {
              BWAPI::Broodwar->printf( "Scouting ( %d, %d )", target->getPosition().x, target->getPosition().y );
            }
          }
        }
      }
    }

  }
}