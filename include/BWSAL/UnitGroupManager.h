#pragma once
#include <BWAPI.h>
#include <BWSAL/UnitGroup.h>
namespace BWSAL
{
  /*! \brief
   *  Monitors unit creation, discovery/evasion etc and maintains indexed lists of all visible units.
   *  Proides fast and straightforward ways to query for various subsets of the set of visible units.
   */
  class UnitGroupManager : public BWAPI::AIModule
  {
    public:
      static UnitGroupManager* create();
      static UnitGroupManager* getInstance();
      static void destroy();
      virtual void onUnitDiscover( BWAPI::Unit unit );
      virtual void onUnitEvade( BWAPI::Unit unit );
      virtual void onUnitMorph( BWAPI::Unit unit );
      virtual void onUnitRenegade( BWAPI::Unit unit );
      friend UnitGroup AllUnits();
      friend UnitGroup SelectAll();
      friend UnitGroup SelectAll( BWAPI::UnitType type );
      friend UnitGroup SelectAllEnemy();
      friend UnitGroup SelectAllEnemy( BWAPI::UnitType type );
      friend UnitGroup SelectAll( BWAPI::Player player, BWAPI::UnitType type );
    private:
      UnitGroupManager();
      ~UnitGroupManager();
      std::map< BWAPI::Unit, BWAPI::Player > unitOwner;
      std::map< BWAPI::Unit, BWAPI::UnitType > unitType;
      std::map< BWAPI::Player, std::map< BWAPI::UnitType, UnitGroup > > data;
      std::map< BWAPI::Player, UnitGroup > allOwnedUnits;
      UnitGroup allUnits;
      BWAPI::Player neutral;
      static UnitGroupManager* s_UnitGroupManager;
  };

  //! Return a UnitGroup containing all visible units
  UnitGroup AllUnits();

  //! Return a UnitGroup containing all (owned) visible units
  UnitGroup SelectAll();

  //! Return a UnitGroup containing all (owned) visible units of this UnitType
  UnitGroup SelectAll( BWAPI::UnitType type );

  //! Return a UnitGroup containing all visible enemy units
  UnitGroup SelectAllEnemy();

  //! Return a UnitGroup containing all visible enemy units of this UnitType
  UnitGroup SelectAllEnemy( BWAPI::UnitType type );

  //! Return a UnitGroup containing all visible units owned by the provided Player of this UnitType
  UnitGroup SelectAll( BWAPI::Player player, BWAPI::UnitType type );
}