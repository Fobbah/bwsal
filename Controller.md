Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/Controller.h

Each component of BWSAL that actually owns and controls units must inherit from Controller and work with the [Arbitrator](Arbitrator.md).

  * [onOffer](#onOffer.md)
  * [onRevoke](#onRevoke.md)
  * [getName](#getName.md)
  * [getShortName](#getShortName.md)
  * [update](#update.md)

## onOffer ##
virtual void onOffer(std::set<`_`Tp> objects)=0;

A callback the [Arbitrator](Arbitrator.md) uses when it wants to offer units to the controller. For each unit in the set, the [Controller](Controller.md) should either [accept](Arbitrator#accept.md) or [decline](Arbitrator#decline.md) each individual unit.

## onRevoke ##
virtual void onRevoke(`_`Tp, `_`Val bid)=0;

A callback the [Arbitrator](Arbitrator.md) uses when has to take a unit away from the [Controller](Controller.md), such as the case when another [Controller](Controller.md) has outbid this [Controller](Controller.md).

## getName ##
virtual std::string getName() const=0;

Should return the name of the [Controller](Controller.md).

## getShortName ##
virtual std::string getShortName() const;

Should return a short name of the [Controller](Controller.md).

## update ##
virtual void update()=0;

Called during AIModule::onFrame.