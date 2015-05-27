#pragma once
#include <BWAPI.h>
#include <BWTA.h>
namespace BWSAL
{
  enum FilterFlag
  {
    exists = 1,
    isAccelerating,
    isAttacking,
    isBeingConstructed,
    isBeingGathered,
    isBeingHealed,
    isBlind,
    isBraking,
    isBurrowed,
    isCarryingGas,
    isCarryingMinerals,
    isCloaked,
    isCompleted,
    isConstructing,
    isDefenseMatrixed,
    isDetected,
    isEnsnared,
    isFollowing,
    isGatheringGas,
    isGatheringMinerals,
    isHallucination,
    isHoldingPosition,
    isIdle,
    isInterruptible,
    isIrradiated,
    isLifted,
    isLoaded,
    isLockedDown,
    isMaelstrommed,
    isMorphing,
    isMoving,
    isParasited,
    isPatrolling,
    isPlagued,
    isRepairing,
    isResearching,
    isSelected,
    isSieged,
    isStartingAttack,
    isStasised,
    isStimmed,
    isStuck,
    isTraining,
    isUnderStorm,
    isUnpowered,
    isUpgrading,
    isVisible,

    canProduce,
    canAttack,
    canMove,
    isFlyer,
    regeneratesHP,
    isSpellcaster,
    hasPermanentCloak,
    isInvincible,
    isOrganic,
    isMechanical,
    isRobotic,
    isDetector,
    isResourceContainer,
    isResourceDepot,
    isRefinery,
    isWorker,
    requiresPsi,
    requiresCreep,
    isTwoUnitsInOneEgg,
    isBurrowable,
    isCloakable,
    isBuilding,
    isAddon,
    isFlyingBuilding,
    isNeutral,
    isHero,
    isPowerup,
    isBeacon,
    isFlagBeacon,
    isSpecialBuilding,
    isSpell,

    Firebat,
    Ghost,
    Goliath,
    Marine,
    Medic,
    SCV,
    Siege_Tank,
    Vulture,
    Vulture_Spider_Mine,
    Battlecruiser,
    Dropship,
    Nuclear_Missile,
    Science_Vessel,
    Valkyrie,
    Wraith,
    Alan_Schezar,
    Alexei_Stukov,
    Arcturus_Mengsk,
    Edmund_Duke,
    Gerard_DuGalle,
    Gui_Montag,
    Hyperion,
    Jim_Raynor_Marine,
    Jim_Raynor_Vulture,
    Magellan,
    Norad_II,
    Samir_Duran,
    Sarah_Kerrigan,
    Tom_Kazansky,
    Civilian,
    Academy,
    Armory,
    Barracks,
    Bunker,
    Command_Center,
    Engineering_Bay,
    Factory,
    Missile_Turret,
    Refinery,
    Science_Facility,
    Starport,
    Supply_Depot,
    Comsat_Station,
    Control_Tower,
    Covert_Ops,
    Machine_Shop,
    Nuclear_Silo,
    Physics_Lab,
    Crashed_Norad_II,
    Ion_Cannon,
    Power_Generator,
    Psi_Disrupter,

    Archon,
    Dark_Archon,
    Dark_Templar,
    Dragoon,
    High_Templar,
    Probe,
    Reaver,
    Scarab,
    Zealot,
    Arbiter,
    Carrier,
    Corsair,
    Interceptor,
    Observer,
    Scout,
    Shuttle,
    Aldaris,
    Artanis,
    Danimoth,
    Hero_Dark_Templar,
    Fenix_Dragoon,
    Fenix_Zealot,
    Gantrithor,
    Mojo,
    Raszagal,
    Tassadar,
    Tassadar_Zeratul_Archon,
    Warbringer,
    Zeratul,
    Arbiter_Tribunal,
    Assimilator,
    Citadel_of_Adun,
    Cybernetics_Core,
    Fleet_Beacon,
    Forge,
    Gateway,
    Nexus,
    Observatory,
    Photon_Cannon,
    Pylon,
    Robotics_Facility,
    Robotics_Support_Bay,
    Shield_Battery,
    Stargate,
    Templar_Archives,
    Khaydarin_Crystal_Form,
    Protoss_Temple,
    Stasis_Cell_Prison,
    Warp_Gate,
    XelNaga_Temple,

