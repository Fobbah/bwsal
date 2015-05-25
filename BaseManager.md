Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/BaseManager.h

Keeps track of each base and handles requests to activate and deactivate individual bases (useful to react to storm drops).

  * [update](#update.md)
  * [getBase](#getBase.md)
  * [expand](#expand.md)
  * [getActiveBases](#getActiveBases.md)
  * [getAllBases](#getAllBases.md)
  * [getName](#getName.md)
  * [onRemoveUnit](#onRemoveUnit.md)

## update ##
void update();

Should be called in AIModule::onFrame.

## getBase ##
Base`*` getBase(BWTA::BaseLocation`*` location);

Returns the Base that is at the specified BaseLocation, or NULL if no base is there.

## expand ##
void expand(int priority = 100);

void expand(BWTA::BaseLocation`*` location, int priority = 100);

Call this to expand to the given base location. Note that nothing yet is done to clear the given base location of mines, burrowed units, blocking mineral patches, or other buildings. If no base location is specified, then the manager picks the closest base location to the player's starting location that hasn't been expanded to yet, and expands there.

## getActiveBases ##
std::set<Base`*`> getActiveBases() const;

Returns the set of active bases (bases currently mining).

## getAllBases ##
std::set<Base`*`> getAllBases() const;

Returns the set of all bases.

## getName ##
std::string getName();

Returns std::string("Base Manager");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Uni`*` unit);

Should be called in AIModule::onUnitDestroy.