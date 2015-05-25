Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/InformationManager.h

Uses logic to infer things. For example, if you see a Corsair, the information manager will know a Stargate was created at some point, and determines an upper bound on when it was created. Also if you see a unit and never saw an onUnitDestroy message for it, the information manager will assume it still exists. Also keeps track of last seen positions of each unit and the time the unit was last seen.

  * [onUnitShow](#onUnitShow.md)
  * [onUnitHide](#onUnitHide.md)
  * [onUnitDestroy](#onUnitDestroy.md)
  * [getPlayer](#getPlayer.md)
  * [getType](#getType.md)
  * [getLastPosition](#getLastPosition.md)
  * [getLastSeenTime](#getLastSeenTime.md)
  * [exists](#exists.md)
  * [enemyHasBuilt](#enemyHasBuilt.md)
  * [getBuildTime](#getBuildTime.md)

## onUnitShow ##
void onUnitShow(BWAPI::Unit`*` unit);

Should be called from AIModule::onUnitShow.

## onUnitHide ##
void onUnitHide(BWAPI::Unit`*` unit);

Should be called from AIModule::onUnitHide.

## onUnitDestroy ##
void onUnitDestroy(BWAPI::Unit`*` unit);

Should be called from AIModule::onUnitDestroy.

## getPlayer ##
BWAPI::Player`*` getPlayer(BWAPI::Unit`*` unit);

Returns unit's last seen player.

## getType ##
BWAPI::UnitType getType(BWAPI::Unit`*` unit);

Returns the unit's last seen type.

## getLastPosition ##
BWAPI::Position getLastPosition(BWAPI::Unit`*` unit);

Returns the unit's last seen position.

## getLastSeenTime ##
int getLastSeenTime(BWAPI::Unit`*` unit);

Returns the last time the unit was seen.

## exists ##
bool exists(BWAPI::Unit`*` unit);

Returns true if the information manager believes the unit exists, or false if the information manager saw the unit get destroyed via onUnitDestroy.

## enemyHasBuilt ##
bool enemyHasBuilt(BWAPI::UnitType type);

Returns true if the enemy has build at least one unit of the given type.

## getBuildTime ##
int getBuildTime(BWAPI::UnitType type);

Returns the latest possible time that the enemy could have built its first unit of the given type.