    Broodling,
    Defiler,
    Drone,
    Egg,
    Hydralisk,
    Infested_Terran,
    Larva,
    Lurker,
    Lurker_Egg,
    Ultralisk,
    Zergling,
    Cocoon,
    Devourer,
    Guardian,
    Mutalisk,
    Overlord,
    Queen,
    Scourge,
    Devouring_One,
    Hunter_Killer,
    Infested_Duran,
    Infested_Kerrigan,
    Kukulza_Guardian,
    Kukulza_Mutalisk,
    Matriarch,
    Torrasque,
    Unclean_One,
    Yggdrasill,
    Creep_Colony,
    Defiler_Mound,
    Evolution_Chamber,
    Extractor,
    Greater_Spire,
    Hatchery,
    Hive,
    Hydralisk_Den,
    Infested_Command_Center,
    Lair,
    Nydus_Canal,
    Queens_Nest,
    Spawning_Pool,
    Spire,
    Spore_Colony,
    Sunken_Colony,
    Ultralisk_Cavern,
    Cerebrate,
    Cerebrate_Daggoth,
    Mature_Chrysalis,
    Overmind,
    Overmind_Cocoon,
    Overmind_With_Shell,

    Bengalaas,
    Kakaru,
    Ragnasaur,
    Rhynadon,
    Scantid,
    Ursadon,

    Mineral_Field,
    Vespene_Geyser,

    Dark_Swarm,
    Disruption_Web,
    Scanner_Sweep,

    Protoss_Beacon,
    Protoss_Flag_Beacon,
    Terran_Beacon,
    Terran_Flag_Beacon,
    Zerg_Beacon,
    Zerg_Flag_Beacon,

    Powerup_Data_Disk,
    Powerup_Flag,
    Powerup_Khalis_Crystal,
    Powerup_Khaydarin_Crystal,
    Powerup_Psi_Emitter,
    Powerup_Uraj_Crystal,
    Powerup_Young_Chrysalis,

    None,
    Unknown_Unit
  };
  enum FliterAttributeScalar
  {
    HitPoints,
    InitialHitPoints,
    Shields,
    Energy,
    Resources,
    InitialResources,
    KillCount,
    GroundWeaponCooldown,
    AirWeaponCooldown,
    SpellCooldown,
    DefenseMatrixPoints,
    DefenseMatrixTimer,
    EnsnareTimer,
    IrradiateTimer,
    LockdownTimer,
    MaelstromTimer,
    PlagueTimer,
    RemoveTimer,
    StasisTimer,
    StimTimer,
    PositionX,
    PositionY,
    InitialPositionX,
    InitialPositionY,
    TilePositionX,
    TilePositionY,
    InitialTilePositionX,
    InitialTilePositionY,
    Angle,
    VelocityX,
    VelocityY,
    TargetPositionX,
    TargetPositionY,
    OrderTimer,
    RemainingBuildTime,
    RemainingTrainTime,
    TrainingQueueCount,
    LoadedUnitsCount,
    InterceptorCount,
    ScarabCount,
    SpiderMineCount,
    RemainingResearchTime,
    RemainingUpgradeTime,
    RallyPositionX,
    RallyPositionY
  };

  enum FilterAttributeUnit
  {
    GetTarget,
    GetOrderTarget,
    GetBuildUnit,
    GetTransport,
    GetRallyUnit,
    GetAddon
  };
  enum FilterAttributeType
  {
    GetType,
    GetInitialType,
    GetBuildType,
    GetTech,
    GetUpgrade
  };

  enum FilterAttributePosition
  {
    GetPosition,
    GetInitialPosition,
    GetTargetPosition,
    GetRallyPosition
  };

  enum FilterAttributeTilePosition
  {
    GetTilePosition,
    GetInitialTilePosition,
  };
  enum FilterAttributeOrder
  {
    GetOrder,
    GetSecondaryOrder
  };


