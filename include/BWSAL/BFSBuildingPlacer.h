#pragma once
#include <BWSAL/BuildingPlacer.h>
#include <BWSAL/RectangleArray.h>
namespace BWSAL
{
  // An implementation of a BuildingPlacer
  class BFSBuildingPlacer : public BuildingPlacer
  {
    public:
      BFSBuildingPlacer();
      virtual BWAPI::TilePosition findBuildLocation( ReservedMap* reserveMap, BWAPI::UnitType unitType, BWAPI::TilePosition seedLocation, BWAPI::Unit* builder = NULL );
    private:
      BWAPI::TilePosition getBuildLocationNear( ReservedMap* reserveMap, BWAPI::TilePosition position, BWAPI::UnitType type, BWAPI::Unit* builder, int buildDist ) const;
      bool canBuildHereWithSpace( ReservedMap* reserveMap, BWAPI::TilePosition position, BWAPI::UnitType type, BWAPI::Unit* builder, int buildDist ) const;
      bool isBuildable( BWAPI::Unit* builder, int x, int y ) const;
      RectangleArray< bool > m_reducedWalkability;

  };
}