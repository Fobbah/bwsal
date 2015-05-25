#include "RushBot.h"
#include "../Addons/Util.h"
using namespace BWAPI;

void RushBot::onStart()
{

  this->showManagerAssignments=false;
  if (Broodwar->isReplay()) return;
  // Enable some cheat flags
  Broodwar->enableFlag(Flag::UserInput);
  //Broodwar->enableFlag(Flag::CompleteMapInformation);
  BWTA::readMap();
  BWTA::analyze();
  this->analyzed=true;
  this->buildManager       = new BuildManager(&this->arbitrator);
  this->techManager        = new TechManager(&this->arbitrator);
  this->upgradeManager     = new UpgradeManager(&this->arbitrator);
  this->scoutManager       = new ScoutManager(&this->arbitrator);
  this->workerManager      = new WorkerManager(&this->arbitrator);
  this->supplyManager      = new SupplyManager();
  this->baseManager        = new BaseManager();
  this->buildOrderManager  = new BuildOrderManager(this->buildManager,this->techManager,this->upgradeManager,this->workerManager,this->supplyManager);
  this->defenseManager     = new DefenseManager(&this->arbitrator);
  this->informationManager = new InformationManager();
  this->borderManager      = new BorderManager();
  this->unitGroupManager   = new UnitGroupManager();
  this->enhancedUI         = new EnhancedUI();

  this->supplyManager->setBuildManager(this->buildManager);
  this->supplyManager->setBuildOrderManager(this->buildOrderManager);
  this->techManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
  this->upgradeManager->setBuildingPlacer(this->buildManager->getBuildingPlacer());
  this->workerManager->setBaseManager(this->baseManager);
  this->workerManager->setBuildOrderManager(this->buildOrderManager);
  this->baseManager->setBuildOrderManager(this->buildOrderManager);
  this->borderManager->setInformationManager(this->informationManager);
  this->baseManager->setBorderManager(this->borderManager);
  this->defenseManager->setBorderManager(this->borderManager);
  this->scoutManager->setInformationManager(this->informationManager);
  
  BWAPI::Race race = Broodwar->self()->getRace();
  BWAPI::Race enemyRace = Broodwar->enemy()->getRace();
  BWAPI::UnitType workerType=race.getWorker();
  double minDist;
  BWTA::BaseLocation* natural=NULL;
  BWTA::BaseLocation* home=BWTA::getStartLocation(Broodwar->self());
  for(std::set<BWTA::BaseLocation*>::const_iterator b=BWTA::getBaseLocations().begin();b!=BWTA::getBaseLocations().end();b++)
  {
    if (*b==home) continue;
    double dist=home->getGroundDistance(*b);
    if (dist>0)
    {
      if (natural==NULL || dist<minDist)
      {
        minDist=dist;
        natural=*b;
      }
    }
  }
  this->buildOrderManager->enableDependencyResolver();
  //make the basic production facility

  BWTA::RectangleArray<double> sumSquaredDistanceMap(Broodwar->mapWidth(),Broodwar->mapHeight());
  sumSquaredDistanceMap.setTo(0);
  BWTA::RectangleArray<double> groundDistanceMap(Broodwar->mapWidth(),Broodwar->mapHeight());
  for(std::set<BWAPI::TilePosition>::iterator t = Broodwar->getStartLocations().begin();t!=Broodwar->getStartLocations().end();t++)
  {
    if (*t==Broodwar->self()->getStartLocation() && Broodwar->getStartLocations().size()>2) continue;
    BWTA::getGroundDistanceMap(*t,groundDistanceMap);
    for(int x=0;x<Broodwar->mapWidth();x++)
    {
      for(int y=0;y<Broodwar->mapHeight();y++)
      {
        double d=groundDistanceMap[x][y];
        if (d<0)
        {
          sumSquaredDistanceMap[x][y]=-1;
        }
        else
        {
          if (*t==Broodwar->self()->getStartLocation())
          {
            d*=0.5;
          }
          sumSquaredDistanceMap[x][y]+=d*d;
        }
      }
    }
  }
  TilePosition proxyLocation = TilePositions::None;
  double proxyLocationD;
  for(int x=0;x<Broodwar->mapWidth();x++)
  {
    for(int y=0;y<Broodwar->mapHeight();y++)
    {
      if (sumSquaredDistanceMap[x][y]>0)
      {
        if (proxyLocation == TilePositions::None ||
            sumSquaredDistanceMap[x][y]<proxyLocationD)
        {
          proxyLocation=TilePosition(x,y);
          proxyLocationD=sumSquaredDistanceMap[x][y];
        }
      }
    }
  }


  if (race == Races::Zerg)
  {
    int mode = rand()%6;
    initialPushSize = 6;
    if (mode==0)
    {
      rush_mode = "4 Pool";
      this->buildOrderManager->build(4,UnitTypes::Zerg_Drone,100);
      this->buildOrderManager->buildAdditional(3,UnitTypes::Zerg_Zergling,90);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,89);
    }
    else if (mode==1)
    {
      rush_mode = "5 Pool";
      this->buildOrderManager->build(5,UnitTypes::Zerg_Drone,100);
      this->buildOrderManager->buildAdditional(3,UnitTypes::Zerg_Zergling,90);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,89);
    }
    else if (mode==2)
    {
      rush_mode = "6 Pool";
      this->buildOrderManager->build(6,UnitTypes::Zerg_Drone,100);
      this->buildOrderManager->buildAdditional(3,UnitTypes::Zerg_Zergling,90);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,89);
    }
    else if (mode==3)
    {
      rush_mode = "7 Pool";
      this->buildOrderManager->build(7,UnitTypes::Zerg_Drone,100);
      this->buildOrderManager->buildAdditional(3,UnitTypes::Zerg_Zergling,90);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,89);
    }
    else if (mode==4)
    {
      rush_mode = "8 Pool";
      this->buildOrderManager->build(8,UnitTypes::Zerg_Drone,100);
      this->buildOrderManager->buildAdditional(3,UnitTypes::Zerg_Zergling,90);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,89);
    }
    else if (mode==5)
    {
      rush_mode = "9 Pool";
      this->buildOrderManager->build(9,UnitTypes::Zerg_Drone,100);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Overlord,95);
      this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Spawning_Pool,91);
      this->buildOrderManager->upgrade(1,UpgradeTypes::Metabolic_Boost,90);
      this->buildOrderManager->buildAdditional(6,UnitTypes::Zerg_Zergling,88);
      initialPushSize = 12;
    }
    this->buildOrderManager->buildAdditional(6,UnitTypes::Zerg_Zergling,70);
    this->buildOrderManager->buildAdditional(1,UnitTypes::Zerg_Overlord,50);
    this->buildOrderManager->buildAdditional(30,UnitTypes::Zerg_Zergling,40);
    //send an overlord out if Zerg
    this->scoutManager->setScoutCount(1);
  }
  else if (race == Races::Terran)
  {
    int mode = 0;//rand()%1;
    if (mode==0)
    {
      rush_mode = "Barracks Barracks Supply";
      this->buildOrderManager->build(8,UnitTypes::Terran_SCV,100);
      this->buildOrderManager->build(2,UnitTypes::Terran_Barracks,95,proxyLocation);
      this->buildOrderManager->build(1,UnitTypes::Terran_Supply_Depot,90);
      this->buildOrderManager->build(8,UnitTypes::Terran_Marine,85);
      this->buildOrderManager->build(2,UnitTypes::Terran_Supply_Depot,80);
      this->buildOrderManager->build(30,UnitTypes::Terran_Marine,75);
    }
    initialPushSize = 6;
  }
  else if (race == Races::Protoss)
  {
    int mode = rand()%3;
    if (mode==0)
    {
      rush_mode = "2 Gate";
      this->buildOrderManager->build(9,UnitTypes::Protoss_Probe,100);
      this->buildOrderManager->build(1,UnitTypes::Protoss_Pylon,95);
      this->buildOrderManager->build(2,UnitTypes::Protoss_Gateway,80);
      this->buildOrderManager->build(20,UnitTypes::Protoss_Zealot,75);
      initialPushSize = 4;
    }
    else if (mode==1)
    {
      rush_mode = "Proxy Two Gate";
      this->buildOrderManager->build(6,UnitTypes::Protoss_Probe,100);
      this->buildOrderManager->build(1,UnitTypes::Protoss_Pylon,95,proxyLocation);
      this->buildOrderManager->build(9,UnitTypes::Protoss_Probe,90);
      this->buildOrderManager->build(2,UnitTypes::Protoss_Gateway,80,proxyLocation);
      this->buildOrderManager->build(20,UnitTypes::Protoss_Zealot,75);
      initialPushSize = 4;
    }
    else if (mode==2)
    {
      rush_mode = "DT Rush";
      this->buildOrderManager->build(9,UnitTypes::Protoss_Probe,100);
      this->buildOrderManager->build(1,UnitTypes::Protoss_Pylon,95);
      this->buildOrderManager->build(1,UnitTypes::Protoss_Gateway,70);
      this->buildOrderManager->build(10,UnitTypes::Protoss_Dark_Templar,65);
      this->buildOrderManager->build(3,UnitTypes::Protoss_Gateway,60);
      initialPushSize = 2;
    }
    this->buildOrderManager->build(40,UnitTypes::Protoss_Zealot,40);
    this->buildOrderManager->build(4,UnitTypes::Protoss_Gateway,35);
  }
  startedAttacking = false;
  this->workerManager->enableAutoBuild();
  this->workerManager->setAutoBuildPriority(40);
  this->baseManager->setRefineryBuildPriority(30);

}

