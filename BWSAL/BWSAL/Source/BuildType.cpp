#include <BWSAL/BuildType.h>
#include <BWSAL/Util.h>
#include <BWAPI.h>
#include <algorithm>
namespace BWSAL
{
  std::map< std::string, BuildType > buildTypeMap;
  std::set< BuildType > buildTypeSet;
  std::map< BWAPI::Race, std::set< BuildType > > buildTypeSetByRace;
  std::set< BuildType > requiredBuildTypeSet;
  std::map< BWAPI::Race, std::set< BuildType > > requiredBuildTypeSetByRace;
  std::map< BWAPI::TechType, BuildType > techTypeToBuildTypeMap;
  std::map< BWAPI::UnitType, BuildType > unitTypeToBuildTypeMap;
  std::map< BWAPI::UpgradeType, BuildType > upgradeTypeToBuildTypeMap;
  bool initializingBuildType = true;
  class BuildTypeInternal
  {
    public:
      BuildTypeInternal()
      {
        techType = BWAPI::TechTypes::None;
        unitType = BWAPI::UnitTypes::None;
        upgradeType = BWAPI::UpgradeTypes::None;
        upgradeLevel = 0;
        buildUnitTime = 0;
        prepTime = 0;
        createsUnit = false;
        morphsBuilder = false;
        needsBuildLocation = false;
        supplyRequired = 0;
        supplyProvided = 0;
        requiresPsi = false;
        requiresLarva = false;
        requiredAddon = BuildTypes::None;
        valid = false;
        mask = 0;
        requiredMask = 0;
      }
      void set( BWAPI::TechType type )
      {
        if ( initializingBuildType )
        {
          this->techType = type;
          this->name = type.getName();
          this->race = type.getRace();
          this->mineralPrice = type.mineralPrice();
          this->gasPrice = type.gasPrice();
          this->builderTime = this->buildUnitTime = type.researchTime();
          this->valid = true;
        }
      }
      void set( BWAPI::UnitType type )
      {
        if ( initializingBuildType )
        {
          this->unitType = type;
          this->name = type.getName();
          this->race = type.getRace();
          this->mineralPrice = type.mineralPrice();
          this->gasPrice = type.gasPrice();
          this->builderTime = type.buildTime() + 24;
          this->buildUnitTime = type.buildTime() + 24;
          if ( this->race == BWAPI::Races::Protoss && type.isBuilding() )
          {
            this->builderTime = 4*24; // protoss buildings only consume a small amount of worker time
            this->buildUnitTime += 4*24;
          }
          this->requiresPsi = type.requiresPsi();
          this->needsBuildLocation = ( type.isBuilding() && !type.whatBuilds().first.isBuilding() );
          this->prepTime = 0;
          if ( this->needsBuildLocation )
          {
            this->prepTime = 4*24; // Need 4 seconds to move to build location
          }

          if ( this->race == BWAPI::Races::Zerg )
          {
            this->morphsBuilder = true;
            this->createsUnit = type.isTwoUnitsInOneEgg();
            if ( type == BWAPI::UnitTypes::Zerg_Infested_Terran )
            {
              this->morphsBuilder = false;
              this->createsUnit = true;
            }
          }
          else
          {
            this->morphsBuilder = false;
            this->createsUnit = true;
            if ( type == BWAPI::UnitTypes::Protoss_Archon || type == BWAPI::UnitTypes::Protoss_Dark_Archon )
            {
              this->morphsBuilder = true;
              this->createsUnit = false;
            }
          }
          this->supplyRequired = type.supplyRequired();
          this->supplyProvided = type.supplyProvided();
          this->valid = true;
        }
      }
      void set( BWAPI::UpgradeType type, int level = 1 )
      {
        if ( initializingBuildType )
        {
          this->upgradeType = type;
          this->upgradeLevel = level;
          if ( type.maxRepeats() == 1 )
          {
            this->name = type.getName();
          }
          else
          {
            if ( level == 1 )
            {
              this->name = type.getName() + " 1";
            }
            else if ( level == 2 )
            {
              this->name = type.getName() + " 2";
            }
            else
            {
              this->name = type.getName() + " 3";
            }
          }
          this->race = type.getRace();
          this->mineralPrice = type.mineralPrice( level );
          this->gasPrice = type.gasPrice( level );
          this->builderTime = this->buildUnitTime = type.upgradeTime( level );
          this->valid = true;
        }
      }
      void setDependencies()
      {
        if ( initializingBuildType )
        {
          if ( this->techType != BWAPI::TechTypes::None )
          {
            this->whatBuilds.first = BuildType( this->techType.whatResearches() );
            this->whatBuilds.second = 1;
            this->requiredBuildTypes.insert( this->whatBuilds );
            if ( this->techType == BWAPI::TechTypes::Lurker_Aspect )
            {
              this->requiredBuildTypes.insert( std::make_pair( BuildTypes::Zerg_Lair, 1 ) );
            }
          }
          else if ( this->unitType != BWAPI::UnitTypes::None )
          {
            if ( this->unitType.whatBuilds().first == BWAPI::UnitTypes::Zerg_Larva )
            {
              this->whatBuilds.first = BuildTypes::Zerg_Hatchery;
              this->whatBuilds.second = 1;
            }
            else
            {
              this->whatBuilds = this->unitType.whatBuilds();
            }
            for ( std::map< BWAPI::UnitType, int >::const_iterator r = this->unitType.requiredUnits().begin(); r != this->unitType.requiredUnits().end(); r++ )
            {
              if ( r->first == BWAPI::UnitTypes::Zerg_Larva )
              {
                this->requiresLarva = true;
                this->requiredBuildTypes.insert( std::make_pair( BuildTypes::Zerg_Hatchery, r->second ) );
              }
              else
              {
                if ( r->first.isAddon() && r->first.whatBuilds().first == this->unitType.whatBuilds().first )
                {
                  this->requiredAddon = BuildType( r->first );
                }
                this->requiredBuildTypes.insert( std::make_pair( BuildType( r->first ), r->second ) );
              }
            }
            if ( this->unitType.requiredTech() != BWAPI::TechTypes::None )
            {
              this->requiredBuildTypes.insert( std::make_pair( BuildType( this->unitType.requiredTech() ), 1 ) );
            }
            if ( this->unitType.requiresPsi() )
            {
              this->requiredBuildTypes.insert( std::make_pair( BuildTypes::Protoss_Pylon, 1 ) );
            }
          }
          else if ( this->upgradeType != BWAPI::UpgradeTypes::None )
          {
            this->whatBuilds.first = BuildType( this->upgradeType.whatUpgrades() );
            this->whatBuilds.second = 1;
            this->requiredBuildTypes.insert( this->whatBuilds );
            this->requiredBuildTypes.insert( std::make_pair( BuildType( this->upgradeType.whatsRequired() ), 1) );
            if ( this->upgradeLevel > 1 )
            {
              this->requiredBuildTypes.insert( std::make_pair( BuildType( this->upgradeType, this->upgradeLevel - 1 ), 1 ) );
            }
          }
        }
      }
      BWAPI::TechType techType;
      BWAPI::UnitType unitType;
      BWAPI::UpgradeType upgradeType;
      int upgradeLevel;

      std::string name;
      BWAPI::Race race;

      std::pair< BuildType, int > whatBuilds;
      std::map< BuildType, int >  requiredBuildTypes;

