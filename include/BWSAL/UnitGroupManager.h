#pragma once
#include <BWAPI.h>
#include <BWSAL/UnitGroup.h>
namespace BWSAL
{
  class UnitGroupManager : public BWAPI::AIModule
  {
    public:
      static UnitGroupManager* create();
      static UnitGroupManager* getInstance();
      static void destroy();
      virtual void onUnitDiscover( BWAPI::Unit* unit );
      virtual void onUnitEvade( BWAPI::Unit* unit );
      virtual void onUnitMorph( BWAPI::Unit* unit );
      virtual void onUnitRenegade( BWAPI::Unit* unit );
      friend UnitGroup AllUnits();
      friend UnitGroup SelectAll();
      friend UnitGroup SelectAll( BWAPI::UnitType type );
      friend UnitGroup SelectAllEnemy();
      friend UnitGroup SelectAllEnemy( BWAPI::UnitType type );
      friend UnitGroup SelectAll( BWAPI::Player* player, BWAPI::UnitType type );
    private:
      UnitGroupManager();
      ~UnitGroupManager();
      std::map< BWAPI::Unit*, BWAPI::Player* > unitOwner;
      std::map< BWAPI::Unit*, BWAPI::UnitType > unitType;
      std::map< BWAPI::Player*, std::map< BWAPI::UnitType, UnitGroup > > data;
      std::map< BWAPI::Player*, UnitGroup > allOwnedUnits;
      UnitGroup allUnits;
      BWAPI::Player* neutral;
      static UnitGroupManager* s_UnitGroupManager;
  };
  UnitGroup AllUnits();
  UnitGroup SelectAll();
  UnitGroup SelectAll( BWAPI::UnitType type );
  UnitGroup SelectAllEnemy();
  UnitGroup SelectAllEnemy( BWAPI::UnitType type );
  UnitGroup SelectAll( BWAPI::Player* player, BWAPI::UnitType type );
}