  class UnitGroup : public std::set< BWAPI::Unit >
  {
    public:
      static UnitGroup& getUnitGroup( std::set< BWAPI::Unit > &units )
      {
        return *( ( UnitGroup* )( &units ) );
      }
      static const UnitGroup& getUnitGroup( const std::set< BWAPI::Unit > &units )
      {
        return *( ( UnitGroup* )( &units ) );
      }

      UnitGroup operator + ( const UnitGroup& other ) const; // Union
      UnitGroup operator|( const UnitGroup& other ) const; // Union
      UnitGroup operator*( const UnitGroup& other ) const; // Intersection
      UnitGroup operator&( const UnitGroup& other ) const; // Intersection
      UnitGroup operator^( const UnitGroup& other ) const; // Symmetric difference
      UnitGroup operator - ( const UnitGroup& other ) const; // Difference
      UnitGroup inRadius( double radius, BWAPI::Position position ) const;
      UnitGroup inRegion( BWTA::Region* region ) const;
      UnitGroup onlyNearestChokepoint( BWTA::Chokepoint* choke ) const;
      UnitGroup onlyNearestBaseLocation( BWTA::BaseLocation* location ) const;
      UnitGroup onlyNearestUnwalkablePolygon( BWTA::Polygon* polygon ) const;
      UnitGroup operator()( int f1 ) const;
      UnitGroup operator()( int f1, int f2 ) const;
      UnitGroup operator()( int f1, int f2, int f3 ) const;
      UnitGroup operator()( int f1, int f2, int f3, int f4 ) const;
      UnitGroup operator()( int f1, int f2, int f3, int f4, int f5 ) const;
      UnitGroup operator()( FliterAttributeScalar a, const char* compare, double value ) const;
      UnitGroup operator()( FliterAttributeScalar a, const char* compare, int value ) const;
      UnitGroup operator()( BWAPI::Player player ) const;
      UnitGroup operator()( FilterAttributeUnit a, BWAPI::Unit unit ) const;
      UnitGroup operator()( FilterAttributeType a, BWAPI::UnitType type ) const;
      UnitGroup operator()( FilterAttributeType a, BWAPI::TechType type ) const;
      UnitGroup operator()( FilterAttributeType a, BWAPI::UpgradeType type ) const;
      UnitGroup operator()( FilterAttributeOrder a, BWAPI::Order type ) const;
      UnitGroup operator()( FilterAttributePosition a, BWAPI::Position position ) const;
      UnitGroup operator()( FilterAttributeTilePosition a, BWAPI::TilePosition position ) const;

      UnitGroup not( int f1 ) const;
      UnitGroup not( int f1, int f2 ) const;
      UnitGroup not( int f1, int f2, int f3 ) const;
      UnitGroup not( int f1, int f2, int f3, int f4 ) const;
      UnitGroup not( int f1, int f2, int f3, int f4, int f5 ) const;
      UnitGroup not( FliterAttributeScalar a, const char* compare, double value ) const;
      UnitGroup not( FliterAttributeScalar a, const char* compare, int value ) const;
      UnitGroup not( BWAPI::Player player ) const;
      UnitGroup not( FilterAttributeUnit a, BWAPI::Unit unit ) const;
      UnitGroup not( FilterAttributeType a, BWAPI::UnitType type ) const;
      UnitGroup not( FilterAttributeType a, BWAPI::TechType type ) const;
      UnitGroup not( FilterAttributeType a, BWAPI::UpgradeType type ) const;
      UnitGroup not( FilterAttributeOrder a, BWAPI::Order type ) const;
      UnitGroup not( FilterAttributePosition a, BWAPI::Position position ) const;
      UnitGroup not( FilterAttributeTilePosition a, BWAPI::TilePosition position ) const;