RushBot::~RushBot()
{
  delete this->buildManager;
  delete this->techManager;
  delete this->upgradeManager;
  delete this->scoutManager;
  delete this->workerManager;
  delete this->supplyManager;
  delete this->buildOrderManager;
  delete this->baseManager;
  delete this->defenseManager;
  delete this->informationManager;
  delete this->borderManager;
  delete this->unitGroupManager;
  delete this->enhancedUI;
}
void RushBot::onEnd(bool isWinner)
{
  log("onEnd(%d)\n",isWinner);
}
void RushBot::onFrame()
{
  if (Broodwar->isReplay()) return;
  if (!this->analyzed) return;
  Broodwar->drawTextScreen(300,0,"%s",rush_mode.c_str());
  this->buildManager->update();
  this->buildOrderManager->update();
  this->baseManager->update();
  this->workerManager->update();
  this->techManager->update();
  this->upgradeManager->update();
  this->supplyManager->update();
  this->scoutManager->update();
  this->enhancedUI->update();
  this->borderManager->update();
//  this->defenseManager->update();
  this->arbitrator.update();

  if (Broodwar->getFrameCount()>24*50)
    scoutManager->setScoutCount(1);

  if (this->informationManager->getEnemyBases().size()>0 && Broodwar->getFrameCount()%80 == 0)
  {
    UnitGroup army = SelectAll()(canAttack)(canMove).not(isWorker);
    if (startedAttacking==false)
    {
      if ((int)army.size()>=this->initialPushSize)
        startedAttacking = true;
    }
    if (startedAttacking)
    {
      army.attackMove((*this->informationManager->getEnemyBases().begin())->getTilePosition());
    }
  }
  std::set<Unit*> units=Broodwar->self()->getUnits();
  if (this->showManagerAssignments)
  {
    for(std::set<Unit*>::iterator i=units.begin();i!=units.end();i++)
    {
      if (this->arbitrator.hasBid(*i))
      {
        int x=(*i)->getPosition().x();
        int y=(*i)->getPosition().y();
        std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> > bids=this->arbitrator.getAllBidders(*i);
        int y_off=0;
        bool first = false;
        const char activeColor = '\x07', inactiveColor = '\x16';
        char color = activeColor;
        for(std::list< std::pair< Arbitrator::Controller<BWAPI::Unit*,double>*, double> >::iterator j=bids.begin();j!=bids.end();j++)
        {
          Broodwar->drawTextMap(x,y+y_off,"%c%s: %d",color,j->first->getShortName().c_str(),(int)j->second);
          y_off+=15;
          color = inactiveColor;
        }
      }
    }
  }

  UnitGroup myPylonsAndGateways = SelectAll()(Pylon,Gateway)(HitPoints,"<=",200);
  for each(Unit* u in myPylonsAndGateways)
  {
    Broodwar->drawCircleMap(u->getPosition().x(),u->getPosition().y(),20,Colors::Red);
  }
}

