#include <BWSAL/ReservedMap.h>
namespace BWSAL
{
  ReservedMap* ReservedMap::s_reservedMap = NULL;

  ReservedMap* ReservedMap::create()
  {
    if ( s_reservedMap )
    {
      return s_reservedMap;
    }
    s_reservedMap = new ReservedMap();
    return s_reservedMap;
  }

  ReservedMap* ReservedMap::getInstance()
  {
    return s_reservedMap;
  }

  void ReservedMap::destroy()
  {
    if ( s_reservedMap )
    {
      delete s_reservedMap;
    }
  }

  ReservedMap::ReservedMap()
  {
    m_reservedMap.resize( BWAPI::Broodwar->mapWidth(), BWAPI::Broodwar->mapHeight() );
    m_reservedMap.setTo( BWAPI::UnitTypes::None );
  }

  ReservedMap::~ReservedMap()
  {
    s_reservedMap = NULL;
  }

  void ReservedMap::reserveTiles( BWAPI::TilePosition position, BWAPI::UnitType type, int width, int height )
  {
    for ( int x = position.x; x < position.x + width && x < (int)m_reservedMap.getWidth(); x++ )
    {
      for ( int y = position.y; y < position.y + height && y < (int)m_reservedMap.getHeight(); y++ )
      {
        m_reservedMap[x][y] = type;
      }
    }
  }

  void ReservedMap::freeTiles( BWAPI::TilePosition position, int width, int height )
  {
    for ( int x = position.x; x < position.x + width && x < (int)m_reservedMap.getWidth(); x++ )
    {
      for ( int y = position.y; y < position.y + height && y < (int)m_reservedMap.getHeight(); y++ )
      {
        m_reservedMap[x][y] = BWAPI::UnitTypes::None;
      }
    }
  }

  bool ReservedMap::isReserved( int x, int y ) const
  {
    return m_reservedMap[x][y] != BWAPI::UnitTypes::None;
  }

  bool ReservedMap::isReserved( BWAPI::TilePosition p ) const
  {
    return m_reservedMap[p.x][p.y] != BWAPI::UnitTypes::None;
  }

  bool ReservedMap::canBuildHere( BWAPI::Unit builder, BWAPI::TilePosition position, BWAPI::UnitType type ) const
  {
    if ( type.isAddon() )
    {
      type = type.whatBuilds().first;
    }
    // returns true if we can build this type of unit here. Takes into account reserved tiles.
	if (!BWAPI::Broodwar->canBuildHere(position, type, builder))
    {
      return false;
    }
    for ( int x = position.x; x < position.x + type.tileWidth(); x++ )
    {
      for ( int y = position.y; y < position.y + type.tileHeight(); y++ )
      {
        if ( m_reservedMap[x][y] != BWAPI::UnitTypes::None )
        {
          return false;
        }
      }
    }
    return true;
  }

  BWAPI::UnitType ReservedMap::getReservedType( int x, int y ) const
  {
    return m_reservedMap[x][y];
  }

  BWAPI::UnitType ReservedMap::getReservedType( BWAPI::TilePosition p ) const
  {
    return m_reservedMap[p.x][p.y];
  }
}