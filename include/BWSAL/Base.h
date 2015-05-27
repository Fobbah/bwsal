#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWSAL/BaseManager.h>
#include <set>
namespace BWSAL
{
  class MacroTask;
  class Base
  {
    friend BaseManager;
    public:
      BWTA::BaseLocation* getBaseLocation() const;
      BWAPI::Unit getResourceDepot() const;
      BWAPI::Unit getRefinery() const;
      const BWAPI::Unitset getMinerals() const;
      const BWAPI::Unitset getGeysers() const;
      void setPaused( bool paused );
      bool isPaused() const;
      bool isReady() const;
      bool isActive() const;
      void update();
      void onUnitDestroy( BWAPI::Unit u );

    protected:
      static Base* CreateBaseNow( BWTA::BaseLocation* bl, bool getGas = true );
      static Base* CreateBaseWhenPossible( BWTA::BaseLocation* bl, bool getGas = true );
      static Base* CreateBaseAtFrame( BWTA::BaseLocation* bl, int frame, bool getGas = true );
      Base( BWTA::BaseLocation* b, BWAPI::Unit resourceDepot = NULL );
      BWTA::BaseLocation* m_baseLocation;
      BWAPI::Unit m_resourceDepot;
      BWAPI::Unit m_refinery;
      MacroTask* m_macroTask;
      bool m_ready;
      bool m_paused;
  };
}