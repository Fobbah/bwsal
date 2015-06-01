#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWSAL.h>
#include <BWSAL/EnhancedUI.h>
#include "OffenseManager.h"
#include "BwapiEventLogger.h"

namespace BWSAL {

class RushBot : public BWAPI::AIModule
{
public:
  void onStart() override;
  void onEnd(bool isWinner) override;
  void onFrame() override;
  void onUnitDiscover(BWAPI::Unit unit) override;
  void onUnitEvade(BWAPI::Unit unit) override;
  void onUnitHide(BWAPI::Unit unit) override;
  void onUnitMorph(BWAPI::Unit unit) override;
  void onUnitRenegade(BWAPI::Unit unit) override;
  void onUnitDestroy(BWAPI::Unit unit) override;
  void onSendText(std::string text) override;

  void onUnitShow(BWAPI::Unit unit) override;
  void onNukeDetect(BWAPI::Position position) override;
  void onReceiveText(BWAPI::Player player, std::string text) override;
  void onPlayerLeft(BWAPI::Player player) override;

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
  BWSAL::OffenseManager*	 m_offenseManager;
  BWSAL::BwapiEventLogger*   m_eventLogger;
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
};

}