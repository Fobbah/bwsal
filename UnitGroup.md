Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/UnitGroup.h

The UnitGroup class is defined as a std::set<BWAPI::Unit`*`> object with a number of additional methods:

  * [getCenter](#getCenter.md)
  * [inRadius](#inRadius.md)
  * [inRegion](#inRegion.md)
  * [onlyNearestChokepoint](#onlyNearestChokepoint.md)
  * [onlyNearestBaseLocation](#onlyNearestBaseLocation.md)
  * [onlyNearestUnwalkablePolygon](#onlyNearestUnwalkablePolygon.md)
  * [operator()](#operator().md)
  * [not](#not.md)
  * [operator+](#operator+.md)
  * [operator\*](#operator*.md)
  * [operator^](#operator^.md)
  * [operator-](#operator-.md)

### getCenter ###
BWAPI::Position getCenter() const;

Returns the average position of all the units in the UnitGroup.

### inRadius ###
UnitGroup inRadius(double radius,BWAPI::Position position) const;

Returns a UnitGroup containing all the units of this UnitGroup which have a distance of less than the given radius to the given position.

### inRegion ###
UnitGroup inRegion(BWTA::Region`*` region) const;

Returns a UnitGroup containing all the units of this UnitGroup which are in the given region.

### onlyNearestChokepoint ###
UnitGroup onlyNearestChokepoint(BWTA::Chokepoint`*` choke) const;

Returns a UnitGroup containing all the units of this UnitGroup which are closer to the given chokepoint than to any other chokepoint on the map.

### onlyNearestBaseLocation ###
UnitGroup onlyNearestBaseLocation(BWTA::BaseLocation`*` location) const;

Returns a UnitGroup containing all the units of this UnitGroup which are closer to the given base location than to any other base location on the map.

### onlyNearestUnwalkablePolygon ###
UnitGroup onlyNearestUnwalkablePolygon(BWTA::Polygon`*` polygon) const;

Returns a UnitGroup containing all the units of this UnitGroup which are closer to the given unwalkable polygon than to any other unwalkable polygon on the map.

### operator() ###
  * UnitGroup operator()(int f1) const;
  * UnitGroup operator()(int f1, int f2) const;
  * UnitGroup operator()(int f1, int f2, int f3) const;
  * UnitGroup operator()(int f1, int f2, int f3, int f4) const;
  * UnitGroup operator()(int f1, int f2, int f3, int f4, int f5) const;
  * UnitGroup operator()(FliterAttributeScalar a, const char`*` compare, double value) const;
  * UnitGroup operator()(FliterAttributeScalar a, const char`*` compare, int value) const;
  * UnitGroup operator()(BWAPI::Player`*` player) const;
  * UnitGroup operator()(FilterAttributeUnit a, BWAPI::Unit`*` unit) const;
  * UnitGroup operator()(FilterAttributeType a, BWAPI::UnitType type) const;
  * UnitGroup operator()(FilterAttributeType a, BWAPI::TechType type) const;
  * UnitGroup operator()(FilterAttributeType a, BWAPI::UpgradeType type) const;
  * UnitGroup operator()(FilterAttributeOrder a, BWAPI::Order type) const;
  * UnitGroup operator()(FilterAttributePosition a, BWAPI::Position position) const;
  * UnitGroup operator()(FilterAttributeTilePosition a, BWAPI::TilePosition position) const;

operator() can be used to create a UnitGroup containing only the units that satisfy one of the specified properties. For example, the following 2 lines creates a UnitGroup called myWorkers which holds all the worker units owned by the player:

UnitGroup myUnits=SelectAll()

UnitGroup myWorkers = myUnits(isWorker)

This can be further narrowed down to only idle workers by another call to operator():

UnitGroup myIdleWorkers = myWorkers(isIdle)

Finally, we can compress this to just 1 line:

UnitGroup myIdleWorkers = SelectAll(isWorker)(isIdle)

Also all of the Unit command methods also exist in the UnitGroup class, making it easy to issue the same command to every unit in the UnitGroup.

As another example, the following gets all units that are gathering minerals or gas:

UnitGroup gatheringUnits = SelectAll()(isGatheringMinerals,isGatheringGas);

While the following returns an empty set (since no unit can gather minerals and gas at the same time):

UnitGroup nothing = SelectAll()(isGatheringMinerals)(isGatheringGas);

### not ###
  * UnitGroup not(int f1) const;
  * UnitGroup not(int f1, int f2) const;
  * UnitGroup not(int f1, int f2, int f3) const;
  * UnitGroup not(int f1, int f2, int f3, int f4) const;
  * UnitGroup not(int f1, int f2, int f3, int f4, int f5) const;
  * UnitGroup not(FliterAttributeScalar a, const char`*` compare, double value) const;
  * UnitGroup not(FliterAttributeScalar a, const char`*` compare, int value) const;
  * UnitGroup not(BWAPI::Player`*` player) const;
  * UnitGroup not(FilterAttributeUnit a, BWAPI::Unit`*` unit) const;
  * UnitGroup not(FilterAttributeType a, BWAPI::UnitType type) const;
  * UnitGroup not(FilterAttributeType a, BWAPI::TechType type) const;
  * UnitGroup not(FilterAttributeType a, BWAPI::UpgradeType type) const;
  * UnitGroup not(FilterAttributeOrder a, BWAPI::Order type) const;
  * UnitGroup not(FilterAttributePosition a, BWAPI::Position position) const;
  * UnitGroup not(FilterAttributeTilePosition a, BWAPI::TilePosition position) const;

Returns the complement of the corresponding operator() method call.

### operator+ ###
UnitGroup operator+(const UnitGroup& other) const;

Returns the union of this UnitGroup with the specified UnitGroup.

### operator`*` ###
UnitGroup operator`*`(const UnitGroup& other) const;

Returns the intersection of this UnitGroup with the specified UnitGroup.

### operator`^` ###
UnitGroup operator`^`(const UnitGroup& other) const;

Returns the symmetric difference of this UnitGroup with the specified UnitGroup.

### operator- ###
UnitGroup operator-(const UnitGroup& other) const;

Returns the difference obtained when subtracting the specified unit group from this unit group.