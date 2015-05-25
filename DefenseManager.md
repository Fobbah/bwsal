Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/DefenseManager.h

The Defense Manager sends all non-worker units to a border assigned by the Border Manager.

  * [update](#update.md)
  * [getName](#getName.md)
  * [getShortName](#getShortName.md)
  * [onRemoveUnit](#onRemoveUnit.md)

## update ##
virtual void update();

Should be called from AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Defense Manager");

## getShortName ##
virtual std::string getShortName() const;

Returns std::string("Def");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called from AIModule::onUnitDestroy.