Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/TechManager.h

Used to research tech.

  * [update](#update.md)
  * [getName](#getName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [research](#research.md)
  * [planned](#planned.md)

## update ##
virtual void update();

Should be called from AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Tech Manager");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called in AIModule::onUnitDestroy.

## research ##
bool research(BWAPI::TechType type);

Call this to research the given tech type.

## planned ##
bool planned(BWAPI::TechType type) const;

Returns true if its currently trying to research the given tech type.