      bool requiresPsi;
      bool requiresLarva;
      BuildType requiredAddon;
      int mineralPrice;
      int gasPrice;
      int executionTime;
      int builderTime;
      int buildUnitTime;
      int prepTime;
      bool createsUnit;
      bool morphsBuilder;
      bool needsBuildLocation;

      int supplyRequired;
      int supplyProvided;
      unsigned int mask;
      unsigned int requiredMask;

      bool valid;
  };
  BuildTypeInternal buildTypeData[203];
  namespace BuildTypes
  {
    unsigned int WorkerMask   = 1 << 0;
    unsigned int RefineryMask = 1 << 1;
    unsigned int SupplyMask   = 1 << 2;
    unsigned int CenterMask   = 1 << 3;
    const BuildType Terran_Marine( 0 );
    const BuildType Terran_Ghost( 1 );
    const BuildType Terran_Vulture( 2 );
    const BuildType Terran_Goliath( 3 );
    const BuildType Terran_Siege_Tank_Tank_Mode( 4 );
    const BuildType Terran_SCV( 5 );
    const BuildType Terran_Wraith( 6 );
    const BuildType Terran_Science_Vessel( 7 );
    const BuildType Terran_Dropship( 8 );
    const BuildType Terran_Battlecruiser( 9 );
    const BuildType Terran_Nuclear_Missile( 10 );
    const BuildType Terran_Siege_Tank_Siege_Mode( 11 );
    const BuildType Terran_Firebat( 12 );
    const BuildType Terran_Medic( 13 );
    const BuildType Zerg_Larva( 14 );
    const BuildType Zerg_Zergling( 15 );
    const BuildType Zerg_Hydralisk( 16 );
    const BuildType Zerg_Ultralisk( 17 );
    const BuildType Zerg_Drone( 18 );
    const BuildType Zerg_Overlord( 19 );
    const BuildType Zerg_Mutalisk( 20 );
    const BuildType Zerg_Guardian( 21 );
    const BuildType Zerg_Queen( 22 );
    const BuildType Zerg_Defiler( 23 );
    const BuildType Zerg_Scourge( 24 );
    const BuildType Zerg_Infested_Terran( 25 );
    const BuildType Terran_Valkyrie( 26 );
    const BuildType Zerg_Cocoon( 27 );
    const BuildType Protoss_Corsair( 28 );
    const BuildType Protoss_Dark_Templar( 29 );
    const BuildType Zerg_Devourer( 30 );
    const BuildType Protoss_Dark_Archon( 31 );
    const BuildType Protoss_Probe( 32 );
    const BuildType Protoss_Zealot( 33 );
    const BuildType Protoss_Dragoon( 34 );
    const BuildType Protoss_High_Templar( 35 );
    const BuildType Protoss_Archon( 36 );
    const BuildType Protoss_Shuttle( 37 );
    const BuildType Protoss_Scout( 38 );
    const BuildType Protoss_Arbiter( 39 );
    const BuildType Protoss_Carrier( 40 );
    const BuildType Protoss_Interceptor( 41 );
    const BuildType Protoss_Reaver( 42 );
    const BuildType Protoss_Observer( 43 );
    const BuildType Protoss_Scarab( 44 );
    const BuildType Zerg_Lurker( 45 );
    const BuildType Terran_Command_Center( 46 );
    const BuildType Terran_Comsat_Station( 47 );
    const BuildType Terran_Nuclear_Silo( 48 );
    const BuildType Terran_Supply_Depot( 49 );
    const BuildType Terran_Refinery( 50 );
    const BuildType Terran_Barracks( 51 );
    const BuildType Terran_Academy( 52 );
    const BuildType Terran_Factory( 53 );
    const BuildType Terran_Starport( 54 );
    const BuildType Terran_Control_Tower( 55 );
    const BuildType Terran_Science_Facility( 56 );
    const BuildType Terran_Covert_Ops( 57 );
    const BuildType Terran_Physics_Lab( 58 );
    const BuildType Terran_Machine_Shop( 59 );
    const BuildType Terran_Engineering_Bay( 60 );
    const BuildType Terran_Armory( 61 );
    const BuildType Terran_Missile_Turret( 62 );
    const BuildType Terran_Bunker( 63 );
    const BuildType Zerg_Hatchery( 64 );
    const BuildType Zerg_Lair( 65 );
    const BuildType Zerg_Hive( 66 );
    const BuildType Zerg_Nydus_Canal( 67 );
    const BuildType Zerg_Hydralisk_Den( 68 );
    const BuildType Zerg_Defiler_Mound( 69 );
    const BuildType Zerg_Greater_Spire( 70 );
    const BuildType Zerg_Queens_Nest( 71 );
    const BuildType Zerg_Evolution_Chamber( 72 );
    const BuildType Zerg_Ultralisk_Cavern( 73 );
    const BuildType Zerg_Spire( 74 );
    const BuildType Zerg_Spawning_Pool( 75 );
    const BuildType Zerg_Creep_Colony( 76 );
    const BuildType Zerg_Spore_Colony( 77 );
    const BuildType Zerg_Sunken_Colony( 78 );
    const BuildType Zerg_Extractor( 79 );
    const BuildType Protoss_Nexus( 80 );
    const BuildType Protoss_Robotics_Facility( 81 );
    const BuildType Protoss_Pylon( 82 );
    const BuildType Protoss_Assimilator( 83 );
    const BuildType Protoss_Observatory( 84 );
    const BuildType Protoss_Gateway( 85 );
    const BuildType Protoss_Photon_Cannon( 86 );
    const BuildType Protoss_Citadel_of_Adun( 87 );
    const BuildType Protoss_Cybernetics_Core( 88 );
    const BuildType Protoss_Templar_Archives( 89 );
    const BuildType Protoss_Forge( 90 );
    const BuildType Protoss_Stargate( 91 );
    const BuildType Protoss_Fleet_Beacon( 92 );
    const BuildType Protoss_Arbiter_Tribunal( 93 );
    const BuildType Protoss_Robotics_Support_Bay( 94 );
    const BuildType Protoss_Shield_Battery( 95 );
    const BuildType Stim_Packs( 96 );
    const BuildType Lockdown( 97 );
    const BuildType EMP_Shockwave( 98 );
    const BuildType Spider_Mines( 99 );
    const BuildType Tank_Siege_Mode( 100 );
    const BuildType Irradiate( 101 );
    const BuildType Yamato_Gun( 102 );
    const BuildType Cloaking_Field( 103 );
    const BuildType Personnel_Cloaking( 104 );
    const BuildType Burrowing( 105 );
    const BuildType Spawn_Broodlings( 106 );
    const BuildType Plague( 107 );
    const BuildType Consume( 108 );
    const BuildType Ensnare( 109 );
    const BuildType Psionic_Storm( 110 );
    const BuildType Hallucination( 111 );
    const BuildType Recall( 112 );
    const BuildType Stasis_Field( 113 );
    const BuildType Restoration( 114 );
    const BuildType Disruption_Web( 115 );
    const BuildType Mind_Control( 116 );
    const BuildType Optical_Flare( 117 );
    const BuildType Maelstrom( 118 );
    const BuildType Lurker_Aspect( 119 );
    const BuildType Terran_Infantry_Armor_1( 120 );
    const BuildType Terran_Infantry_Armor_2( 121 );
    const BuildType Terran_Infantry_Armor_3( 122 );
    const BuildType Terran_Vehicle_Plating_1( 123 );
    const BuildType Terran_Vehicle_Plating_2( 124 );
    const BuildType Terran_Vehicle_Plating_3( 125 );
    const BuildType Terran_Ship_Plating_1( 126 );
    const BuildType Terran_Ship_Plating_2( 127 );
    const BuildType Terran_Ship_Plating_3( 128 );
    const BuildType Zerg_Carapace_1( 129 );
    const BuildType Zerg_Carapace_2( 130 );
    const BuildType Zerg_Carapace_3( 131 );
    const BuildType Zerg_Flyer_Carapace_1( 132 );
    const BuildType Zerg_Flyer_Carapace_2( 133 );
    const BuildType Zerg_Flyer_Carapace_3( 134 );
    const BuildType Protoss_Ground_Armor_1( 135 );
    const BuildType Protoss_Ground_Armor_2( 136 );
    const BuildType Protoss_Ground_Armor_3( 137 );
    const BuildType Protoss_Air_Armor_1( 138 );
    const BuildType Protoss_Air_Armor_2( 139 );
    const BuildType Protoss_Air_Armor_3( 140 );
    const BuildType Terran_Infantry_Weapons_1( 141 );
    const BuildType Terran_Infantry_Weapons_2( 142 );
    const BuildType Terran_Infantry_Weapons_3( 143 );
    const BuildType Terran_Vehicle_Weapons_1( 144 );
    const BuildType Terran_Vehicle_Weapons_2( 145 );
    const BuildType Terran_Vehicle_Weapons_3( 146 );
    const BuildType Terran_Ship_Weapons_1( 147 );
    const BuildType Terran_Ship_Weapons_2( 148 );
    const BuildType Terran_Ship_Weapons_3( 149 );
    const BuildType Zerg_Melee_Attacks_1( 150 );
    const BuildType Zerg_Melee_Attacks_2( 151 );
    const BuildType Zerg_Melee_Attacks_3( 152 );
    const BuildType Zerg_Missile_Attacks_1( 153 );
    const BuildType Zerg_Missile_Attacks_2( 154 );
    const BuildType Zerg_Missile_Attacks_3( 155 );
    const BuildType Zerg_Flyer_Attacks_1( 156 );
    const BuildType Zerg_Flyer_Attacks_2( 157 );
    const BuildType Zerg_Flyer_Attacks_3( 158 );
    const BuildType Protoss_Ground_Weapons_1( 159 );
    const BuildType Protoss_Ground_Weapons_2( 160 );
    const BuildType Protoss_Ground_Weapons_3( 161 );
    const BuildType Protoss_Air_Weapons_1( 162 );
    const BuildType Protoss_Air_Weapons_2( 163 );
    const BuildType Protoss_Air_Weapons_3( 164 );
    const BuildType Protoss_Plasma_Shields_1( 165 );
    const BuildType Protoss_Plasma_Shields_2( 166 );
    const BuildType Protoss_Plasma_Shields_3( 167 );
    const BuildType U_238_Shells( 168 );
    const BuildType Ion_Thrusters( 169 );
    const BuildType Titan_Reactor( 170 );
    const BuildType Ocular_Implants( 171 );
    const BuildType Moebius_Reactor( 172 );
    const BuildType Apollo_Reactor( 173 );
    const BuildType Colossus_Reactor( 174 );
    const BuildType Ventral_Sacs( 175 );
    const BuildType Antennae( 176 );
    const BuildType Pneumatized_Carapace( 177 );
    const BuildType Metabolic_Boost( 178 );
    const BuildType Adrenal_Glands( 179 );
    const BuildType Muscular_Augments( 180 );
    const BuildType Grooved_Spines( 181 );
    const BuildType Gamete_Meiosis( 182 );
    const BuildType Metasynaptic_Node( 183 );
    const BuildType Singularity_Charge( 184 );
    const BuildType Leg_Enhancements( 185 );
    const BuildType Scarab_Damage( 186 );
    const BuildType Reaver_Capacity( 187 );
    const BuildType Gravitic_Drive( 188 );
    const BuildType Sensor_Array( 189 );
    const BuildType Gravitic_Boosters( 190 );
    const BuildType Khaydarin_Amulet( 191 );
    const BuildType Apial_Sensors( 192 );
    const BuildType Gravitic_Thrusters( 193 );
    const BuildType Carrier_Capacity( 194 );
    const BuildType Khaydarin_Core( 195 );
    const BuildType Argus_Jewel( 196 );
    const BuildType Argus_Talisman( 197 );
    const BuildType Caduceus_Reactor( 198 );
    const BuildType Chitinous_Plating( 199 );
    const BuildType Anabolic_Synthesis( 200 );
    const BuildType Charon_Boosters( 201 );
    const BuildType None( 202 );
    void set( BuildType buildType, BWAPI::TechType type )
    {
       buildTypeSet.insert( buildType );
       buildTypeData[buildType.getID()].set( type );
       techTypeToBuildTypeMap[type] = buildType;
    }
    void set( BuildType buildType, BWAPI::UnitType type )
    {
       buildTypeSet.insert( buildType );
       buildTypeData[buildType.getID()].set( type );
       unitTypeToBuildTypeMap[type] = buildType;
    }
    void set( BuildType buildType, BWAPI::UpgradeType type, int level = 1 )
    {
       buildTypeSet.insert( buildType );
       buildTypeData[buildType.getID()].set( type, level );
       if ( level == 1 )
       {
         upgradeTypeToBuildTypeMap[type] = buildType;
       }
    }
    void init()
    {
      set( Terran_Marine, BWAPI::UnitTypes::Terran_Marine );
      set( Terran_Ghost, BWAPI::UnitTypes::Terran_Ghost );
      set( Terran_Vulture, BWAPI::UnitTypes::Terran_Vulture );
      set( Terran_Goliath, BWAPI::UnitTypes::Terran_Goliath );
      set( Terran_Siege_Tank_Tank_Mode, BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode );
      set( Terran_SCV, BWAPI::UnitTypes::Terran_SCV );
      set( Terran_Wraith, BWAPI::UnitTypes::Terran_Wraith );
      set( Terran_Science_Vessel, BWAPI::UnitTypes::Terran_Science_Vessel );
      set( Terran_Dropship, BWAPI::UnitTypes::Terran_Dropship );
      set( Terran_Battlecruiser, BWAPI::UnitTypes::Terran_Battlecruiser );
      set( Terran_Nuclear_Missile, BWAPI::UnitTypes::Terran_Nuclear_Missile );
      set( Terran_Siege_Tank_Siege_Mode, BWAPI::UnitTypes::Terran_Siege_Tank_Siege_Mode );
      set( Terran_Firebat, BWAPI::UnitTypes::Terran_Firebat );
      set( Terran_Medic, BWAPI::UnitTypes::Terran_Medic );
      set( Zerg_Larva, BWAPI::UnitTypes::Zerg_Larva );
      set( Zerg_Zergling, BWAPI::UnitTypes::Zerg_Zergling );
      set( Zerg_Hydralisk, BWAPI::UnitTypes::Zerg_Hydralisk );
      set( Zerg_Ultralisk, BWAPI::UnitTypes::Zerg_Ultralisk );
      set( Zerg_Drone, BWAPI::UnitTypes::Zerg_Drone );
      set( Zerg_Overlord, BWAPI::UnitTypes::Zerg_Overlord );
      set( Zerg_Mutalisk, BWAPI::UnitTypes::Zerg_Mutalisk );
      set( Zerg_Guardian, BWAPI::UnitTypes::Zerg_Guardian );
      set( Zerg_Queen, BWAPI::UnitTypes::Zerg_Queen );
      set( Zerg_Defiler, BWAPI::UnitTypes::Zerg_Defiler );
      set( Zerg_Scourge, BWAPI::UnitTypes::Zerg_Scourge );
      set( Zerg_Infested_Terran, BWAPI::UnitTypes::Zerg_Infested_Terran );
      set( Terran_Valkyrie, BWAPI::UnitTypes::Terran_Valkyrie );
      set( Zerg_Cocoon, BWAPI::UnitTypes::Zerg_Cocoon );
      set( Protoss_Corsair, BWAPI::UnitTypes::Protoss_Corsair );
      set( Protoss_Dark_Templar, BWAPI::UnitTypes::Protoss_Dark_Templar );
      set( Zerg_Devourer, BWAPI::UnitTypes::Zerg_Devourer );
      set( Protoss_Dark_Archon, BWAPI::UnitTypes::Protoss_Dark_Archon );
      set( Protoss_Probe, BWAPI::UnitTypes::Protoss_Probe );
      set( Protoss_Zealot, BWAPI::UnitTypes::Protoss_Zealot );
      set( Protoss_Dragoon, BWAPI::UnitTypes::Protoss_Dragoon );
      set( Protoss_High_Templar, BWAPI::UnitTypes::Protoss_High_Templar );
      set( Protoss_Archon, BWAPI::UnitTypes::Protoss_Archon );
      set( Protoss_Shuttle, BWAPI::UnitTypes::Protoss_Shuttle );
      set( Protoss_Scout, BWAPI::UnitTypes::Protoss_Scout );
      set( Protoss_Arbiter, BWAPI::UnitTypes::Protoss_Arbiter );
      set( Protoss_Carrier, BWAPI::UnitTypes::Protoss_Carrier );
      set( Protoss_Interceptor, BWAPI::UnitTypes::Protoss_Interceptor );
      set( Protoss_Reaver, BWAPI::UnitTypes::Protoss_Reaver );
      set( Protoss_Observer, BWAPI::UnitTypes::Protoss_Observer );
      set( Protoss_Scarab, BWAPI::UnitTypes::Protoss_Scarab );
      set( Zerg_Lurker, BWAPI::UnitTypes::Zerg_Lurker );
      set( Terran_Command_Center, BWAPI::UnitTypes::Terran_Command_Center );
      set( Terran_Comsat_Station, BWAPI::UnitTypes::Terran_Comsat_Station );
      set( Terran_Nuclear_Silo, BWAPI::UnitTypes::Terran_Nuclear_Silo );
      set( Terran_Supply_Depot, BWAPI::UnitTypes::Terran_Supply_Depot );
      set( Terran_Refinery, BWAPI::UnitTypes::Terran_Refinery );
      set( Terran_Barracks, BWAPI::UnitTypes::Terran_Barracks );
      set( Terran_Academy, BWAPI::UnitTypes::Terran_Academy );
      set( Terran_Factory, BWAPI::UnitTypes::Terran_Factory );
      set( Terran_Starport, BWAPI::UnitTypes::Terran_Starport );
      set( Terran_Control_Tower, BWAPI::UnitTypes::Terran_Control_Tower );
      set( Terran_Science_Facility, BWAPI::UnitTypes::Terran_Science_Facility );
      set( Terran_Covert_Ops, BWAPI::UnitTypes::Terran_Covert_Ops );
      set( Terran_Physics_Lab, BWAPI::UnitTypes::Terran_Physics_Lab );
      set( Terran_Machine_Shop, BWAPI::UnitTypes::Terran_Machine_Shop );
      set( Terran_Engineering_Bay, BWAPI::UnitTypes::Terran_Engineering_Bay );
      set( Terran_Armory, BWAPI::UnitTypes::Terran_Armory );
      set( Terran_Missile_Turret, BWAPI::UnitTypes::Terran_Missile_Turret );
      set( Terran_Bunker, BWAPI::UnitTypes::Terran_Bunker );
      set( Zerg_Hatchery, BWAPI::UnitTypes::Zerg_Hatchery );
      set( Zerg_Lair, BWAPI::UnitTypes::Zerg_Lair );
      set( Zerg_Hive, BWAPI::UnitTypes::Zerg_Hive );
      set( Zerg_Nydus_Canal, BWAPI::UnitTypes::Zerg_Nydus_Canal );
      set( Zerg_Hydralisk_Den, BWAPI::UnitTypes::Zerg_Hydralisk_Den );
      set( Zerg_Defiler_Mound, BWAPI::UnitTypes::Zerg_Defiler_Mound );
      set( Zerg_Greater_Spire, BWAPI::UnitTypes::Zerg_Greater_Spire );
      set( Zerg_Queens_Nest, BWAPI::UnitTypes::Zerg_Queens_Nest );
      set( Zerg_Evolution_Chamber, BWAPI::UnitTypes::Zerg_Evolution_Chamber );
      set( Zerg_Ultralisk_Cavern, BWAPI::UnitTypes::Zerg_Ultralisk_Cavern );
      set( Zerg_Spire, BWAPI::UnitTypes::Zerg_Spire );
      set( Zerg_Spawning_Pool, BWAPI::UnitTypes::Zerg_Spawning_Pool );
      set( Zerg_Creep_Colony, BWAPI::UnitTypes::Zerg_Creep_Colony );
      set( Zerg_Spore_Colony, BWAPI::UnitTypes::Zerg_Spore_Colony );
      set( Zerg_Sunken_Colony, BWAPI::UnitTypes::Zerg_Sunken_Colony );
      set( Zerg_Extractor, BWAPI::UnitTypes::Zerg_Extractor );
      set( Protoss_Nexus, BWAPI::UnitTypes::Protoss_Nexus );
      set( Protoss_Robotics_Facility, BWAPI::UnitTypes::Protoss_Robotics_Facility );
      set( Protoss_Pylon, BWAPI::UnitTypes::Protoss_Pylon );
      set( Protoss_Assimilator, BWAPI::UnitTypes::Protoss_Assimilator );
      set( Protoss_Observatory, BWAPI::UnitTypes::Protoss_Observatory );
      set( Protoss_Gateway, BWAPI::UnitTypes::Protoss_Gateway );
      set( Protoss_Photon_Cannon, BWAPI::UnitTypes::Protoss_Photon_Cannon );
      set( Protoss_Citadel_of_Adun, BWAPI::UnitTypes::Protoss_Citadel_of_Adun );
      set( Protoss_Cybernetics_Core, BWAPI::UnitTypes::Protoss_Cybernetics_Core );
      set( Protoss_Templar_Archives, BWAPI::UnitTypes::Protoss_Templar_Archives );
      set( Protoss_Forge, BWAPI::UnitTypes::Protoss_Forge );
      set( Protoss_Stargate, BWAPI::UnitTypes::Protoss_Stargate );
      set( Protoss_Fleet_Beacon, BWAPI::UnitTypes::Protoss_Fleet_Beacon );
      set( Protoss_Arbiter_Tribunal, BWAPI::UnitTypes::Protoss_Arbiter_Tribunal );
      set( Protoss_Robotics_Support_Bay, BWAPI::UnitTypes::Protoss_Robotics_Support_Bay );
      set( Protoss_Shield_Battery, BWAPI::UnitTypes::Protoss_Shield_Battery );
      set( Stim_Packs, BWAPI::TechTypes::Stim_Packs );
      set( Lockdown, BWAPI::TechTypes::Lockdown );
      set( EMP_Shockwave, BWAPI::TechTypes::EMP_Shockwave );
      set( Spider_Mines, BWAPI::TechTypes::Spider_Mines );
      set( Tank_Siege_Mode, BWAPI::TechTypes::Tank_Siege_Mode );
      set( Irradiate, BWAPI::TechTypes::Irradiate );
      set( Yamato_Gun, BWAPI::TechTypes::Yamato_Gun );
      set( Cloaking_Field, BWAPI::TechTypes::Cloaking_Field );
      set( Personnel_Cloaking, BWAPI::TechTypes::Personnel_Cloaking );
      set( Burrowing, BWAPI::TechTypes::Burrowing );
      set( Spawn_Broodlings, BWAPI::TechTypes::Spawn_Broodlings );
      set( Plague, BWAPI::TechTypes::Plague );
      set( Consume, BWAPI::TechTypes::Consume );
      set( Ensnare, BWAPI::TechTypes::Ensnare );
      set( Psionic_Storm, BWAPI::TechTypes::Psionic_Storm );
      set( Hallucination, BWAPI::TechTypes::Hallucination );
      set( Recall, BWAPI::TechTypes::Recall );
      set( Stasis_Field, BWAPI::TechTypes::Stasis_Field );
      set( Restoration, BWAPI::TechTypes::Restoration );
      set( Disruption_Web, BWAPI::TechTypes::Disruption_Web );
      set( Mind_Control, BWAPI::TechTypes::Mind_Control );
      set( Optical_Flare, BWAPI::TechTypes::Optical_Flare );
      set( Maelstrom, BWAPI::TechTypes::Maelstrom );
      set( Lurker_Aspect, BWAPI::TechTypes::Lurker_Aspect );
      set( Terran_Infantry_Armor_1, BWAPI::UpgradeTypes::Terran_Infantry_Armor, 1 );
      set( Terran_Infantry_Armor_2, BWAPI::UpgradeTypes::Terran_Infantry_Armor, 2 );
      set( Terran_Infantry_Armor_3, BWAPI::UpgradeTypes::Terran_Infantry_Armor, 3 );
      set( Terran_Vehicle_Plating_1, BWAPI::UpgradeTypes::Terran_Vehicle_Plating, 1 );
      set( Terran_Vehicle_Plating_2, BWAPI::UpgradeTypes::Terran_Vehicle_Plating, 2 );
      set( Terran_Vehicle_Plating_3, BWAPI::UpgradeTypes::Terran_Vehicle_Plating, 3 );
      set( Terran_Ship_Plating_1, BWAPI::UpgradeTypes::Terran_Ship_Plating, 1 );
      set( Terran_Ship_Plating_2, BWAPI::UpgradeTypes::Terran_Ship_Plating, 2 );
      set( Terran_Ship_Plating_3, BWAPI::UpgradeTypes::Terran_Ship_Plating, 3 );
      set( Zerg_Carapace_1, BWAPI::UpgradeTypes::Zerg_Carapace, 1 );
      set( Zerg_Carapace_2, BWAPI::UpgradeTypes::Zerg_Carapace, 2 );
      set( Zerg_Carapace_3, BWAPI::UpgradeTypes::Zerg_Carapace, 3 );
      set( Zerg_Flyer_Carapace_1, BWAPI::UpgradeTypes::Zerg_Flyer_Carapace, 1 );
      set( Zerg_Flyer_Carapace_2, BWAPI::UpgradeTypes::Zerg_Flyer_Carapace, 2 );
      set( Zerg_Flyer_Carapace_3, BWAPI::UpgradeTypes::Zerg_Flyer_Carapace, 3 );
      set( Protoss_Ground_Armor_1, BWAPI::UpgradeTypes::Protoss_Ground_Armor, 1 );
      set( Protoss_Ground_Armor_2, BWAPI::UpgradeTypes::Protoss_Ground_Armor, 2 );
      set( Protoss_Ground_Armor_3, BWAPI::UpgradeTypes::Protoss_Ground_Armor, 3 );
      set( Protoss_Air_Armor_1, BWAPI::UpgradeTypes::Protoss_Air_Armor, 1 );
      set( Protoss_Air_Armor_2, BWAPI::UpgradeTypes::Protoss_Air_Armor, 2 );
      set( Protoss_Air_Armor_3, BWAPI::UpgradeTypes::Protoss_Air_Armor, 3 );
      set( Terran_Infantry_Weapons_1, BWAPI::UpgradeTypes::Terran_Infantry_Weapons, 1 );
      set( Terran_Infantry_Weapons_2, BWAPI::UpgradeTypes::Terran_Infantry_Weapons, 2 );
      set( Terran_Infantry_Weapons_3, BWAPI::UpgradeTypes::Terran_Infantry_Weapons, 3 );
      set( Terran_Vehicle_Weapons_1, BWAPI::UpgradeTypes::Terran_Vehicle_Weapons, 1 );
      set( Terran_Vehicle_Weapons_2, BWAPI::UpgradeTypes::Terran_Vehicle_Weapons, 2 );
      set( Terran_Vehicle_Weapons_3, BWAPI::UpgradeTypes::Terran_Vehicle_Weapons, 3 );
      set( Terran_Ship_Weapons_1, BWAPI::UpgradeTypes::Terran_Ship_Weapons, 1 );
      set( Terran_Ship_Weapons_2, BWAPI::UpgradeTypes::Terran_Ship_Weapons, 2 );
      set( Terran_Ship_Weapons_3, BWAPI::UpgradeTypes::Terran_Ship_Weapons, 3 );
      set( Zerg_Melee_Attacks_1, BWAPI::UpgradeTypes::Zerg_Melee_Attacks, 1 );
      set( Zerg_Melee_Attacks_2, BWAPI::UpgradeTypes::Zerg_Melee_Attacks, 2 );
      set( Zerg_Melee_Attacks_3, BWAPI::UpgradeTypes::Zerg_Melee_Attacks, 3 );
      set( Zerg_Missile_Attacks_1, BWAPI::UpgradeTypes::Zerg_Missile_Attacks, 1 );
      set( Zerg_Missile_Attacks_2, BWAPI::UpgradeTypes::Zerg_Missile_Attacks, 2 );
      set( Zerg_Missile_Attacks_3, BWAPI::UpgradeTypes::Zerg_Missile_Attacks, 3 );
      set( Zerg_Flyer_Attacks_1, BWAPI::UpgradeTypes::Zerg_Flyer_Attacks, 1 );
      set( Zerg_Flyer_Attacks_2, BWAPI::UpgradeTypes::Zerg_Flyer_Attacks, 2 );
      set( Zerg_Flyer_Attacks_3, BWAPI::UpgradeTypes::Zerg_Flyer_Attacks, 3 );
      set( Protoss_Ground_Weapons_1, BWAPI::UpgradeTypes::Protoss_Ground_Weapons, 1 );
      set( Protoss_Ground_Weapons_2, BWAPI::UpgradeTypes::Protoss_Ground_Weapons, 2 );
      set( Protoss_Ground_Weapons_3, BWAPI::UpgradeTypes::Protoss_Ground_Weapons, 3 );
      set( Protoss_Air_Weapons_1, BWAPI::UpgradeTypes::Protoss_Air_Weapons, 1 );
      set( Protoss_Air_Weapons_2, BWAPI::UpgradeTypes::Protoss_Air_Weapons, 2 );
      set( Protoss_Air_Weapons_3, BWAPI::UpgradeTypes::Protoss_Air_Weapons, 3 );
      set( Protoss_Plasma_Shields_1, BWAPI::UpgradeTypes::Protoss_Plasma_Shields, 1 );
      set( Protoss_Plasma_Shields_2, BWAPI::UpgradeTypes::Protoss_Plasma_Shields, 2 );
      set( Protoss_Plasma_Shields_3, BWAPI::UpgradeTypes::Protoss_Plasma_Shields, 3 );
      set( U_238_Shells, BWAPI::UpgradeTypes::U_238_Shells );
      set( Ion_Thrusters, BWAPI::UpgradeTypes::Ion_Thrusters );
      set( Titan_Reactor, BWAPI::UpgradeTypes::Titan_Reactor );
      set( Ocular_Implants, BWAPI::UpgradeTypes::Ocular_Implants );
      set( Moebius_Reactor, BWAPI::UpgradeTypes::Moebius_Reactor );
      set( Apollo_Reactor, BWAPI::UpgradeTypes::Apollo_Reactor );
      set( Colossus_Reactor, BWAPI::UpgradeTypes::Colossus_Reactor );
      set( Ventral_Sacs, BWAPI::UpgradeTypes::Ventral_Sacs );
      set( Antennae, BWAPI::UpgradeTypes::Antennae );
      set( Pneumatized_Carapace, BWAPI::UpgradeTypes::Pneumatized_Carapace );
      set( Metabolic_Boost, BWAPI::UpgradeTypes::Metabolic_Boost );
      set( Adrenal_Glands, BWAPI::UpgradeTypes::Adrenal_Glands );
      set( Muscular_Augments, BWAPI::UpgradeTypes::Muscular_Augments );
      set( Grooved_Spines, BWAPI::UpgradeTypes::Grooved_Spines );
      set( Gamete_Meiosis, BWAPI::UpgradeTypes::Gamete_Meiosis );
      set( Metasynaptic_Node, BWAPI::UpgradeTypes::Metasynaptic_Node );
      set( Singularity_Charge, BWAPI::UpgradeTypes::Singularity_Charge );
      set( Leg_Enhancements, BWAPI::UpgradeTypes::Leg_Enhancements );
      set( Scarab_Damage, BWAPI::UpgradeTypes::Scarab_Damage );
      set( Reaver_Capacity, BWAPI::UpgradeTypes::Reaver_Capacity );
      set( Gravitic_Drive, BWAPI::UpgradeTypes::Gravitic_Drive );
      set( Sensor_Array, BWAPI::UpgradeTypes::Sensor_Array );
      set( Gravitic_Boosters, BWAPI::UpgradeTypes::Gravitic_Boosters );
      set( Khaydarin_Amulet, BWAPI::UpgradeTypes::Khaydarin_Amulet );
      set( Apial_Sensors, BWAPI::UpgradeTypes::Apial_Sensors );
      set( Gravitic_Thrusters, BWAPI::UpgradeTypes::Gravitic_Thrusters );
      set( Carrier_Capacity, BWAPI::UpgradeTypes::Carrier_Capacity );
      set( Khaydarin_Core, BWAPI::UpgradeTypes::Khaydarin_Core );
      set( Argus_Jewel, BWAPI::UpgradeTypes::Argus_Jewel );
      set( Argus_Talisman, BWAPI::UpgradeTypes::Argus_Talisman );
      set( Caduceus_Reactor, BWAPI::UpgradeTypes::Caduceus_Reactor );
      set( Chitinous_Plating, BWAPI::UpgradeTypes::Chitinous_Plating );
      set( Anabolic_Synthesis, BWAPI::UpgradeTypes::Anabolic_Synthesis );
      set( Charon_Boosters, BWAPI::UpgradeTypes::Charon_Boosters );
      set( None, BWAPI::UnitTypes::None );
      for( BuildType i : buildTypeSet )
      {
        if ( i != BuildTypes::None )
        {
          buildTypeSetByRace[i.getRace()].insert( i );
        }
        buildTypeData[i.getID()].setDependencies();
        std::string name = i.getName();
        fixName( name );
        buildTypeMap.insert( std::make_pair( name, i ) );
      }

      // Contstruct required build type set
      for( BuildType i : buildTypeSet )
      {
        if ( i != BuildTypes::None )
        {
          for ( std::map< BuildType, int >::const_iterator j = i.requiredBuildTypes().begin(); j != i.requiredBuildTypes().end(); j++ )
          {
            requiredBuildTypeSet.insert( j->first );
          }
        }
      }
      std::set< BWAPI::Race > races;
      races.insert( BWAPI::Races::Terran );
      races.insert( BWAPI::Races::Protoss );
      races.insert( BWAPI::Races::Zerg );

      // Give workers and refineries of different races the same bit masks
      for( BWAPI::Race r : races )
      {
        requiredBuildTypeSet.insert( BuildType( r.getWorker() ) );
        requiredBuildTypeSet.insert( BuildType( r.getRefinery() ) );
        requiredBuildTypeSet.insert( BuildType( r.getSupplyProvider() ) );
        requiredBuildTypeSet.insert( BuildType( r.getCenter() ) );
        requiredBuildTypeSetByRace[r].insert( BuildType( r.getWorker() ) );
        requiredBuildTypeSetByRace[r].insert( BuildType( r.getRefinery() ) );
        requiredBuildTypeSetByRace[r].insert( BuildType( r.getSupplyProvider() ) );
        requiredBuildTypeSetByRace[r].insert( BuildType( r.getCenter() ) );
        buildTypeData[BuildType( r.getWorker() )].mask = WorkerMask;
        buildTypeData[BuildType( r.getRefinery() )].mask = RefineryMask;
        buildTypeData[BuildType( r.getSupplyProvider() )].mask = SupplyMask;
        buildTypeData[BuildType( r.getCenter() )].mask = CenterMask;
      }

      // Set masks for required build types, and generate required build type set by race
      for( BuildType i : requiredBuildTypeSet )
      {
        if ( buildTypeData[i.getID()].mask > 0 )
        {
          continue;
        }
        buildTypeData[i.getID()].mask = 1 << requiredBuildTypeSetByRace[i.getRace()].size();
        requiredBuildTypeSetByRace[i.getRace()].insert( i );
      }

      // Generate required mask for each build type
      for( BuildType i : buildTypeSet )
      {
        if ( i != BuildTypes::None )
        {
          for ( std::map< BuildType, int >::const_iterator j = i.requiredBuildTypes().begin(); j != i.requiredBuildTypes().end(); j++ )
          {
            buildTypeData[i.getID()].requiredMask |= j->first.getMask();
          }
        }
      }

      initializingBuildType = false;

    }
  }

