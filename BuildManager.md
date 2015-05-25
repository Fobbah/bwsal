Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/BuildManager.h

The build manager is used to build units and buildings. To start building a new unit, simply call BuildManager::build([UnitType](http://code.google.com/p/bwapi/wiki/UnitType) type).

When a new unit type request comes in, the build manager will pass the unit type off to one of the three sub-build managers:
  * [Construction Manager](ConstructionManager.md) - handles creating new buildings and add-ons
  * [Production Manager](ProductionManager.md) - handles training units from production buildings
  * [Morph Manager](MorphManager.md) - handles morphing units in to other units.

Methods:

  * [update](#update.md)
  * [getName](#getName.md)
  * [getShortName](#getShortName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [build](#build.md)
  * [getPlannedCount](#getPlannedCount.md)
  * [getStartedCount](#getStartedCount.md)
  * [getCompletedCount](#getCompletedCount.md)
  * [setBuildDistance](#setBuildDistance.md)
  * [getBuildType](#getBuildType.md)

## update ##
void update();

Should be called during AIModule::onFrame.

## getName ##
std::string getName() const;

Returns std::string("Build Manager");

## getShortName ##
std::string getShortName() const;

Returns std::string("Build");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called during AIModule::onUnitDestroy.

## build ##
bool build(BWAPI::UnitType type);

bool build(BWAPI::UnitType type, bool forceNoAddon);

bool build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition);

bool build(BWAPI::UnitType type, BWAPI::TilePosition goalPosition, bool forceNoAddon);

Call this to build a new unit of the given type. If the unit is a building, an optional goalPosition can be used to specified where the building should be placed near. If no goal position is specified, the goal position will be the player's starting position so new buildings are placed near the main base.

## getPlannedCount ##
int getPlannedCount(BWAPI::UnitType type) const;

Returns the number of units planned for creation. Includes all started and completed units, so getPlannedCount >= getStartedCount >= getCompletedCount.

## getStartedCount ##
int getStartedCount(BWAPI::UnitType type) const;

Returns the number of units that have started construction/training/morphing, or have been completed. Includes all completed units, so getStartedCount >= getCompletedCount.

## getCompletedCount ##
int getCompletedCount(BWAPI::UnitType type) const;

Simply calls BWAPI::Broodwar->self()->completedUnitCount(type).

## setBuildDistance ##
void setBuildDistance(int distance);

Sets the build distance for the building placer.

## getBuildType ##
BWAPI::UnitType getBuildType(BWAPI::Unit`*` unit) const;

Returns the unit type that the given unit is about to train, or UnitTypes::None if the unit is not about to train anything.