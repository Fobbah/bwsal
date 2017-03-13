#pragma once
#include <BWAPI.h>

namespace BWSAL
{
  /*! 
   * \brief Simple composition that can represent a UnitType, TechType OR UpgradeType to be produced.
   *  Useful for scheduling a single ordered list of tasks to produce that can span any of these types.
   *  Comes with many useful utility functions to interact with these tasks in a unified way.
   */
  class BuildType
  {
    public:
      BuildType();
      BuildType( int id );
      BuildType( const BuildType& other );
      BuildType( const BWAPI::TechType& other );
      BuildType( const BWAPI::UnitType& other );
      BuildType( const BWAPI::UpgradeType& other, int level = 1 );
      BuildType& operator=( const BuildType& other );
      BuildType& operator=( const BWAPI::TechType& other );
      BuildType& operator=( const BWAPI::UnitType& other );
      BuildType& operator=( const BWAPI::UpgradeType& other );
      bool operator==( const BuildType& other ) const;
      bool operator==( const BWAPI::TechType& other ) const;
      bool operator==( const BWAPI::UnitType& other ) const;
      bool operator==( const BWAPI::UpgradeType& other ) const;
      bool operator<( const BuildType& other ) const;
      operator int() const;

	  //! Returns the unique ID of this BuildType
      int getID() const;

	  //! Returns the name of this BuildType
      const std::string& getName() const;

	  //! Returns the race of this BuildType
      BWAPI::Race getRace() const;

	  //! Returns true if this BuildType is a TechType
      bool isTechType() const;

	  //! Returns true if this BuildType is a UnitType
      bool isUnitType() const;

	  //! Returns true if this BuildType is an UpgradeType
      bool isUpgradeType() const;

	  //! Return the TechType for this BuildType.
      BWAPI::TechType getTechType() const;

	  //! Return the UnitType for this BuildType.
      BWAPI::UnitType getUnitType() const;

	  //! Return the UpgradeType for this BuildType.
      BWAPI::UpgradeType getUpgradeType() const;

	  //! Return the level of the UpgradeType for this BuildType
      int getUpgradeLevel() const;
      unsigned int getMask() const;
      unsigned int getRequiredMask() const;

	  //! Returns the list of BuildType that build this BuildType
      const std::pair< BuildType, int > whatBuilds() const;

	  //! Returns the list of BuildType that are tech tree requirements to build this BuildType
      const std::map< BuildType, int >& requiredBuildTypes() const;

	  //! Returns true if this BuildType needs a Psi field
      bool requiresPsi() const;

	  //! Returns true if this BuildType requires larva
      bool requiresLarva() const;

	  //! Returns true if this BuildType requires an Addon for the Builder
      BuildType requiredAddon() const;

	  //! Returns the mineral cost to produce this BuildType
      int mineralPrice() const;

	  //! Returns the gas cost to produce this BuildType
      int gasPrice() const;

      int builderTime() const;
      int buildUnitTime() const;
      int prepTime() const;
      bool createsUnit() const;

	  //! Returns true if this BuildType morphs the builder when constructed
      bool morphsBuilder() const;

	  //! Returns true if this BuildType requires a construction location (eg, a building)
      bool needsBuildLocation() const;

	  //! Returns the amount of supply required to support this BuildType
      int supplyRequired() const;

	  //! Returns the amount of supply provided by this BuildType
      int supplyProvided() const;

      bool build( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::TilePosition buildLocation ) const;
      bool isPreparing( BWAPI::Unit builder, BWAPI::Unit secondBuilder ) const;
      bool isBuilding( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::Unit createdUnit ) const;
      bool isCompleted( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::Unit createdUnit, BWAPI::Unit secondCreatedUnit ) const;
      int remainingTime( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::Unit createdUnit ) const;
    private:
      int id;
  };
  namespace BuildTypes
  {
    BuildType getBuildType( const std::string name );
    std::set< BuildType >& allBuildTypes();
    std::set< BuildType >& allBuildTypes( BWAPI::Race race );
    std::set< BuildType >& allRequiredBuildTypes();
    std::set< BuildType >& allRequiredBuildTypes( BWAPI::Race race );

