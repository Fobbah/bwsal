#pragma once
#include <BWAPI.h>
#include <BWTA.h>
namespace BWSAL
{
  class InformationManager;

  /*! \brief A border is a set of chokepoints that divides the map into two or more parts. The Border Manager can be used both to get the border around our base (i.e. for defense), and also for getting the border around the enemy's base (i.e. for containment).
   */
  class BorderManager : public BWAPI::AIModule
  {
    public:
      static BorderManager* create( InformationManager* informationManager );
      static BorderManager* getInstance();
      static void destroy();

	  //! Should be called every frame from within BWAPI::AIModule onFrame()
      virtual void onFrame();

	  //! Call when you are about to expand to the given base location.
      void addMyBase( BWTA::BaseLocation* location );

	  //! Call when you are abandoning a base at the given base location.
      void removeMyBase( BWTA::BaseLocation* location );

	  //! Returns the border around our base. Useful for defending against ground attacks.
      const std::set< BWTA::Chokepoint* >& getMyBorder() const;

	  //! Returns the border around the enemy base. Useful for containing the enemy in his base.
      const std::set< BWTA::Chokepoint* >& getEnemyBorder() const;

	  //! Returns the set of regions controlled by our player.
      const std::set< BWTA::Chokepoint* >& getMyRegions() const;

	  //! Returns the set of regions controlled by the enemy.
      const std::set< BWTA::Chokepoint* >& getEnemyRegions() const;
      void draw();
    private:
      BorderManager();
      ~BorderManager();
      void recalculateBorders();
      InformationManager* m_informationManager;
      std::set< BWTA::BaseLocation* > m_myBases;
      std::set< BWTA::BaseLocation* > m_enemyBases;
      std::set< BWTA::Region* > m_myRegions;
      std::set< BWTA::Region* > m_enemyRegions;
      std::set< BWTA::Chokepoint* > m_myBorder;
      std::set< BWTA::Chokepoint* > m_enemyBorder;
      static BorderManager* s_borderManager;
  };
}