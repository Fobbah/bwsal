#include <BWSAL/BFSBuildingPlacer.h>
#include <BWSAL/ReservedMap.h>
#include <BWSAL/Heap.h>
#include <BWSAL/Util.h>
namespace BWSAL
{
  BFSBuildingPlacer::BFSBuildingPlacer()
  {
    m_reducedWalkability.resize( BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight() );
    m_reducedWalkability.setTo( true );
    for ( int x = 0; x < BWAPI::Broodwar->mapWidth() * 4; x++ )
    {
      for ( int y = 0; y < BWAPI::Broodwar->mapHeight() * 4; y++ )
      {
        m_reducedWalkability[x/4][y/4] &= BWAPI::Broodwar->isWalkable( x, y );
      }
    }
  }
  BWAPI::TilePosition BFSBuildingPlacer::findBuildLocation( ReservedMap *reserveMap, BWAPI::UnitType unitType, BWAPI::TilePosition seedLocation, BWAPI::Unit builder )
  {
    return getBuildLocationNear( reserveMap, seedLocation, unitType, builder, 1 );
  }

  BWAPI::TilePosition BFSBuildingPlacer::getBuildLocationNear( ReservedMap *reserveMap, BWAPI::TilePosition position, BWAPI::UnitType type, BWAPI::Unit builder, int buildDist ) const
  {
    // returns a valid build location near the specified tile position.
    if ( type.isAddon() )
    {
      type = type.whatBuilds().first;
    }
    Heap< BWAPI::TilePosition, int > searchHeap( true );
    searchHeap.push( std::make_pair( position, 0 ) );
    std::set< BWAPI::TilePosition > closed;

    // Do a breadth first search to find a nearby valid build location with space
    while ( searchHeap.empty() == false )
    {
      BWAPI::TilePosition t = searchHeap.top().first;
      int s = searchHeap.top().second;
      searchHeap.pop();
      if ( canBuildHereWithSpace( reserveMap, t, type, builder, buildDist ) )
      {
        // We can build here with space so return this tile position
        return t;
      }
	  int tx = t.x;
      int ty = t.y;
      int minx = max( tx - 1, 0 );
      int maxx = min( tx + 1, BWAPI::Broodwar->mapWidth() - 1 );
      int miny = max( ty - 1, 0 );
      int maxy = min( ty + 1, BWAPI::Broodwar->mapHeight() - 1 );

      for ( int x = minx; x <= maxx; x++ )
      {
        for ( int y = miny; y <= maxy; y++ )
        {
          if ( m_reducedWalkability[x][y] )
          {
            BWAPI::TilePosition t2( x, y );
            if ( closed.find( t2 ) == closed.end() )
            {
              int ds = 10;
              if ( x != tx && y != ty )
              {
                ds = 14; // diagonal distance, approximation of 10*sqrt( 2 )
              }
              closed.insert( t2);
              searchHeap.push( std::make_pair( t2, s + ds ) );
            }
          }
        }
      }
    }
    // We didn't find a build position, try looking for one with less space
    if ( buildDist > 0 )
    {
      return getBuildLocationNear( reserveMap, position, type, builder, buildDist - 1 );
    }
    return BWAPI::TilePositions::None;
  }

  bool BFSBuildingPlacer::canBuildHereWithSpace( ReservedMap *reserveMap, BWAPI::TilePosition position, BWAPI::UnitType type, BWAPI::Unit builder, int buildDist ) const
  {
    if ( type.isAddon() )
    {
      type = type.whatBuilds().first;
    }
    // returns true if we can build this type of unit here with the specified amount of space.
    // space value is stored in this->buildDistance.

    // if we can't build here, we of course can't build here with space
    if ( !reserveMap->canBuildHere( builder, position, type ) )
    {
      return false;
    }
    if ( type.isRefinery() )
    {
      return true;
    }

    int width = type.tileWidth();
    int height = type.tileHeight();

    // make sure we leave space for add - ons. These types of units can have addons:
    if ( type == BWAPI::UnitTypes::Terran_Command_Center ||
         type == BWAPI::UnitTypes::Terran_Factory || 
         type == BWAPI::UnitTypes::Terran_Starport ||
         type == BWAPI::UnitTypes::Terran_Science_Facility )
    {
      width += 2;
    }
    int startx = position.x - buildDist;
    if ( startx < 0 )
    {
      return false;
    }
    int starty = position.y - buildDist;
    if ( starty < 0 )
    {
      return false;
    }
    int endx = position.x + width + buildDist;
    if ( endx > BWAPI::Broodwar->mapWidth() )
    {
      return false;
    }
    int endy = position.y + height + buildDist;
    if ( endy > BWAPI::Broodwar->mapHeight() )
    {
      return false;
    }

    for ( int x = startx; x < endx; x++ )
    {
      for ( int y = starty; y < endy; y++ )
      {
        if ( !isBuildable( builder, x, y ) || reserveMap->isReserved( x, y ) )
        {
          return false;
        }
      }
    }

    if ( position.x > 3 )
    {
      int startx2 = max( startx - 2, 0 );
      for ( int x = startx2; x < startx; x++ )
      {
        for ( int y = starty; y < endy; y++ )
        {
          for each ( BWAPI::Unit u in BWAPI::Broodwar->getUnitsOnTile( x, y ) )
          {
            if ( !u->isLifted() && u != builder )
            {
              BWAPI::UnitType type = u->getType();
              if ( type == BWAPI::UnitTypes::Terran_Command_Center ||
                   type == BWAPI::UnitTypes::Terran_Factory || 
                   type == BWAPI::UnitTypes::Terran_Starport ||
                   type == BWAPI::UnitTypes::Terran_Science_Facility )
              {
                return false;
              }
            }
          }
        }
      }
    }
    return true;
  }
  bool BFSBuildingPlacer::isBuildable( BWAPI::Unit builder, int x, int y ) const
  {
    // returns true if this tile is currently buildable, takes into account units on tile
    if ( !BWAPI::Broodwar->isBuildable( x, y ) )
    {
      return false;
    }
    for each ( BWAPI::Unit u in BWAPI::Broodwar->getUnitsOnTile( x, y ) )
    {
      if ( u->getType().isBuilding() && !u->isLifted() && !u->getType().isFlyer() && u != builder )
      {
        return false;
      }
    }
    return true;
  }
}