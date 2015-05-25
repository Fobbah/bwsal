Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/SupplyManager.h

Keeps an eye on the supply count provided and supply count used, and orders the BuildManager to build more supply providing units as needed to ensure that the AI never gets supply blocked.

  * [update](#update.md)
  * [getName](#getName.md)
  * [getPlannedSupply](#getPlannedSupply.md)

## update ##
void update();

Should be called in AIModule::onFrame.

## getName ##
std::string getName() const;

Returns std::string("Supply Manager");

## getPlannedSupply ##
int getPlannedSupply() const;

Returns the currently planned supply.