    void init();
    extern unsigned int WorkerMask;
    extern unsigned int RefineryMask;
    extern unsigned int SupplyMask;
    extern unsigned int CenterMask;
    extern const BuildType Terran_Marine;
    extern const BuildType Terran_Ghost;
    extern const BuildType Terran_Vulture;
    extern const BuildType Terran_Goliath;
    extern const BuildType Terran_Siege_Tank_Tank_Mode;
    extern const BuildType Terran_SCV;
    extern const BuildType Terran_Wraith;
    extern const BuildType Terran_Science_Vessel;
    extern const BuildType Terran_Dropship;
    extern const BuildType Terran_Battlecruiser;
    extern const BuildType Terran_Nuclear_Missile;
    extern const BuildType Terran_Siege_Tank_Siege_Mode;
    extern const BuildType Terran_Firebat;
    extern const BuildType Terran_Medic;
    extern const BuildType Zerg_Larva;
    extern const BuildType Zerg_Zergling;
    extern const BuildType Zerg_Hydralisk;
    extern const BuildType Zerg_Ultralisk;
    extern const BuildType Zerg_Drone;
    extern const BuildType Zerg_Overlord;
    extern const BuildType Zerg_Mutalisk;
    extern const BuildType Zerg_Guardian;
    extern const BuildType Zerg_Queen;
    extern const BuildType Zerg_Defiler;
    extern const BuildType Zerg_Scourge;
    extern const BuildType Zerg_Infested_Terran;
    extern const BuildType Terran_Valkyrie;
    extern const BuildType Zerg_Cocoon;
    extern const BuildType Protoss_Corsair;
    extern const BuildType Protoss_Dark_Templar;
    extern const BuildType Zerg_Devourer;
    extern const BuildType Protoss_Dark_Archon;
    extern const BuildType Protoss_Probe;
    extern const BuildType Protoss_Zealot;
    extern const BuildType Protoss_Dragoon;
    extern const BuildType Protoss_High_Templar;
    extern const BuildType Protoss_Archon;
    extern const BuildType Protoss_Shuttle;
    extern const BuildType Protoss_Scout;
    extern const BuildType Protoss_Arbiter;
    extern const BuildType Protoss_Carrier;
    extern const BuildType Protoss_Interceptor;
    extern const BuildType Protoss_Reaver;
    extern const BuildType Protoss_Observer;
    extern const BuildType Protoss_Scarab;
    extern const BuildType Zerg_Lurker;
    extern const BuildType Terran_Command_Center;
    extern const BuildType Terran_Comsat_Station;
    extern const BuildType Terran_Nuclear_Silo;
    extern const BuildType Terran_Supply_Depot;
    extern const BuildType Terran_Refinery;
    extern const BuildType Terran_Barracks;
    extern const BuildType Terran_Academy;
    extern const BuildType Terran_Factory;
    extern const BuildType Terran_Starport;
    extern const BuildType Terran_Control_Tower;
    extern const BuildType Terran_Science_Facility;
    extern const BuildType Terran_Covert_Ops;
    extern const BuildType Terran_Physics_Lab;
    extern const BuildType Terran_Machine_Shop;
    extern const BuildType Terran_Engineering_Bay;
    extern const BuildType Terran_Armory;
    extern const BuildType Terran_Missile_Turret;
    extern const BuildType Terran_Bunker;
    extern const BuildType Zerg_Hatchery;
    extern const BuildType Zerg_Lair;
    extern const BuildType Zerg_Hive;
    extern const BuildType Zerg_Nydus_Canal;
    extern const BuildType Zerg_Hydralisk_Den;
    extern const BuildType Zerg_Defiler_Mound;
    extern const BuildType Zerg_Greater_Spire;
    extern const BuildType Zerg_Queens_Nest;
    extern const BuildType Zerg_Evolution_Chamber;
    extern const BuildType Zerg_Ultralisk_Cavern;
    extern const BuildType Zerg_Spire;
    extern const BuildType Zerg_Spawning_Pool;
    extern const BuildType Zerg_Creep_Colony;
    extern const BuildType Zerg_Spore_Colony;
    extern const BuildType Zerg_Sunken_Colony;
    extern const BuildType Zerg_Extractor;
    extern const BuildType Protoss_Nexus;
    extern const BuildType Protoss_Robotics_Facility;
    extern const BuildType Protoss_Pylon;
    extern const BuildType Protoss_Assimilator;
    extern const BuildType Protoss_Observatory;
    extern const BuildType Protoss_Gateway;
    extern const BuildType Protoss_Photon_Cannon;
    extern const BuildType Protoss_Citadel_of_Adun;
    extern const BuildType Protoss_Cybernetics_Core;
    extern const BuildType Protoss_Templar_Archives;
    extern const BuildType Protoss_Forge;
    extern const BuildType Protoss_Stargate;
    extern const BuildType Protoss_Fleet_Beacon;
    extern const BuildType Protoss_Arbiter_Tribunal;
    extern const BuildType Protoss_Robotics_Support_Bay;
    extern const BuildType Protoss_Shield_Battery;
    extern const BuildType Stim_Packs;
    extern const BuildType Lockdown;
    extern const BuildType EMP_Shockwave;
    extern const BuildType Spider_Mines;
    extern const BuildType Tank_Siege_Mode;
    extern const BuildType Irradiate;
    extern const BuildType Yamato_Gun;
    extern const BuildType Cloaking_Field;
    extern const BuildType Personnel_Cloaking;
    extern const BuildType Burrowing;
    extern const BuildType Spawn_Broodlings;
    extern const BuildType Plague;
    extern const BuildType Consume;
    extern const BuildType Ensnare;
    extern const BuildType Psionic_Storm;
    extern const BuildType Hallucination;
    extern const BuildType Recall;
    extern const BuildType Stasis_Field;
    extern const BuildType Restoration;
    extern const BuildType Disruption_Web;
    extern const BuildType Mind_Control;
    extern const BuildType Optical_Flare;
    extern const BuildType Maelstrom;
    extern const BuildType Lurker_Aspect;
    extern const BuildType Terran_Infantry_Armor_1;
    extern const BuildType Terran_Infantry_Armor_2;
    extern const BuildType Terran_Infantry_Armor_3;
    extern const BuildType Terran_Vehicle_Plating_1;
    extern const BuildType Terran_Vehicle_Plating_2;
    extern const BuildType Terran_Vehicle_Plating_3;
    extern const BuildType Terran_Ship_Plating_1;
    extern const BuildType Terran_Ship_Plating_2;
    extern const BuildType Terran_Ship_Plating_3;
    extern const BuildType Zerg_Carapace_1;
    extern const BuildType Zerg_Carapace_2;
    extern const BuildType Zerg_Carapace_3;
    extern const BuildType Zerg_Flyer_Carapace_1;
    extern const BuildType Zerg_Flyer_Carapace_2;
    extern const BuildType Zerg_Flyer_Carapace_3;
    extern const BuildType Protoss_Ground_Armor_1;
    extern const BuildType Protoss_Ground_Armor_2;
    extern const BuildType Protoss_Ground_Armor_3;
    extern const BuildType Protoss_Air_Armor_1;
    extern const BuildType Protoss_Air_Armor_2;
    extern const BuildType Protoss_Air_Armor_3;
    extern const BuildType Terran_Infantry_Weapons_1;
    extern const BuildType Terran_Infantry_Weapons_2;
    extern const BuildType Terran_Infantry_Weapons_3;
    extern const BuildType Terran_Vehicle_Weapons_1;
    extern const BuildType Terran_Vehicle_Weapons_2;
    extern const BuildType Terran_Vehicle_Weapons_3;
    extern const BuildType Terran_Ship_Weapons_1;
    extern const BuildType Terran_Ship_Weapons_2;
    extern const BuildType Terran_Ship_Weapons_3;
    extern const BuildType Zerg_Melee_Attacks_1;
    extern const BuildType Zerg_Melee_Attacks_2;
    extern const BuildType Zerg_Melee_Attacks_3;
    extern const BuildType Zerg_Missile_Attacks_1;
    extern const BuildType Zerg_Missile_Attacks_2;
    extern const BuildType Zerg_Missile_Attacks_3;
    extern const BuildType Zerg_Flyer_Attacks_1;
    extern const BuildType Zerg_Flyer_Attacks_2;
    extern const BuildType Zerg_Flyer_Attacks_3;
    extern const BuildType Protoss_Ground_Weapons_1;
    extern const BuildType Protoss_Ground_Weapons_2;
    extern const BuildType Protoss_Ground_Weapons_3;
    extern const BuildType Protoss_Air_Weapons_1;
    extern const BuildType Protoss_Air_Weapons_2;
    extern const BuildType Protoss_Air_Weapons_3;
    extern const BuildType Protoss_Plasma_Shields_1;
    extern const BuildType Protoss_Plasma_Shields_2;
    extern const BuildType Protoss_Plasma_Shields_3;
    extern const BuildType U_238_Shells;
    extern const BuildType Ion_Thrusters;
    extern const BuildType Titan_Reactor;
    extern const BuildType Ocular_Implants;
    extern const BuildType Moebius_Reactor;
    extern const BuildType Apollo_Reactor;
    extern const BuildType Colossus_Reactor;
    extern const BuildType Ventral_Sacs;
    extern const BuildType Antennae;
    extern const BuildType Pneumatized_Carapace;
    extern const BuildType Metabolic_Boost;
    extern const BuildType Adrenal_Glands;
    extern const BuildType Muscular_Augments;
    extern const BuildType Grooved_Spines;
    extern const BuildType Gamete_Meiosis;
    extern const BuildType Metasynaptic_Node;
    extern const BuildType Singularity_Charge;
    extern const BuildType Leg_Enhancements;
    extern const BuildType Scarab_Damage;
    extern const BuildType Reaver_Capacity;
    extern const BuildType Gravitic_Drive;
    extern const BuildType Sensor_Array;
    extern const BuildType Gravitic_Boosters;
    extern const BuildType Khaydarin_Amulet;
    extern const BuildType Apial_Sensors;
    extern const BuildType Gravitic_Thrusters;
    extern const BuildType Carrier_Capacity;
    extern const BuildType Khaydarin_Core;
    extern const BuildType Argus_Jewel;
    extern const BuildType Argus_Talisman;
    extern const BuildType Caduceus_Reactor;
    extern const BuildType Chitinous_Plating;
    extern const BuildType Anabolic_Synthesis;
    extern const BuildType Charon_Boosters;
    extern const BuildType None;
  }
}