#include "BasicAIModule.h"
#include <BWSAL/Util.h>
#include <algorithm>
using namespace BWSAL;
using namespace BWAPI;
using namespace std;
BasicAIModule::BasicAIModule()
{
}

BasicAIModule::~BasicAIModule()
{
}

void BasicAIModule::onStart()
{
  BWSAL::BWSAL_init();
  Broodwar->enableFlag( Flag::UserInput );
  BWTA::readMap();
  BWTA::analyze();
  BWSAL::resetLog();

  m_informationManager = InformationManager::create();
  m_borderManager = BorderManager::create( m_informationManager );
  m_baseManager = BaseManager::create( m_borderManager );
  m_buildingPlacer = new BFSBuildingPlacer();
  m_reservedMap = ReservedMap::create();
  m_unitArbitrator = new UnitArbitrator();
  m_unitGroupManager = UnitGroupManager::create();
  m_buildUnitManager = BuildUnitManager::create();
  m_buildEventTimeline = BuildEventTimeline::create( m_buildUnitManager );
  m_taskScheduler = TaskScheduler::create( m_buildEventTimeline, m_buildUnitManager );
  m_taskExecutor = TaskExecutor::create( m_unitArbitrator, m_buildEventTimeline, m_reservedMap, m_buildingPlacer );
  m_workerManager = WorkerManager::create( m_unitArbitrator, m_baseManager );
  m_scoutManager = ScoutManager::create( m_unitArbitrator, m_informationManager );
  m_defenseManager = DefenseManager::create( m_unitArbitrator, m_borderManager );
  m_buildOrderManager = BuildOrderManager::create( m_taskScheduler, m_taskExecutor, m_buildUnitManager );
  m_supplyManager = SupplyManager::create( m_buildOrderManager, m_taskScheduler );
  m_enhancedUI = new EnhancedUI();

  m_buildEventTimeline->initialize();
  m_scoutManager->initialize();

  m_modules.push_back( m_informationManager );
  m_modules.push_back( m_borderManager );
  m_modules.push_back( m_baseManager );
  m_modules.push_back( m_buildEventTimeline );
  m_modules.push_back( m_reservedMap );
  m_modules.push_back( m_unitGroupManager );
  m_modules.push_back( m_buildUnitManager );
  m_modules.push_back( m_taskScheduler );
  m_modules.push_back( m_taskExecutor );
  m_modules.push_back( m_workerManager );
  m_modules.push_back( m_scoutManager );
  m_modules.push_back( m_defenseManager );

  BWAPI::Race race = Broodwar->self()->getRace();
  BWAPI::Race enemyRace = Broodwar->enemy()->getRace();
  double minDist;
  BWTA::BaseLocation* natural = NULL;
  BWTA::BaseLocation* home = BWTA::getStartLocation( Broodwar->self() );
  for( BWTA::BaseLocation* bl : BWTA::getBaseLocations() )
  {
    if ( bl != home )
    {
      double dist = home->getGroundDistance( bl );
      if ( dist > 0 )
      {
        if ( natural == NULL || dist < minDist)
        {
          minDist = dist;
          natural = bl;
        }
      }
    }
  }
  int buildID = 1;
  if ( race == Races::Zerg )
  {
    // Send an overlord out if Zerg
    m_scoutManager->setScoutCount( 1 );

    if (buildID == 1)
    {
      //morph 5 lurkers (tests dependency resolver, task scheduler)
      m_buildOrderManager->build( 8, UnitTypes::Zerg_Drone, 90 );
      m_buildOrderManager->buildAdditional( 1, UnitTypes::Zerg_Overlord, 85 );
      m_buildOrderManager->build( 12, UnitTypes::Zerg_Drone, 84 );
      m_buildOrderManager->buildAdditional( 1, UnitTypes::Zerg_Lair, 82);
      m_buildOrderManager->buildAdditional( 5, UnitTypes::Zerg_Lurker, 80);
      m_buildOrderManager->build( 12, UnitTypes::Zerg_Drone, 30 );
    }
    else if (buildID >= 2)
    {
      //12 pool 6 lings (tests larva task scheduler)
      m_buildOrderManager->build( 8, UnitTypes::Zerg_Drone, 90 );
      m_buildOrderManager->buildAdditional( 1, UnitTypes::Zerg_Overlord, 85 );
      m_buildOrderManager->build( 12, UnitTypes::Zerg_Drone, 84 );
      m_buildOrderManager->buildAdditional( 1, UnitTypes::Zerg_Spawning_Pool, 70 );
      m_buildOrderManager->buildAdditional( 3, UnitTypes::Zerg_Zergling, 65 );
      m_buildOrderManager->build( 40, UnitTypes::Zerg_Drone, 62 );
    }
  }
  else if ( race == Races::Terran )
  {
    if (buildID == 1)
    {
      //build 3 comsat stations (tests dependency resolver)
      m_buildOrderManager->build(9,UnitTypes::Terran_SCV,90);
      m_buildOrderManager->build(1,UnitTypes::Terran_Supply_Depot,85);
      m_buildOrderManager->build(20,UnitTypes::Terran_SCV,80);
      m_buildOrderManager->buildAdditional(3,UnitTypes::Terran_Comsat_Station,60);
    }
    else if (buildID == 2)
    {
      //a complex build order
      m_buildOrderManager->build(9,UnitTypes::Terran_SCV,90);
      m_buildOrderManager->build(1,UnitTypes::Terran_Supply_Depot,85);
      m_buildOrderManager->build(20,UnitTypes::Terran_SCV,80);
      m_buildOrderManager->buildAdditional(1,UnitTypes::Terran_Barracks,60);
      m_buildOrderManager->buildAdditional(9,UnitTypes::Terran_Marine,45);
 
      m_buildOrderManager->buildAdditional(1,UnitTypes::Terran_Refinery,42);
      m_buildOrderManager->buildAdditional(1,UnitTypes::Terran_Barracks,40);
      m_buildOrderManager->buildAdditional(1,UnitTypes::Terran_Academy,39);
      m_buildOrderManager->buildAdditional(9,UnitTypes::Terran_Medic,38);


      m_buildOrderManager->research(TechTypes::Stim_Packs,35);
      m_buildOrderManager->research(TechTypes::Tank_Siege_Mode,35);
      m_buildOrderManager->buildAdditional(3,UnitTypes::Terran_Siege_Tank_Tank_Mode,34);
      m_buildOrderManager->buildAdditional(2,UnitTypes::Terran_Science_Vessel,30);
      m_buildOrderManager->research(TechTypes::Irradiate,30);

      m_buildOrderManager->upgrade(1,UpgradeTypes::Terran_Infantry_Weapons,20);
      m_buildOrderManager->build(3,UnitTypes::Terran_Missile_Turret,13);
      m_buildOrderManager->upgrade(3,UpgradeTypes::Terran_Infantry_Weapons,12);
      m_buildOrderManager->upgrade(3,UpgradeTypes::Terran_Infantry_Armor,12);
      m_buildOrderManager->build(1,UnitTypes::Terran_Engineering_Bay,11);
      m_buildOrderManager->buildAdditional(40,UnitTypes::Terran_Marine,10);
      m_buildOrderManager->build(6,UnitTypes::Terran_Barracks,8);
      m_buildOrderManager->build(2,UnitTypes::Terran_Engineering_Bay,7);
      m_buildOrderManager->buildAdditional(10,UnitTypes::Terran_Siege_Tank_Tank_Mode,5);
    }
    else if (buildID >= 3)
    {
      //terran mech - tests task ordering with add-on requiring units
      m_buildOrderManager->build(9,UnitTypes::Terran_SCV,90);
      m_buildOrderManager->build(1,UnitTypes::Terran_Supply_Depot,85);
      m_buildOrderManager->build(20,UnitTypes::Terran_SCV,80);
      m_buildOrderManager->buildAdditional(2,BWAPI::UnitTypes::Terran_Machine_Shop,70);
      m_buildOrderManager->buildAdditional(3,BWAPI::UnitTypes::Terran_Factory,60);
      m_buildOrderManager->research(TechTypes::Spider_Mines,55);
      m_buildOrderManager->research(TechTypes::Tank_Siege_Mode,55);
      m_buildOrderManager->buildAdditional(20,BWAPI::UnitTypes::Terran_Vulture,40);
      m_buildOrderManager->buildAdditional(20,BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode,40);
      m_buildOrderManager->upgrade(3,UpgradeTypes::Terran_Vehicle_Weapons,20);
    }
    
  }
  else if (race == Races::Protoss)
  {
    //build 20 carriers - tests dependency resolver
    m_buildOrderManager->build(8,UnitTypes::Protoss_Probe,90);
    m_buildOrderManager->build(1,UnitTypes::Protoss_Pylon,85);
    m_buildOrderManager->build(20,UnitTypes::Protoss_Probe,80);
    m_buildOrderManager->buildAdditional(10,UnitTypes::Protoss_Dragoon,70);
    m_buildOrderManager->buildAdditional(10,UnitTypes::Protoss_Zealot,70);
    m_buildOrderManager->upgrade(1,UpgradeTypes::Singularity_Charge,61);
    m_buildOrderManager->buildAdditional(20,UnitTypes::Protoss_Carrier,60);
  }
  m_drawTasks = true;
  m_drawAssignments = false;
  m_drawResources = true;
  m_drawLarva = Broodwar->self()->getRace() == Races::Zerg;

}

