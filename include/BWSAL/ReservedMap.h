#pragma once
#include <BWAPI.h>
#include <BWSAL/RectangleArray.h>
namespace BWSAL
{
  //! Tracks tiles reserved for building placement
  class ReservedMap : public BWAPI::AIModule
  {
    public:
	  //! Create the singleton instance of this ReservedMap
      static ReservedMap* create();

	  //! Return the singleton instance of this ReservedMap
      static ReservedMap* getInstance();

	  //! Destroy the singleton instance of this ReservedMap
      static void destroy();

	  //! Reserve to given area of tiles for construction
      void reserveTiles( BWAPI::TilePosition position, BWAPI::UnitType type, int width = 1, int height = 1 );

	  //! Free the specified tiles for construction
      void freeTiles( BWAPI::TilePosition position, int width = 1, int height = 1 );

	  //! Returns true if the given tile position (as x,y coordinates) is reserved for construction 
      bool isReserved( int x, int y ) const;

	  //! Returns true if the given TilePosition is reserved for construction
      bool isReserved( BWAPI::TilePosition p ) const;

	  //! Returns true if the given builder can build this unit type at the provided position
      bool canBuildHere( BWAPI::Unit builder, BWAPI::TilePosition position, BWAPI::UnitType type ) const;

	  //! Returns the type of unit reserving the tile at the specified tile position (given as x,y coordinates)
      BWAPI::UnitType getReservedType( int x, int y ) const;

	  //! Returns the type of unit reserving the tile at the specified TilePosition
      BWAPI::UnitType getReservedType( BWAPI::TilePosition p ) const;
    private:
      ReservedMap();
      ~ReservedMap();
      RectangleArray< BWAPI::UnitType > m_reservedMap;
      static ReservedMap* s_reservedMap;
  };
}