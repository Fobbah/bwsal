Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/BorderManager.h

A border is a set of chokepoints that divides the map into two or more parts. The Border Manager can be used both to get the border around our base (i.e. for defense), and also for getting the border around the enemy's base (i.e. for containment).

  * [addMyBase](#addMyBase.md)
  * [removeMyBase](#removeMyBase.md)
  * [getMyBorder](#getMyBorder.md)
  * [getEnemyBorder](#getEnemyBorder.md)
  * [getMyRegions](#getMyRegions.md)
  * [getEnemyRegions](#getEnemyRegions.md)
  * [update](#update.md)

## addMyBase ##
void addMyBase(BWTA::BaseLocation`*` location);

Call when you are about to expand to the given base location.

## removeMyBase ##
void removeMyBase(BWTA::BaseLocation`*` location);

Call when you are abandoning a base at the given base location.

## getMyBorder ##
const std::set<BWTA::Chokepoint`*`>& getMyBorder() const;

Returns the border around our base. Useful for defending against ground attacks.

## getEnemyBorder ##
const std::set<BWTA::Chokepoint`*`>& getEnemyBorder() const;

Returns the border around the enemy base. Useful for containing the enemy in his base.

## getMyRegions ##
const std::set<BWTA::Chokepoint`*`>& getMyRegions() const;

Returns the set of regions controlled by our player.

## getEnemyRegions ##
const std::set<BWTA::Chokepoint`*`>& getEnemyRegions() const;

Returns the set of regions controlled by the enemy.

## update ##
void update();

Should be called from AIModule::onFrame.