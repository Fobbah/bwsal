#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <map>
namespace BWSAL
{
  class InformationManager : public BWAPI::AIModule
  {
    public:
      static InformationManager* create();
      static InformationManager* getInstance();
      static void destroy();
	  virtual void onFrame();
	  virtual void onUnitDiscover(BWAPI::Unit unit) override;
	  virtual void onUnitHide(BWAPI::Unit unit) override;
	  virtual void onUnitEvade(BWAPI::Unit unit) override;
      virtual void onUnitDestroy( BWAPI::Unit unit );
      BWAPI::Player getPlayer( BWAPI::Unit unit ) const;
      BWAPI::UnitType getType( BWAPI::Unit unit ) const;
      BWAPI::Position getLastPosition( BWAPI::Unit unit ) const;
      int getLastSeenTime( BWAPI::Unit unit ) const;
      bool exists( BWAPI::Unit unit ) const;
      bool enemyHasBuilt( BWAPI::UnitType type ) const;
      int getBuildTime( BWAPI::UnitType type ) const;
      const std::set< BWTA::BaseLocation* >& getEnemyBases() const;
      void setBaseEmpty( BWTA::BaseLocation* base );
    private:
      InformationManager();
      ~InformationManager();
      class UnitData
      {
        public:
          UnitData();
          BWAPI::Position m_position;
          BWAPI::UnitType m_type;
          BWAPI::Player m_player;
          int m_lastSeenTime;
          bool m_exists;

		  void update(BWAPI::Unit u);
      };
      void updateBuildTime( BWAPI::UnitType type, int time );
      std::map< BWAPI::Unit, UnitData > m_savedData;
      std::map< BWAPI::UnitType, int > m_buildTime;
      std::set< BWTA::BaseLocation* > m_enemyBases;
      std::map< BWTA::BaseLocation*, BWAPI::Unit > m_enemyBaseCenters;
      std::set< BWTA::BaseLocation* > m_startLocationCouldContainEnemy;
      bool m_scoutedAnEnemyBase;
      static InformationManager* s_informationManager;
  };
}