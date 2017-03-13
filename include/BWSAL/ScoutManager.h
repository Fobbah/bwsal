#pragma once
#include <BWSAL/Arbitrator.h>
#include <BWAPI.h>
namespace BWSAL
{
  class InformationManager;

  /*! \brief The scout manager takes a worker and sends it to all base locations to scout the enemy.
   */
  class ScoutManager : public UnitController, public BWAPI::AIModule
  {
    public:
	  //! Create the ScoutManager singleton instance
      static ScoutManager* create( UnitArbitrator* arbitrator, InformationManager* informationManager );

	  //! Return the ScoutManager singleton instance
      static ScoutManager* getInstance();

	  //! Destroy the ScoutManager singleton instance
      static void destroy();

	  //! UnitController callback, the scout manager has been offered these units by the Arbitrator
      virtual void onOffer( std::set< BWAPI::Unit > units );

	  //! UnitController callback, the Arbitrator has revoked access to this unit, and been outbid with the supplied bid
      virtual void onRevoke( BWAPI::Unit unit, double bid );

	  //! Should be called every frame from AIModule::onFrame()
      virtual void onFrame();

	  //! Return the name of this component
      virtual std::string getName() const;

	  //! Initialise the scout manager
      void initialize();

	  //! Returns true if there is at least 1 active scouting unit
      bool isScouting() const;

	  //! Sets the numberof scouting units to the given amount
      void setScoutCount( int count );

	  //! Enable the visualisation of debugging information
      void setDebugMode( bool debugMode );
    private:
      ScoutManager();
      ~ScoutManager();
      class ScoutData
      {
        public:
          enum ScoutMode
          {
            Idle,
            Searching,
            Roaming,
            Harassing,
            Fleeing
          };
          ScoutData(){ m_mode = Idle; }
          BWTA::BaseLocation* m_target;
          ScoutMode m_mode;
      };

      std::map< BWAPI::Unit, ScoutData > m_scouts;
      UnitArbitrator* m_arbitrator;
      
      std::list< BWTA::BaseLocation* > m_baseLocationsToScout;
      std::set< BWTA::BaseLocation* > m_baseLocationsExplored;
      BWTA::BaseLocation* m_myStartLocation;
      std::pair< std::list< BWTA::BaseLocation* >, double > ScoutManager::getShortestPath( BWTA::BaseLocation* currentBaseLocation, std::set< BWTA::BaseLocation* > &baseLocations );
      std::pair< std::list< BWTA::BaseLocation* >, double > ScoutManager::getScoutPath( std::set< BWTA::BaseLocation* > baseLocations );
      bool needMoreScouts() const;
      void requestScout( double bid );
      void addScout( BWAPI::Unit unit );
      void updateScoutAssignments();
      void drawAssignments();
      InformationManager* m_informationManager;
      
      size_t m_desiredScoutCount;
      int m_scoutingStartFrame;
      bool m_debugMode;
      static ScoutManager* s_scoutManager;
  };
}