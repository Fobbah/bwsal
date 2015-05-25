Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/BuildOrderManager.h

The build order manager accepts prioritized build, upgrade, and research commands and will attempt to execute the commands such that lower priority orders do not delay higher priority orders.

The dependency resolver, if enabled, will ensure that the minimum number of required units are created before executing a build, research, or upgrade command, and if not, it will queue up the required units.

For example, if you only execute build(30,UnitTypes::Protoss\_Carrier,50), the dependency resolver will build an Assimilator (for gas), Pylon, Gateway, Cybernetics Core, Stargate, Fleet Beacon, and finally start building Carriers.

Member functions:

  * [update](#update.md)
  * [getName](#getName.md)
  * [build](#build.md)
  * [buildAdditional](#buildAdditional.md)
  * [research](#research.md)
  * [upgrade](#upgrade.md)
  * [hasResources](#hasResources.md)
  * [spendResources](#spendResources.md)
  * [getPlannedCount](#getPlannedCount.md)
  * [enableDependencyResolver](#enableDependencyResolver.md)
  * [enableDebugMode](#enableDebugMode.md)
  * [unitsCanMake](#unitsCanMake.md)
  * [techsCanResearch](#techsCanResearch.md)
  * [upgradesCanResearch](#upgradesCanResearch.md)

## update ##
void update();

Should be called in AIModule::onFrame.

## getName ##
std::string getName() const;

Returns std::string("Build Order Manager");

## build ##
void build(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);

Order to make the player have the given number of units of the given type. Optionally a tile position may be specified. For example if the player has 4 vultures, and the AI calls build(20,UnitTypes::Terran\_Vulture,80) then the Build Order Manager will build vultures until the player has 20 total.

## buildAdditional ##
void buildAdditional(int count, BWAPI::UnitType t, int priority, BWAPI::TilePosition seedPosition=BWAPI::TilePositions::None);

Order to build the given number of units of the given type. Optionally a tile position may be specified. For example if the player has 4 vultures, and the AI calls buildAdditional(20,UnitTypes::Terran\_Vulture,80) then the Build Order Manager will build 20 additional vultures, resulting in a total of 24.

## research ##
void research(BWAPI::TechType t, int priority);

Order to research the given tech at the given priority.

## upgrade ##
void upgrade(int level, BWAPI::UpgradeType t, int priority);

Order to upgrade the given upgrade type to the given level at the given priority.

## hasResources ##
  * bool hasResources(BWAPI::UnitType t);
  * bool hasResources(BWAPI::TechType t);
  * bool hasResources(BWAPI::UpgradeType t);

Returns true if the player has enough resources for the given type (taking into account the Build Order Manager's resource reserve system).

## spendResources ##
  * void spendResources(BWAPI::UnitType t);
  * void spendResources(BWAPI::TechType t);
  * void spendResources(BWAPI::UpgradeType t);

Spends resources in the Build Order Manager's resource reserve system. Should be used if you plan to spend resources on units/buildings/tech/upgrades without using the BuildOrderManager (i.e. by calling the BuildManager directly or ordering a unit directly).

## getPlannedCount ##
int getPlannedCount(BWAPI::UnitType t);

Returns the currently planned number of units of the given type (includes started and completed units).

## enableDependencyResolver ##
void enableDependencyResolver();

Enables the dependency resolver, which will automatically queue up required units when needed.

## enableDebugMode ##
void enableDebugMode();

Enables debug mode, which will show a lot of debug information on the screen. Useful if you want to see what the Build Order Manager is currently planning to do.

## unitsCanMake ##
std::set<BWAPI::UnitType> unitsCanMake(BWAPI::Unit`*` builder, int time);

Returns the set of unit types the given unit will be able to make at the given time.

## techsCanResearch ##
std::set<BWAPI::TechType> techsCanResearch(BWAPI::Unit`*` techUnit, int time);

Returns the set of tech types the given unit will be able to research at the given time.

## upgradesCanResearch ##
std::set<BWAPI::UpgradeType> upgradesCanResearch(BWAPI::Unit`*` techUnit, int time);

Returns the set of upgrade types the given unit will be able to upgrade at the given time.