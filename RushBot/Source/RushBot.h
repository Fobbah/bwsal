#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWSAL.h>

namespace BWSAL {

class RushBot : public BWAPI::AIModule
{
public:
  virtual void onStart();
  virtual void onEnd(bool isWinner);
  virtual void onFrame();
  virtual void onUnitDiscover(BWAPI::Unit* unit);
  virtual void onUnitEvade(BWAPI::Unit* unit);
  virtual void onUnitMorph(BWAPI::Unit* unit);
  virtual void onUnitRenegade(BWAPI::Unit* unit);
  virtual void onUnitDestroy(BWAPI::Unit* unit);
  virtual void onSendText(std::string text);
  ~RushBot(); //not part of BWAPI::AIModule
  void showStats(); //not part of BWAPI::AIModule
  void showPlayers();
  void showForces();
  bool analyzed;
  std::map<BWAPI::Unit*,BWAPI::UnitType> buildings;
  Arbitrator<BWAPI::Unit*,double> arbitrator;
  WorkerManager* workerManager;
  SupplyManager* supplyManager;
  /*
  BuildManager* buildManager;
  TechManager* techManager;
  UpgradeManager* upgradeManager;
  */
  BaseManager* baseManager;
  ScoutManager* scoutManager;
  BuildOrderManager* buildOrderManager;
  DefenseManager* defenseManager;
  InformationManager* informationManager;
  BorderManager* borderManager;
  UnitGroupManager* unitGroupManager;
  //EnhancedUI* enhancedUI;
  bool showManagerAssignments;
  std::string rush_mode;
  int initialPushSize;
  bool startedAttacking;
};

}