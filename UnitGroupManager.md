Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/UnitGroupManager.h

Global functions:

  * [AllUnits](#AllUnits.md)
  * [SelectAll](#SelectAll.md)
  * [SelectAllEnemy](#SelectAllEnemy.md)

### AllUnits ###
UnitGroup AllUnits();

Returns a UnitGroup containing all accessible units.

### SelectAll ###
UnitGroup SelectAll();

Returns a UnitGroup containing all units owned by our player.

UnitGroup SelectAll(BWAPI::UnitType type);

Returns a UnitGroup containing all units of the given type owned by our player (or owned by neutral, if the unit type is a neutral unit type).

UnitGroup SelectAll(BWAPI::Player`*` player, BWAPI::UnitType type);

Returns a UnitGroup containing all units of the given type owned by the given player.

### SelectAllEnemy ###
UnitGroup SelectAllEnemy();

Returns a UnitGroup containing all units owned by the enemy player.

UnitGroup SelectAllEnemy(BWAPI::UnitType type);

Returns a UnitGroup containing all units of the given type owned by the enemy player.

Member functions:

  * [onUnitShow](#onUnitShow.md)
  * [onUnitHide](#onUnitHide.md)
  * [onUnitMorph](#onUnitMorph.md)
  * [onUnitRenegade](#onUnitRenegade.md)

### onUnitShow ###
void onUnitShow(BWAPI::Unit**unit);**

Should be called from AIModule::onUnitShow.

### onUnitHide ###
void onUnitHide(BWAPI::Unit**unit);**

Should be called from AIModule::onUnitHide.

### onUnitMorph ###
void onUnitMorph(BWAPI::Unit**unit);**

Should be called from AIModule::onUnitMorph .

### onUnitRenegade ###
void onUnitRenegade(BWAPI::Unit**unit);**

Should be called from AIModule::onUnitRenegade.