  BuildType::BuildType()
  {
    this->id = BuildTypes::None.id;
  }

  BuildType::BuildType( int id )
  {
    this->id = id;
    if ( !initializingBuildType && ( id < 0 || id >= 203 || !buildTypeData[id].valid ) )
    {
      this->id = BuildTypes::None.id;
    }
  }

  BuildType::BuildType( const BuildType& other )
  {
    this->id = other.id;
  }

  BuildType::BuildType( const BWAPI::TechType& other )
  {
    std::map< BWAPI::TechType, BuildType >::iterator i = techTypeToBuildTypeMap.find( other );
    if ( i == techTypeToBuildTypeMap.end() )
    {
      this->id = BuildTypes::None.id;
    }
    else
    {
      this->id = i->second.id;
    }
  }

  BuildType::BuildType( const BWAPI::UnitType& other )
  {
    std::map< BWAPI::UnitType, BuildType >::iterator i = unitTypeToBuildTypeMap.find( other );
    if ( i == unitTypeToBuildTypeMap.end() )
    {
      this->id = BuildTypes::None.id;
    }
    else
    {
      this->id = i->second.id;
    }
  }

  BuildType::BuildType( const BWAPI::UpgradeType& other, int level )
  {
    std::map< BWAPI::UpgradeType, BuildType >::iterator i = upgradeTypeToBuildTypeMap.find( other );
    if ( i == upgradeTypeToBuildTypeMap.end() )
    {
      this->id = BuildTypes::None.id;
    }
    else
    {
      this->id = ( i->second.id ) + level - 1;
    }
  }

