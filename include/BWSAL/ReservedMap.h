#pragma once
#include <BWAPI.h>
#include <BWSAL/RectangleArray.h>
namespace BWSAL
{
  class ReservedMap : public BWAPI::AIModule
  {
    public:
      static ReservedMap* create();
      static ReservedMap* getInstance();
      static void destroy();
      void reserveTiles( BWAPI::TilePosition position, BWAPI::UnitType type, int width = 1, int height = 1 );
      void freeTiles( BWAPI::TilePosition position, int width = 1, int height = 1 );
      bool isReserved( int x, int y ) const;
      bool isReserved( BWAPI::TilePosition p ) const;
      bool canBuildHere( BWAPI::Unit builder, BWAPI::TilePosition position, BWAPI::UnitType type ) const;
      BWAPI::UnitType getReservedType( int x, int y ) const;
      BWAPI::UnitType getReservedType( BWAPI::TilePosition p ) const;
    private:
      ReservedMap();
      ~ReservedMap();
      RectangleArray< BWAPI::UnitType > m_reservedMap;
      static ReservedMap* s_reservedMap;
  };
}