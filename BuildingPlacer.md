Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/BuildingPlacer.h

Finds a suitable build position for a given unit type. This is not a controller so it doesn't actually own units, rather it is used by the ConstructionManager.

  * [canBuildHere](BuildingPlacer#canBuildHere.md)
  * [canBuildHereWithSpace](BuildingPlacer#canBuildHereWithSpace.md)
  * [getBuildLocation](BuildingPlacer#getBuildLocation.md)
  * [getBuildLocationNear](BuildingPlacer#getBuildLocationNear.md)
  * [buildable](BuildingPlacer#buildable.md)
  * [reserveTiles](BuildingPlacer#reserveTiles.md)
  * [freeTiles](BuildingPlacer#freeTiles.md)
  * [setBuildDistance](BuildingPlacer#setBuildDistance.md)
  * [getBuildDistance](BuildingPlacer#getBuildDistance.md)

## canBuildHere ##
bool canBuildHere(BWAPI::TilePosition position, BWAPI::UnitType type) const;

Returns true if the given unit type can be constructed at the given build tile position. This uses BWAPI's Game::canBuildHere function, but also takes into account reserve tiles, and returns false if one of the tiles has been reserved for another building.

## canBuildHereWithSpace ##
bool canBuildHereWithSpace(BWAPI::TilePosition position, BWAPI::UnitType type) const;

Returns true if the given unit type can be constructed at the given build tile position with a certain amount of space (so the building is not immediately adjacent to another building, but has some walking room). The amount of tiles to be placed between buildings is the build distance, which by default is set to 1, but can be changed with [setBuildDistance](#setBuildDistance.md).

## getBuildLocation ##
BWAPI::TilePosition getBuildLocation(BWAPI::UnitType type) const;

Returns a valid build location on the map for the given unit type if one exists. The tile position returned is not necessarily near your base.

## getBuildLocationNear ##
BWAPI::TilePosition getBuildLocationNear(BWAPI::TilePosition position,BWAPI::UnitType type) const;

A basic build location algorithm that searches out from the given starting position in a spiral-like fashion until it finds a place it can build with space.

## buildable ##
bool buildable(int x, int y) const;

Returns true if the given build tile is buildable and there are no units currently on the build tile.

## reserveTiles ##
void reserveTiles(BWAPI::TilePosition position, int width, int height);

Reserves the given build tiles. This is needed to prevent the Construction manager from trying to build 2 buildings in the same spot.

## freeTiles ##
void freeTiles(BWAPI::TilePosition position, int width, int height);

Used to free the given build tiles. Generally this is down when a building is destroyed or lifted (like a Barracks).

## setBuildDistance ##
void setBuildDistance(int distance);

Sets the build distance to the specified value.

## getBuildDistance ##
int getBuildDistance();

Returns the current build distance (how much space to put between buildings)