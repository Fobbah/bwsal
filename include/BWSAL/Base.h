#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <BWSAL/BaseManager.h>
#include <set>
namespace BWSAL
{
  class MacroTask;

  /*! \brief Represents a Base being tracked by the BaseManager
   *         Stores information about the current status of that base.
   */
  class Base
  {
    friend BaseManager;
    public:
	  //! Return the location of this base
      BWTA::BaseLocation* getBaseLocation() const;

	  //! Return the resource depot (command center etc) associated with this base
      BWAPI::Unit getResourceDepot() const;

	  //! Return the refinery building associated with this base
      BWAPI::Unit getRefinery() const;

	  //! Return the set of mineral patches associated with this base
      const BWAPI::Unitset getMinerals() const;

	  //! Return the set of geysers associated with this base
      const BWAPI::Unitset getGeysers() const;

	  //! Pause this base FIXME review, is this dead code?
      void setPaused( bool paused );

	  //! Return true if this base is paused FIXME review, is this dead code?
      bool isPaused() const;

	  //! Return true if this base is ready for construction FIXME revieww, should this be hidden from public API?
      bool isReady() const;

	  //! Return true if this base is active TODO review
      bool isActive() const;

	  //! Update this base location FIXME hide from public API
      void update();

	  //! Notify this base location of the destroyed unit FIXME hide from public API
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