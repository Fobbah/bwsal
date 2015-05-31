#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWSAL.h>
#include <BWSAL/EnhancedUI.h>

namespace BWSAL {

class RushBot : public BWAPI::AIModule
{
public:
  virtual void onStart() override;
  virtual void onEnd(bool isWinner) override;
  virtual void onFrame() override;
  virtual void onUnitDiscover(BWAPI::Unit unit) override;
  virtual void onUnitEvade(BWAPI::Unit unit) override;
  virtual void onUnitMorph(BWAPI::Unit unit) override;
  virtual void onUnitRenegade(BWAPI::Unit unit) override;
  virtual void onUnitDestroy(BWAPI::Unit unit) override;
  virtual void onSendText(std::string text) override;
  ~RushBot(); //not part of BWAPI::AIModule
  void showStats(); //not part of BWAPI::AIModule
  void showPlayers();
  void showForces();
  bool analyzed;

  std::list< BWAPI::AIModule* > m_modules;
  BWSAL::BorderManager*      m_borderManager;
  BWSAL::InformationManager* m_informationManager;
  BWSAL::BaseManager*        m_baseManager;
  BWSAL::BuildEventTimeline* m_buildEventTimeline;
  BWSAL::BuildingPlacer*     m_buildingPlacer;
  BWSAL::ReservedMap*        m_reservedMap;
  BWSAL::TaskScheduler*      m_taskScheduler;
  BWSAL::TaskExecutor*       m_taskExecutor;
  BWSAL::UnitArbitrator*     m_unitArbitrator;
  BWSAL::UnitGroupManager*   m_unitGroupManager;
  BWSAL::WorkerManager*      m_workerManager;
  BWSAL::BuildUnitManager*   m_buildUnitManager;
  BWSAL::ScoutManager*       m_scoutManager;
  //BWSAL::DefenseManager*     m_defenseManager;
  BWSAL::BuildOrderManager*  m_buildOrderManager;
  BWSAL::SupplyManager*      m_supplyManager;
  EnhancedUI*                m_enhancedUI;
  /*
  std::map<BWAPI::Unit*,BWAPI::UnitType> buildings;
  Arbitrator<BWAPI::Unit*,double> arbitrator;
  WorkerManager* workerManager;
  SupplyManager* supplyManager;
  BuildManager* buildManager;
  TechManager* techManager;
  UpgradeManager* upgradeManager;
  BaseManager* baseManager;
  ScoutManager* scoutManager;
  BuildOrderManager* buildOrderManager;
  DefenseManager* defenseManager;
  InformationManager* informationManager;
  BorderManager* borderManager;
  UnitGroupManager* unitGroupManager;
  */
  //EnhancedUI* enhancedUI;
  bool showManagerAssignments;
  std::string rush_mode;
  int initialPushSize;
  bool startedAttacking;
};

}