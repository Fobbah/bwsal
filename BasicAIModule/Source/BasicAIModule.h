#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWSAL.h>
#include "EnhancedUI.h"

class BasicAIModule : public BWAPI::AIModule
{
  public:
    BasicAIModule();
    ~BasicAIModule();
    virtual void onStart();
    virtual void onEnd( bool isWinner );
    virtual void onFrame();
    virtual void onSendText( std::string text );
    virtual void onUnitDiscover( BWAPI::Unit* unit );
    virtual void onUnitEvade( BWAPI::Unit* unit );
    virtual void onUnitDestroy( BWAPI::Unit* unit );
    virtual void onUnitMorph( BWAPI::Unit* unit );
    virtual void onUnitRenegade( BWAPI::Unit* unit );
    virtual void onUnitComplete( BWAPI::Unit* unit );
  private:
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
    BWSAL::DefenseManager*     m_defenseManager;
    BWSAL::BuildOrderManager*  m_buildOrderManager;
    BWSAL::SupplyManager*      m_supplyManager;
    EnhancedUI*                m_enhancedUI;
    bool m_drawTasks;
    bool m_drawAssignments;
    bool m_drawResources;
    bool m_drawLarva;

};