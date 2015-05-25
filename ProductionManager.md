Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/ProductionManager.h

Given a unit type, the production manager will attempt to get a suitable factory and train the given unit type.

  * [update](#update.md)
  * [getName](#getName.md)
  * [getShortName](#getShortName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [train](#train.md)
  * [getPlannedCount](#getPlannedCount.md)
  * [getStartedCount](#getStartedCount.md)
  * [getBuildType](#getBuildType.md)

## update ##
virtual void update();

Should be called in AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Production Manager");

## getShortName ##
virtual std::string getShortName() const;

Returns std::string("Prod");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called in AIModule::onUnitDestroy.

## train ##
bool train(BWAPI::UnitType type, bool forceNoAddon=false);

Trains the given unit type. If the second parameter is supplied and is true, then the unit will only be trained at a building that does not have an addon.

## getPlannedCount ##
int getPlannedCount(BWAPI::UnitType type) const;

Returns the planned count for the given unit type.

## getStartedCount ##
int getStartedCount(BWAPI::UnitType type) const;

Returns the started count for the given unit type.

## getBuildType ##
BWAPI::UnitType getBuildType(BWAPI::Unit`*` unit) const;

Returns the unit type that the given unit is about to train, or UnitTypes::None if the unit is not about to train anything.