  BuildType& BuildType::operator=( const BuildType& other )
  {
    this->id = other.id;
    return *this;
  }

  BuildType& BuildType::operator=( const BWAPI::TechType& other )
  {
    std::map< BWAPI::TechType, BuildType >::iterator i = techTypeToBuildTypeMap.find( other );
    if ( i == techTypeToBuildTypeMap.end() )
    {
      this->id = BuildTypes::None.id;
    }
    else
    {
      this->id = i->second.id;
    }
    return *this;
  }

  BuildType& BuildType::operator=( const BWAPI::UnitType& other )
  {
    std::map< BWAPI::UnitType, BuildType >::iterator i = unitTypeToBuildTypeMap.find( other );
    if ( i == unitTypeToBuildTypeMap.end() )
    {
      this->id = BuildTypes::None.id;
    }
    else
    {
      this->id = i->second.id;
    }
    return *this;
  }

  BuildType& BuildType::operator=( const BWAPI::UpgradeType& other )
  {
    std::map< BWAPI::UpgradeType, BuildType >::iterator i = upgradeTypeToBuildTypeMap.find( other );
    if ( i == upgradeTypeToBuildTypeMap.end() )
    {
      this->id = BuildTypes::None.id;
    }
    else
    {
      this->id = i->second.id;
    }
    return *this;
  }