void BasicAIModule::onEnd( bool isWinner )
{
}

void BasicAIModule::onFrame()
{
  if ( Broodwar->isPaused() )
  {
    return;
  }

  m_unitArbitrator->update();
  m_buildEventTimeline->reset();

  for( BWAPI::AIModule* m : m_modules )
  {
    m->onFrame();
  }
  m_enhancedUI->update();
  m_borderManager->draw();
  if ( m_informationManager->getEnemyBases().empty() )
  {
    if ( Broodwar->getFrameCount() > 2 * 24 * 60 )
    {
      m_scoutManager->setScoutCount( 1 );
    }
  }
  else
  {
    m_scoutManager->setScoutCount( 0 );
  }


  m_buildEventTimeline->m_initialState.createUnclaimedBuildUnits();

  m_buildOrderManager->onFrame();
  m_supplyManager->onFrame();

  if ( m_drawResources )
  {
    m_buildEventTimeline->draw();
  }
  if ( m_drawLarva )
  {
    m_buildEventTimeline->drawLarvaCounts();
  }
  int y = -16;
  Broodwar->drawTextScreen( 0, y += 16, "Time: %d, Minerals: %f, Gas: %f", m_buildEventTimeline->m_initialState.getTime(), m_buildEventTimeline->m_initialState.getMinerals(), m_buildEventTimeline->m_initialState.getGas() );
  y = 50 - 16;
  if ( m_drawTasks )
  {
    m_buildOrderManager->draw(20, y);
  }

  BWAPI::Unitset units = Broodwar->self()->getUnits();
  if ( m_drawAssignments )
  {
    for( Unit i : units )
    {
      if ( m_unitArbitrator->hasBid( i ) )
      {
        int x = i->getPosition().x;
        int y = i->getPosition().y;
        std::list< std::pair< UnitController*, double > > bids = m_unitArbitrator->getAllBidders( i );
        int y_off = 0;
        bool first = false;
        const char activeColor = '\x07', inactiveColor = '\x16';
        char color = activeColor;
        for ( std::list< std::pair< UnitController*, double > >::iterator j = bids.begin(); j != bids.end(); j++ )
        {
          Broodwar->drawTextMap( x, y + y_off, "%c%s: %d", color, j->first->getName().c_str(), (int)j->second );
          y_off += 15;
          color = inactiveColor;
        }
      }
    }
  }
}

