#include <BWSAL/UnitGroupManager.h>
namespace BWSAL
{
  UnitGroupManager* UnitGroupManager::s_UnitGroupManager = NULL;
  UnitGroupManager* UnitGroupManager::create()
  {
    if ( s_UnitGroupManager )
    {
      return s_UnitGroupManager;
    }
    s_UnitGroupManager = new UnitGroupManager();
    return s_UnitGroupManager;
  }

  UnitGroupManager* UnitGroupManager::getInstance()
  {
    return s_UnitGroupManager;
  }

  void UnitGroupManager::destroy()
  {
    if ( s_UnitGroupManager )
    {
      delete s_UnitGroupManager;
    }
  }

  UnitGroupManager::UnitGroupManager()
  {
    for each ( BWAPI::Unit u in BWAPI::Broodwar->getAllUnits() )
    {
      onUnitDiscover( u );
    }
    neutral = NULL;
    for each ( BWAPI::Player p in BWAPI::Broodwar->getPlayers() )
    {
      if ( p->isNeutral() )
      {
        neutral = p;
      }
    }
  }

  UnitGroupManager::~UnitGroupManager()
  {
    s_UnitGroupManager = NULL;
  }

  void UnitGroupManager::onUnitDiscover( BWAPI::Unit unit )
  {
    unitOwner[unit] = unit->getPlayer();
    unitType[unit] = unit->getType();
    data[unit->getPlayer()][unit->getType()].insert( unit );
    allOwnedUnits[unit->getPlayer()].insert( unit );
    allUnits.insert( unit );
  }

  void UnitGroupManager::onUnitEvade( BWAPI::Unit unit )
  {
    unitOwner[unit] = unit->getPlayer();
    unitType[unit] = unit->getType();
    data[unit->getPlayer()][unit->getType()].erase( unit );
    allOwnedUnits[unit->getPlayer()].erase( unit );
    allUnits.erase( unit );
  }

  void UnitGroupManager::onUnitMorph( BWAPI::Unit unit )
  {
    data[unitOwner[unit]][unitType[unit]].erase( unit );
    unitType[unit] = unit->getType();
    unitOwner[unit] = unit->getPlayer();
    data[unit->getPlayer()][unit->getType()].insert( unit );
  }

  void UnitGroupManager::onUnitRenegade( BWAPI::Unit unit )
  {
    data[unitOwner[unit]][unitType[unit]].erase( unit );
    allOwnedUnits[unitOwner[unit]].erase( unit );
    unitType[unit] = unit->getType();
    unitOwner[unit] = unit->getPlayer();
    data[unit->getPlayer()][unit->getType()].insert( unit );
    allOwnedUnits[unit->getPlayer()].insert( unit );
  }

  UnitGroup AllUnits()
  {
    return UnitGroupManager::s_UnitGroupManager->allUnits;
  }

  UnitGroup SelectAll()
  {
    return UnitGroupManager::s_UnitGroupManager->allOwnedUnits[BWAPI::Broodwar->self()];
  }

  UnitGroup SelectAll( BWAPI::UnitType type )
  {
    if ( type.isNeutral() && UnitGroupManager::s_UnitGroupManager->neutral != NULL )
    {
      return UnitGroupManager::s_UnitGroupManager->data[UnitGroupManager::s_UnitGroupManager->neutral][type];
    }
    return UnitGroupManager::s_UnitGroupManager->data[BWAPI::Broodwar->self()][type];
  }

  UnitGroup SelectAllEnemy()
  {
    return UnitGroupManager::s_UnitGroupManager->allOwnedUnits[BWAPI::Broodwar->enemy()];
  }

  UnitGroup SelectAllEnemy( BWAPI::UnitType type )
  {
    return UnitGroupManager::s_UnitGroupManager->data[BWAPI::Broodwar->enemy()][type];
  }

  UnitGroup SelectAll( BWAPI::Player player, BWAPI::UnitType type )
  {
    return UnitGroupManager::s_UnitGroupManager->data[player][type];
  }
}