  bool BuildType::operator==( const BuildType& other ) const
  {
    return this->id == other.id;
  }

  bool BuildType::operator==( const BWAPI::TechType& other ) const
  {
    return this->id == BuildType( other ).id;
  }

  bool BuildType::operator==( const BWAPI::UnitType& other ) const
  {
    return this->id == BuildType( other ).id;
  }

  bool BuildType::operator==( const BWAPI::UpgradeType& other ) const
  {
    return this->id == BuildType( other ).id;
  }

  bool BuildType::operator<( const BuildType& other ) const
  {
    return this->id < other.id;
  }

  BuildType::operator int() const
  {
    return this->id;
  }

  int BuildType::getID() const
  {
    return this->id;
  }

  const std::string& BuildType::getName() const
  {
    return buildTypeData[this->id].name;
  }

  BWAPI::Race BuildType::getRace() const
  {
    return buildTypeData[this->id].race;
  }

  bool BuildType::isTechType() const
  {
    return buildTypeData[this->id].techType != BWAPI::TechTypes::None;
  }

  bool BuildType::isUnitType() const
  {
    return buildTypeData[this->id].unitType != BWAPI::UnitTypes::None;
  }

  bool BuildType::isUpgradeType() const
  {
    return buildTypeData[this->id].upgradeType != BWAPI::UpgradeTypes::None;
  }

