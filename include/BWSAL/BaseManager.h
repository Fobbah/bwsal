#pragma once
#include <map>
#include <set>
#include <BWTA.h>
#include <BWSAL/Arbitrator.h>
namespace BWSAL
{
  class Base;
  class BorderManager;

  /*! \brief Tracks all available base locations and whether the player has an active base there, 
   *  and automates expansions to base locations.
   */
  class BaseManager : public BWAPI::AIModule
  {
    public:
	  //! Creates the BaseManager singleton
      static BaseManager* create( BorderManager* borderManager );

	  //! Retrieve the BaseManager singleton
      static BaseManager* getInstance();

	  //! Destroy the active BaseManager singleton
      static void destroy();

	  //! Must be called every frame during BWAPI::AIModule onFrame(). Updates the list of bases and whether they have been claimed
      virtual void onFrame();

	  //! Must be called every frame during BWPAI::AIModule onUnitDestroy(). Notifies the bases of destroyed units so that they may update whether or not a base is currently claimed.
      virtual void onUnitDestroy( BWAPI::Unit unit );

	  //! Retrieve the Base at a given BWTA::BaseLocation
      Base* getBase( BWTA::BaseLocation* location ) const;

	  //! Schedule with the build order manager to create a base immediately at priority 1000
      Base* expandNow( BWTA::BaseLocation* location = NULL, bool getGas = true );

	  //! Schedule with the build order manager to expand as soon as a base becomes available at priority 1. 
      Base* expandWhenPossible( BWTA::BaseLocation* location = NULL, bool getGas = true );

	  //! Schedule with the build order manager to expand after the given frame number, at priority 1000
      Base* expandAtFrame( int frame, BWTA::BaseLocation* location = NULL, bool getGas = true );

	  //! Get the list of all active Bases
      const std::set< Base* >& getActiveBases() const;

	  //! Get the list of all ready Bases
      const std::set< Base* >& getReadyBases() const;
	  
	  //! Get the list of all Bases
      const std::set< Base* >& getAllBases() const;

	  //! Get the list of destroyed Bases
      const std::set< Base* >& getDestroyedBases() const;

	  //! Return the name of this component
      std::string getName() const;

    private:
      BWTA::BaseLocation* decideWhereToExpand() const;
      BaseManager();
      ~BaseManager();
      std::map< BWTA::BaseLocation*, Base* > m_location2base;
      std::set< Base* > m_activeBases;
      std::set< Base* > m_readyBases;
      std::set< Base* > m_allBases;
      std::set< Base* > m_destroyedBases;
      BorderManager* m_borderManager;
      static BaseManager* s_baseManager;
  };
}