      UnitGroup& operator += ( const UnitGroup& other ); // Union
      UnitGroup& operator |= ( const UnitGroup& other ); // Union
      UnitGroup& operator *= ( const UnitGroup& other ); // Intersection
      UnitGroup& operator &= ( const UnitGroup& other ); // Intersection
      UnitGroup& operator ^= ( const UnitGroup& other ); // Symmetric difference
      UnitGroup& operator -= ( const UnitGroup& other ); // Difference
      BWAPI::Unit getNearest( BWAPI::Position position ) const;
      bool contains( BWAPI::Unit u ) const;
      BWAPI::Position getCenter() const;

      /** Orders the unit to attack move to the specified location. */
      bool attack( BWAPI::Position target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to attack the specified unit. */
      bool attack( BWAPI::Unit target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to build the given unit type at the given position. Note that if the player does not
       * have enough resources when the unit attempts to place the building down, the order will fail. The
       * tile position specifies where the top left corner of the building will be placed. */
      bool build( BWAPI::TilePosition target, BWAPI::UnitType type ) const;

      /** Orders the unit to build the given addon. The unit must be a Terran building that can have an addon
       * and the specified unit type must be an addon unit type. */
      bool buildAddon( BWAPI::UnitType type ) const;

      /** Orders this unit to add the specified unit type to the training queue. Note that the player must
       * have sufficient resources to train. If you wish to make units from a hatchery, use getLarva to get
       * the larva associated with the hatchery and then call morph on the larva you want to morph. This
       * command can also be used to make interceptors and scarabs. */
      bool train( BWAPI::UnitType type ) const;

      /** Orders the unit to morph into the specified unit type. Returns false if given a wrong type.
       * \see Unit::cancelMorph, Unit::isMorphing. */
      bool morph( BWAPI::UnitType type ) const;

      /** Orders the unit to research the given tech type.
       * \see Unit::cancelResearch, Unit::Unit#isResearching, Unit::getRemainingResearchTime, Unit::getTech. */
      bool research( BWAPI::TechType tech ) const;

      /** Orders the unit to upgrade the given upgrade type.
       * \see Unit::cancelUpgrade, Unit::Unit#isUpgrading, Unit::getRemainingUpgradeTime, Unit::getUpgrade. */
      bool upgrade( BWAPI::UpgradeType upgrade ) const;

      /** Orders the unit to set its rally position to the specified position.
       * \see Unit::getRallyPosition, Unit::getRallyUnit. */
      bool setRallyPoint( BWAPI::Position target ) const;

      /** Orders the unit to set its rally unit to the specified unit.
       * \see Unit::setRallyPosition, Unit::getRallyPosition, Unit::getRallyUnit. */
      bool setRallyPoint( BWAPI::Unit target ) const;

      /** Orders the unit to move from its current position to the specified position.
       * \see Unit::isMoving.  */
      bool move( BWAPI::Position target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to patrol between its current position and the specified position.
       * \see Unit::isPatrolling.  */
      bool patrol( BWAPI::Position target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to hold its position.*/
      bool holdPosition( bool shiftQueueCommand = false ) const;

      /** Orders the unit to stop. */
      bool stop( bool shiftQueueCommand = false ) const;

      /** Orders the unit to follow the specified unit.
       * \see Unit::isFollowing. */
      bool follow( BWAPI::Unit target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to gather the specified unit ( must be mineral or refinery type ).
       * \see Unit::isGatheringGas, Unit::isGatheringMinerals. */
      bool gather( BWAPI::Unit target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to return its cargo to a nearby resource depot such as a Command Center. Only
       * workers that are carrying minerals or gas can be ordered to return cargo.
       * \see Unit::isCarryingGas, Unit::isCarryingMinerals. */
      bool returnCargo( bool shiftQueueCommand = false ) const;

      /** Orders the unit to repair the specified unit. Only Terran SCVs can be ordered to repair, and the
       * target must be a mechanical Terran unit or building.
       * \see Unit::isRepairing. */
      bool repair( BWAPI::Unit target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to burrow. Either the unit must be a Zerg Lurker, or the unit must be a Zerg ground
       * unit and burrow tech must be researched.
       * \see: Unit::unburrow, Unit::isBurrowed. */
      bool burrow() const;

      /** Orders the burrowed unit to unburrow.
       * \see: Unit::burrow, Unit::isBurrowed.
       * */
      bool unburrow() const;

      /** Orders the unit to cloak.
       * \see: Unit::decloak, Unit::isCloaked. */
      bool cloak() const;

      /** Orders the unit to decloak.
       * \see: Unit::cloak, Unit::isCloaked. */
      bool decloak() const;

      /** Orders the unit to siege. Note: unit must be a Terran siege tank.
       * \see Unit::unsiege, Unit::isSieged. */
      bool siege() const;

      /** Orders the unit to unsiege. Note: unit must be a Terran siege tank.
       * \see: Unit::unsiege, Unit::isSieged. */
      bool unsiege() const;

      /** Orders the unit to lift. Note: unit must be a Terran building that can be lifted.
       * \see Unit::land, Unit::isLifted.  */
      bool lift() const;

      /** Orders the unit to land. Note: unit must be a Terran building that is currently lifted.
       * \see Unit::lift, Unit::isLifted. */
      bool land( BWAPI::TilePosition target ) const;

      /** Orders the unit to load the target unit.
       * \see Unit::unload, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded. */
      bool load( BWAPI::Unit target, bool shiftQueueCommand = false ) const;

      /** Orders the unit to unload the target unit.
       * \see Unit::load, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded. */
      bool unload( BWAPI::Unit target ) const;

      /** Orders the unit to unload all loaded units at the unit's current position.
       * \see Unit::load, Unit::unload, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded. */
      bool unloadAll( bool shiftQueueCommand = false ) const;

      /** Orders the unit to unload all loaded units at the specified location. Unit should be a Terran
       * Dropship, Protoss Shuttle, or Zerg Overlord. If the unit is a Terran Bunker, the units will be
       * unloaded right outside the bunker, like in the first version of unloadAll.
       * \see Unit::load, Unit::unload, Unit::unloadAll, Unit::getLoadedUnits, Unit:isLoaded. */
      bool unloadAll( BWAPI::Position target, bool shiftQueueCommand = false ) const;

      /** Works like the right click in the GUI. */
      bool rightClick( BWAPI::Position target, bool shiftQueueCommand = false ) const;

      /** Works like the right click in the GUI. Right click on a mineral patch to order a worker to mine,
       * right click on an enemy to attack it. */
      bool rightClick( BWAPI::Unit target, bool shiftQueueCommand = false ) const;

      /** Orders the SCV to stop constructing the building, and the building is left in a partially complete
       * state until it is canceled, destroyed, or completed.
       * \see Unit::isConstructing. */
      bool haltConstruction() const;

      /** Orders the building to stop being constructed.
       * \see Unit::beingConstructed. */
      bool cancelConstruction() const;

      /** Orders the unit to stop making the addon. */
      bool cancelAddon() const;

      /** Orders the unit to remove the specified unit from its training queue.
       * \see Unit::train, Unit::cancelTrain, Unit::isTraining, Unit::getTrainingQueue. */
      bool cancelTrain( int slot = -2 ) const;

      /** Orders the unit to stop morphing.
       * \see Unit::morph, Unit::isMorphing. */
      bool cancelMorph() const;

      /** Orders the unit to cancel a research in progress.
       * \see Unit::research, Unit::isResearching, Unit::getTech. */
      bool cancelResearch() const;

      /** Orders the unit to cancel an upgrade in progress.
       * \see Unit::upgrade, Unit::isUpgrading, Unit::getUpgrade. */
      bool cancelUpgrade() const;

      /** Orders the unit to use a tech not requiring a target ( ie Stim Pack ). Returns true if it is a valid
       * tech. */
      bool useTech( BWAPI::TechType tech ) const;

      /** Orders the unit to use a tech requiring a position target ( ie Dark Swarm ). Returns true if it is a
       * valid tech.*/
      bool useTech( BWAPI::TechType tech, BWAPI::Position target ) const;

      /** Orders the unit to use a tech requiring a unit target ( ie Irradiate ). Returns true if it is a valid
       * tech.*/
      bool useTech( BWAPI::TechType tech, BWAPI::Unit target ) const;
  };
}