  BWAPI::TechType BuildType::getTechType() const
  {
    return buildTypeData[this->id].techType;
  }

  BWAPI::UnitType BuildType::getUnitType() const
  {
    return buildTypeData[this->id].unitType;
  }

  BWAPI::UpgradeType BuildType::getUpgradeType() const
  {
    return buildTypeData[this->id].upgradeType;
  }

  int BuildType::getUpgradeLevel() const
  {
    return buildTypeData[this->id].upgradeLevel;
  }

  unsigned int BuildType::getMask() const
  {
    return buildTypeData[this->id].mask;
  }
  
  unsigned int BuildType::getRequiredMask() const
  {
    return buildTypeData[this->id].requiredMask;
  }

  const std::pair< BuildType, int > BuildType::whatBuilds() const
  {
    return buildTypeData[this->id].whatBuilds;
  }

  const std::map< BuildType, int >& BuildType::requiredBuildTypes() const
  {
    return buildTypeData[this->id].requiredBuildTypes;
  }

  bool BuildType::requiresPsi() const
  {
    return buildTypeData[this->id].requiresPsi;
  }

  bool BuildType::requiresLarva() const
  {
    return buildTypeData[this->id].requiresLarva;
  }

  BuildType BuildType::requiredAddon() const
  {
    return buildTypeData[this->id].requiredAddon;
  }

