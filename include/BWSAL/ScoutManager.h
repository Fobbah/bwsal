#pragma once
#include <BWSAL/Arbitrator.h>
#include <BWAPI.h>
namespace BWSAL
{
  class InformationManager;
  class ScoutManager : public UnitController, public BWAPI::AIModule
  {
    public:
      static ScoutManager* create( UnitArbitrator* arbitrator, InformationManager* informationManager );
      static ScoutManager* getInstance();
      static void destroy();
      virtual void onOffer( std::set< BWAPI::Unit* > units );
      virtual void onRevoke( BWAPI::Unit* unit, double bid );
      virtual void onFrame();

      virtual std::string getName() const;
      void initialize();

      bool isScouting() const;
      void setScoutCount( int count );
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

      std::map< BWAPI::Unit*, ScoutData > m_scouts;
      UnitArbitrator* m_arbitrator;
      
      std::list< BWTA::BaseLocation* > m_baseLocationsToScout;
      std::set< BWTA::BaseLocation* > m_baseLocationsExplored;
      BWTA::BaseLocation* m_myStartLocation;
      std::pair< std::list< BWTA::BaseLocation* >, double > ScoutManager::getShortestPath( BWTA::BaseLocation* currentBaseLocation, std::set< BWTA::BaseLocation* > &baseLocations );
      std::pair< std::list< BWTA::BaseLocation* >, double > ScoutManager::getScoutPath( std::set< BWTA::BaseLocation* > baseLocations );
      bool needMoreScouts() const;
      void requestScout( double bid );
      void addScout( BWAPI::Unit* unit );
      void updateScoutAssignments();
      void drawAssignments();
      InformationManager* m_informationManager;
      
      size_t m_desiredScoutCount;
      int m_scoutingStartFrame;
      bool m_debugMode;
      static ScoutManager* s_scoutManager;
  };
}