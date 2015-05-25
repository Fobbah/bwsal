Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/ConstructionManager.h

Given a unit type, the construction manager will attempt to get a suitable worker and build the given unit type at the build position returned by BuildingPlacer.

  * [update](#update.md)
  * [getName](#getName.md)
  * [getShortName](#getShortName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [build](#build.md)
  * [getPlannedCount](#getPlannedCount.md)
  * [getStartedCount](#getStartedCount.md)

## update ##
virtual void update();

Should be called in AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Construction Manager");

## getShortName ##
virtual std::string getShortName() const;

Returns std::string("Con");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called in AIModule::onUnitDestroy.

## build ##
bool build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition);

Builds the given unit type near the given goal position. Unit type must be a building.

## getPlannedCount ##
int getPlannedCount(BWAPI::UnitType type) const;

Returns the planned count for the given unit type.

## getStartedCount ##
int getStartedCount(BWAPI::UnitType type) const;

Returns the started count for the given unit type.