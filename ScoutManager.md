Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/ScoutManager.h

The scout manager takes a worker and sends it to all base locations to scout the enemy.

  * [update](#update.md)
  * [getName](#getName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [isScouting](#isScouting.md)
  * [setScoutCount](#setScoutCount.md)

## update ##
virtual void update();

Should be called from AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Scout Manager");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called from AIModule::onUnitDestroy.

## isScouting ##
bool isScouting() const;

Returns true if there is at least 1 active scouting unit.

## setScoutCount ##
void setScoutCount(int count);

Sets the number of scouting units to the given amount.