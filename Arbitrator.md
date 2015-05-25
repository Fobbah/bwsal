Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/Arbitrator/Arbitrator.h

The Arbitrator facilitates cooperation between controllers (add-ons that order units around) and decides which controller gets which units, however the class uses templates so it can work with any kind of object.

  * [setBid](#setBid.md)
  * [removeBid](#removeBid.md)
  * [removeAllBids](#removeAllBids.md)
  * [accept](#accept.md)
  * [decline](#decline.md)
  * [hasBid](#hasBid.md)
  * [getHighestBidder](#getHighestBidder.md)
  * [getAllBidders](#getAllBidders.md)
  * [getObjects](#getObjects.md)
  * [onRemoveObject](#onRemoveObject.md)
  * [getBid](#getBid.md)
  * [update](#update.md)

## setBid ##
bool setBid(Controller<`_`Tp,`_`Val>`*` c, `_`Tp obj, `_`Val bid);

bool setBid(Controller<`_`Tp,`_`Val>`*` c, std::set<`_`Tp> objs, `_`Val bid);

Sets the bid of the given object or set of objects to the given value. Bids cannot be increased from onRevoke(), and in onOffer only the offered units can have their bids increased.

## removeBid ##
bool removeBid(Controller<`_`Tp,`_`Val>`*` c, `_`Tp obj);


bool removeBid(Controller<`_`Tp,`_`Val>`*` c, std::set<`_`Tp> objs, `_`Val bid);

Removes the bid for the given object or set of objects.

## removeAllBids ##
bool removeAllBids(Controller<_Tp,_Val>`*` c);

Removes all bids for a given controller. Should be called when the controller doesn't need any objects any more (such as in the controller's destructor).

## accept ##
bool accept(Controller<`_`Tp,`_`Val>`*` c, `_`Tp obj);

bool accept(Controller<`_`Tp,`_`Val>`*` c, std::set<`_`Tp> objs);

bool accept(Controller<`_`Tp,`_`Val>`*` c, `_`Tp obj, `_`Val bid);

bool accept(Controller<`_`Tp,`_`Val>`*` c, std::set<`_`Tp> objs, `_`Val bid);

Accept the given object or set of objects. Only call from Controller::onOffer.

## decline ##
bool decline(Controller<`_`Tp,`_`Val>`*` c, `_`Tp obj, `_`Val bid);

bool decline(Controller<`_`Tp,`_`Val>`*` c, std::set<`_`Tp> objs, `_`Val bid);

Decline the given object or set of objects, with the specified new bid value. Only call from Controller::onOffer.

## hasBid ##
bool hasBid(`_`Tp obj) const;

Returns true if the given object has at least one bid.

## getHighestBidder ##
const std::pair<Controller<`_`Tp,`_`Val>`*`, `_`Val>& getHighestBidder(`_`Tp obj) const;

Returns pair containing the highest bidder for the object, along with the value of the highest bid.

## getAllBidders ##
const std::list< std::pair<Controller<`_`Tp,`_`Val>`*`, `_`Val> > getAllBidders(`_`Tp obj) const;

Returns a list of all the bidders for the given object.

## getObjects ##
const std::set<`_`Tp>& getObjects(Controller<`_`Tp,`_`Val>`*` c) const;

Returns all the objects owned by the given controller.

## onRemoveObject ##
void onRemoveObject(`_`Tp obj);

Must be called from AIModule::onUnitDestroy.

## getBid ##
`_`Val getBid(Controller<`_`Tp,`_`Val>`*` c, `_`Tp obj) const;

Returns the bid the given controller has placed for the given object.

## update ##
void update();

Must be called from AIModule::onUpdate.