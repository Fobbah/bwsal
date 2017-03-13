#pragma once
#include <BWAPI.h>
namespace BWSAL
{
  class ReservedMap;

  //! Interface for a building placer algorithm, eg BFSBuildingPlacer
  class BuildingPlacer
  {
    public:

	  //! Return an appropriate bulding location for the given building, given the seed location, map of reserved tiles, and desired building unit type
      virtual BWAPI::TilePosition findBuildLocation( ReservedMap* reserveMap, BWAPI::UnitType unitType, BWAPI::TilePosition seedLocation, BWAPI::Unit builder = NULL ) = 0;
  };
}