  int BuildType::mineralPrice() const
  {
    return buildTypeData[this->id].mineralPrice;
  }

  int BuildType::gasPrice() const
  {
    return buildTypeData[this->id].gasPrice;
  }

  int BuildType::builderTime() const
  {
    return buildTypeData[this->id].builderTime;
  }

  int BuildType::buildUnitTime() const
  {
    return buildTypeData[this->id].buildUnitTime;
  }

  int BuildType::prepTime() const
  {
    return buildTypeData[this->id].prepTime;
  }

  bool BuildType::createsUnit() const
  {
    return buildTypeData[this->id].createsUnit;
  }

  bool BuildType::morphsBuilder() const
  {
    return buildTypeData[this->id].morphsBuilder;
  }

  bool BuildType::needsBuildLocation() const
  {
    return buildTypeData[this->id].needsBuildLocation;
  }

  int BuildType::supplyRequired() const
  {
    return buildTypeData[this->id].supplyRequired;
  }

  int BuildType::supplyProvided() const
  {
    return buildTypeData[this->id].supplyProvided;
  }

  bool BuildType::build( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::TilePosition buildLocation ) const
  {
    // Sanity check
    if ( builder == NULL )
    {
      return false;
    }
    if ( buildTypeData[this->id].techType != BWAPI::TechTypes::None )
    {
      return builder->research( buildTypeData[this->id].techType );
    }

    if ( buildTypeData[this->id].upgradeType != BWAPI::UpgradeTypes::None )
    {
      return builder->upgrade( buildTypeData[this->id].upgradeType );
    }

    if ( buildTypeData[this->id].unitType != BWAPI::UnitTypes::None )
    {
      if ( buildTypeData[this->id].unitType == BWAPI::UnitTypes::Protoss_Archon )
      {
        return builder->useTech( BWAPI::TechTypes::Archon_Warp, secondBuilder );
      }
      if ( buildTypeData[this->id].unitType == BWAPI::UnitTypes::Protoss_Dark_Archon )
      {
        return builder->useTech( BWAPI::TechTypes::Dark_Archon_Meld, secondBuilder );
      }
      if ( buildTypeData[this->id].unitType.isAddon() )
      {
        return builder->buildAddon( buildTypeData[this->id].unitType );
      }
      if ( buildTypeData[this->id].unitType.isBuilding() == buildTypeData[this->id].unitType.whatBuilds().first.isBuilding() )
      {
        return builder->morph( buildTypeData[this->id].unitType );
      }
      if ( buildTypeData[this->id].unitType.isBuilding() )
      {
		  return builder->build(buildTypeData[this->id].unitType, buildLocation);
      }
      return builder->train( buildTypeData[this->id].unitType );
    }
    return false;
  }

