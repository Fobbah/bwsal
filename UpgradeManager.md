Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/UpgradeManager.h

Used to research upgrades.

  * [update](#update.md)
  * [getName](#getName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [upgrade](#upgrade.md)
  * [getPlannedLevel](#getPlannedLevel.md)
  * [getStartedLevel](#getStartedLevel.md)
  * [getCompletedLevel](#getCompletedLevel.md)

## update ##
virtual void update();

Should be called from AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Upgrade Manager");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called from AIModule::onUnitDestroy.

## upgrade ##
bool upgrade(BWAPI::UpgradeType type);

Call this to upgrade the given upgrade type.

## getPlannedLevel ##
int getPlannedLevel(BWAPI::UpgradeType type) const;

Returns the planned upgrade level for the given type.

## getStartedLevel ##
int getStartedLevel(BWAPI::UpgradeType type) const;

Returns the started upgrade level for the given type.

## getCompletedLevel ##
int getCompletedLevel(BWAPI::UpgradeType type) const;

Simply calls BWAPI::Broodwar->self()->getUpgradeLevel(type).