void RushBot::onUnitDestroy(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  this->arbitrator.onRemoveObject(unit);
  this->buildManager->onRemoveUnit(unit);
  this->techManager->onRemoveUnit(unit);
  this->upgradeManager->onRemoveUnit(unit);
  this->workerManager->onRemoveUnit(unit);
  this->scoutManager->onRemoveUnit(unit);
  this->defenseManager->onRemoveUnit(unit);
  this->informationManager->onUnitDestroy(unit);
  this->baseManager->onRemoveUnit(unit);
}

void RushBot::onUnitDiscover(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  this->informationManager->onUnitDiscover(unit);
  this->unitGroupManager->onUnitDiscover(unit);
}
void RushBot::onUnitEvade(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  this->informationManager->onUnitEvade(unit);
  this->unitGroupManager->onUnitEvade(unit);
}

void RushBot::onUnitMorph(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  this->unitGroupManager->onUnitMorph(unit);
}
void RushBot::onUnitRenegade(BWAPI::Unit* unit)
{
  if (Broodwar->isReplay()) return;
  this->unitGroupManager->onUnitRenegade(unit);
}

void RushBot::onSendText(std::string text)
{
  if (Broodwar->isReplay())
  {
    Broodwar->sendText("%s",text.c_str());
    return;
  }
  UnitType type=UnitTypes::getUnitType(text);
  if (text=="debug")
  {
    if (this->showManagerAssignments==false)
    {
      this->showManagerAssignments=true;
      this->buildOrderManager->setDebugMode(true);
      this->scoutManager->setDebugMode(true);
    }
    else
    {
      this->showManagerAssignments=false;
      this->buildOrderManager->setDebugMode(false);
      this->scoutManager->setDebugMode(false);
    }
    Broodwar->printf("%s",text.c_str());
    return;
  }
  if (text=="expand")
  {
    this->baseManager->expand();
    Broodwar->printf("%s",text.c_str());
    return;
  }
  if (type!=UnitTypes::Unknown)
  {
    this->buildOrderManager->buildAdditional(1,type,300);
  }
  else
  {
    TechType type=TechTypes::getTechType(text);
    if (type!=TechTypes::Unknown)
    {
      this->techManager->research(type);
    }
    else
    {
      UpgradeType type=UpgradeTypes::getUpgradeType(text);
      if (type!=UpgradeTypes::Unknown)
      {
        this->upgradeManager->upgrade(type);
      }
      else
        Broodwar->printf("You typed '%s'!",text.c_str());
    }
  }
  Broodwar->sendText("%s",text.c_str());
}