  bool BuildType::isPreparing( BWAPI::Unit builder, BWAPI::Unit secondBuilder ) const
  {
    // Sanity check
    if ( builder == NULL )
    {
      return false;
    }

    if ( buildTypeData[this->id].techType != BWAPI::TechTypes::None )
    {
      return builder->isResearching() && builder->getTech() == buildTypeData[this->id].techType;
    }

    if ( buildTypeData[this->id].upgradeType != BWAPI::UpgradeTypes::None )
    {
      return builder->isUpgrading() && builder->getUpgrade() == buildTypeData[this->id].upgradeType;
    }

    if ( buildTypeData[this->id].unitType != BWAPI::UnitTypes::None )
    {
      return builder->isConstructing() ||
             builder->isBeingConstructed() ||
             builder->isMorphing() ||
             builder->isTraining() ||
             builder->getOrder() == BWAPI::Orders::ArchonWarp ||
             builder->getOrder() == BWAPI::Orders::DarkArchonMeld;
    }
    return false;
  }
  bool BuildType::isBuilding( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::Unit createdUnit ) const
  {
    // Sanity check
    if ( builder == NULL )
    {
      return false;
    }

    if ( buildTypeData[this->id].techType != BWAPI::TechTypes::None )
    {
      return builder->isResearching() && builder->getTech() == buildTypeData[this->id].techType;
    }

    if ( buildTypeData[this->id].upgradeType != BWAPI::UpgradeTypes::None )
    {
      return builder->isUpgrading() && builder->getUpgrade() == buildTypeData[this->id].upgradeType;
    }

    if ( buildTypeData[this->id].unitType != BWAPI::UnitTypes::None )
    {
      if ( buildTypeData[this->id].unitType == BWAPI::UnitTypes::Protoss_Archon )
      {
        return builder->getBuildType() == BWAPI::UnitTypes::Protoss_Archon ||
         secondBuilder->getBuildType() == BWAPI::UnitTypes::Protoss_Archon;
      }

      if ( buildTypeData[this->id].unitType == BWAPI::UnitTypes::Protoss_Dark_Archon )
      {
        return builder->getBuildType() == BWAPI::UnitTypes::Protoss_Dark_Archon ||
         secondBuilder->getBuildType() == BWAPI::UnitTypes::Protoss_Dark_Archon;
      }

      if ( buildTypeData[this->id].unitType.isAddon() )
      {
        return createdUnit != NULL &&
               createdUnit->exists() &&
               createdUnit->isConstructing() &&
               createdUnit->getType() == buildTypeData[this->id].unitType;
      }

      if ( buildTypeData[this->id].morphsBuilder )
      {
        return ( builder->isConstructing() || builder->isMorphing() );
      }

      if ( buildTypeData[this->id].unitType.isBuilding() )
      {
        return createdUnit != NULL &&
               createdUnit->exists() &&
               createdUnit->isConstructing() &&
               createdUnit->getType() == buildTypeData[this->id].unitType;
      }

      return builder->isTraining() &&
             builder->getTrainingQueue().size() > 0 &&
             ( *builder->getTrainingQueue().begin() ) == buildTypeData[this->id].unitType;
    }
    return false;
  }

  bool BuildType::isCompleted( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::Unit createdUnit, BWAPI::Unit secondCreatedUnit ) const
  {
    if ( buildTypeData[this->id].techType != BWAPI::TechTypes::None )
    {
      return BWAPI::Broodwar->self()->hasResearched( buildTypeData[this->id].techType );
    }

    if ( buildTypeData[this->id].upgradeType != BWAPI::UpgradeTypes::None )
    {
      return BWAPI::Broodwar->self()->getUpgradeLevel( buildTypeData[this->id].upgradeType ) >= buildTypeData[this->id].upgradeLevel;
    }

    if ( buildTypeData[this->id].unitType != BWAPI::UnitTypes::None )
    {
      if ( ( buildTypeData[this->id].createsUnit || buildTypeData[this->id].requiresLarva ) &&
           !( createdUnit != NULL &&
              createdUnit->exists() &&
              createdUnit->isCompleted() &&
              createdUnit->getType() == buildTypeData[this->id].unitType ) )
      {
        return false;
      }
      if ( ( buildTypeData[this->id].createsUnit && buildTypeData[this->id].requiresLarva ) &&
           !( secondCreatedUnit != NULL &&
              secondCreatedUnit->exists() &&
              secondCreatedUnit->isCompleted() &&
              secondCreatedUnit->getType() == buildTypeData[this->id].unitType ) )
      {
        return false;
      }
      if ( buildTypeData[this->id].morphsBuilder )
      {
        bool builderMorphed = ( builder != NULL && builder->exists() && builder->isCompleted() && builder->getType() == buildTypeData[this->id].unitType );
        bool secondBuilderMorphed = ( secondBuilder != NULL && secondBuilder->exists() && secondBuilder->isCompleted() && secondBuilder->getType() == buildTypeData[this->id].unitType );
        if ( builderMorphed == false && secondBuilderMorphed == false )
        {
          return false;
        }
      }
      if ( buildTypeData[this->id].unitType.isAddon() && builder->getAddon() != createdUnit )
      {
        return false;
      }
      return true;
    }
    return false;
  }

  int BuildType::remainingTime( BWAPI::Unit builder, BWAPI::Unit secondBuilder, BWAPI::Unit createdUnit ) const
  {
    // Sanity check
    if ( builder == NULL )
    {
      return buildTypeData[this->id].buildUnitTime;
    }

    if ( buildTypeData[this->id].techType != BWAPI::TechTypes::None )
    {
      return builder->getRemainingResearchTime();
    }
    if ( buildTypeData[this->id].upgradeType != BWAPI::UpgradeTypes::None )
    {
      return builder->getRemainingUpgradeTime();
    }
    if ( buildTypeData[this->id].unitType != BWAPI::UnitTypes::None )
    {
      int t = 0;
      if ( buildTypeData[this->id].createsUnit )
      {
        if ( createdUnit != NULL && createdUnit->exists() )
        {
          return createdUnit->getRemainingBuildTime();
        }
        else
        {
          return buildTypeData[this->id].buildUnitTime;
        }
      }
      else
      {
        int t = 0;
        if ( builder != NULL && builder->exists() )
        {
          t = max( builder->getRemainingBuildTime(), builder->getRemainingTrainTime() );
        }
        if ( secondBuilder != NULL && secondBuilder->exists() )
        {
          t = max( t, max( secondBuilder->getRemainingBuildTime(), secondBuilder->getRemainingTrainTime() ) );
        }
      }
      return t;
    }
    return 0;
  }

  BuildType BuildTypes::getBuildType( std::string name )
  {
    fixName( name );
    std::map< std::string, BuildType >::iterator i = buildTypeMap.find( name );
    if ( i == buildTypeMap.end() )
    {
      return BuildTypes::None;
    }
    return ( *i ).second;
  }

  std::set< BuildType >& BuildTypes::allBuildTypes()
  {
    return buildTypeSet;
  }
  std::set< BuildType >& BuildTypes::allBuildTypes( BWAPI::Race r )
  {
    return buildTypeSetByRace[r];
  }
  std::set< BuildType >& BuildTypes::allRequiredBuildTypes()
  {
    return requiredBuildTypeSet;
  }
  std::set< BuildType >& BuildTypes::allRequiredBuildTypes( BWAPI::Race r )
  {
    return requiredBuildTypeSetByRace[r];
  }
}