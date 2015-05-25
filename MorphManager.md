Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/MorphManager.h

The morph manager is used to morph Zerg units and buildings.

  * [update](#update.md)
  * [getName](#getName.md)
  * [getShortName](#getShortName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [morph](#morph.md)
  * [getPlannedCount](#getPlannedCount.md)
  * [getStartedCount](#getStartedCount.md)

## update ##
virtual void update();

Should be called in AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Morph Manager");

## getShortName ##
virtual std::string getShortName() const;

Returns std::string("Morph");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called in AIModule::onUnitDestroy.

## morph ##
bool morph(BWAPI::UnitType type);

Morphs the given unit type.

## getPlannedCount ##
int getPlannedCount(BWAPI::UnitType type) const;

Returns the planned count for the given unit type.

## getStartedCount ##
int getStartedCount(BWAPI::UnitType type) const;

Returns the started count for the given unit type.