void BasicAIModule::onSendText( std::string text )
{
  for( BWAPI::AIModule* m : m_modules )
  {
    m->onSendText( text );
  }
  if ( text == "t")
  {
    m_drawTasks = !m_drawTasks;
  }
  else if ( text == "a")
  {
    m_drawAssignments = !m_drawAssignments;
  }
  else if ( text == "r")
  {
    m_drawResources = !m_drawResources;
  }
  else if ( text == "l")
  {
    m_drawLarva = !m_drawLarva;
  }
  else if ( text == "expand" )
  {
    m_baseManager->expandNow();
  }
  else if ( BuildTypes::getBuildType( text ) != BuildTypes::None )
  {
    m_buildOrderManager->buildAdditional( 1, BuildTypes::getBuildType( text ), 300 );
  }
  else
  {
    BWAPI::Broodwar->printf( "Unrecognized command: '%s'", text.c_str() );
    BWAPI::Broodwar->sendText( text.c_str() );
  }
}

void BasicAIModule::onUnitDiscover( BWAPI::Unit unit )
{
  for( BWAPI::AIModule* m : m_modules )
  {
    m->onUnitDiscover( unit );
  }
}

void BasicAIModule::onUnitEvade( BWAPI::Unit unit )
{
  for( BWAPI::AIModule* m : m_modules )
  {
    m->onUnitEvade( unit );
  }
}

void BasicAIModule::onUnitDestroy( BWAPI::Unit unit )
{
  m_unitArbitrator->onRemoveObject( unit );
  for( BWAPI::AIModule* m : m_modules )
  {
    m->onUnitDestroy( unit );
  }
}

void BasicAIModule::onUnitMorph( BWAPI::Unit unit )
{
  for( BWAPI::AIModule* m : m_modules )
  {
    m->onUnitMorph( unit );
  }
}

void BasicAIModule::onUnitRenegade( BWAPI::Unit unit )
{
  for( BWAPI::AIModule* m : m_modules )
  {
    m->onUnitRenegade( unit );
  }
}

void BasicAIModule::onUnitComplete( BWAPI::Unit unit )
{
  for( BWAPI::AIModule* m : m_modules )
  {
    m->onUnitComplete( unit );
  }
}
