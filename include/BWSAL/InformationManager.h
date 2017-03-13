#pragma once
#include <BWAPI.h>
#include <BWTA.h>
#include <map>
namespace BWSAL
{
  /*! 
   *  \brief Keeps track of what the player has seen, or can logically infer from what it has observed.
   *  
   *  Must be called onFrame, onUnitHide, onUnitEvade and onUnitDestroy to update it's information with what can be seen in the current frame.
   *  Can be used to retrieve information that is constant, but no longer visible, such as the type or last position of a unit that can no longer be seen.
   *  Can also make inferences such as whether the enemy has built a unit and the latest posible time that must have occured
   *  (from directly observed information or logically inferred from that player's tech tree).
   *  Finally, keeps track of all observed player base locations.
   */
  class InformationManager : public BWAPI::AIModule
  {
    public:
	  //! Creates the InformationManager singleton
      static InformationManager* create();

	  //! Retrieve the InformationManager singleton
      static InformationManager* getInstance();

	  //! Destroy the active InformationManager singleton
      static void destroy();

	  //! Must be called every frame during the BWAPI::AIModule onFrame() callback. Updates the information manager with information currently visible to the player. 
	  virtual void onFrame();

	  //! Must be called during the BWAPI::AIModule onUnitDiscover() callback. The newly observable unit is added the InformationManager's list of tracked units
	  virtual void onUnitDiscover(BWAPI::Unit unit) override;

	  //! Should be called during the BWAPI::AIModule onUnitHide() callback.
	  virtual void onUnitHide(BWAPI::Unit unit) override;

	  //! Should be called during the BWAPI::AIModule() onUnitEvade() callback.
	  virtual void onUnitEvade(BWAPI::Unit unit) override;

	  //! Must be called during the BWAPI::AIModule onUnitDestroy() callback. The newly destroyed unit
      virtual void onUnitDestroy( BWAPI::Unit unit );

	  //! Retrieve the last known owner of this unit
      BWAPI::Player getPlayer( BWAPI::Unit unit ) const;

	  //! Retrieve the last known type of this unit
      BWAPI::UnitType getType( BWAPI::Unit unit ) const;

	  //! Retrieve the last observed position of this unit
      BWAPI::Position getLastPosition( BWAPI::Unit unit ) const;

	  //! Retrieve the time (as a frame number) this unit was last observed
      int getLastSeenTime( BWAPI::Unit unit ) const;

	  //! Retrive if (to the best of the information manager's knowledge) this unit still exists
      bool exists( BWAPI::Unit unit ) const;

	  //! Retrieve if the information manager has observed that an enemy has built this unit type, or can infer such from that player's tech tree.
      bool enemyHasBuilt( BWAPI::UnitType type ) const;

	  //! From what the information manager has observed, infer the latest possible time that the enemy must have build this unit type (either from direct observation or inference from that player's tech tree)
      int getBuildTime( BWAPI::UnitType type ) const;

	  //! Retrieve the list of known enemy base locations
      const std::set< BWTA::BaseLocation* >& getEnemyBases() const;

	  //! Set the baselocation as empty, as another module (such as the ScoutManager) has determined that this base definitely does not contain an enemy base
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