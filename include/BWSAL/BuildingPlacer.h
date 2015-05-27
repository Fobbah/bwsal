#pragma once
#include <BWAPI.h>
namespace BWSAL
{
  class ReservedMap;
  class BuildingPlacer
  {
    public:
      virtual BWAPI::TilePosition findBuildLocation( ReservedMap* reserveMap, BWAPI::UnitType unitType, BWAPI::TilePosition seedLocation, BWAPI::Unit builder = NULL ) = 0;
  };
}