#pragma once
#include <BWAPI.h>
#include <BWTA.h>
namespace BWSAL
{
  class InformationManager;
  class BorderManager : public BWAPI::AIModule
  {
    public:
      static BorderManager* create( InformationManager* informationManager );
      static BorderManager* getInstance();
      static void destroy();
      virtual void onFrame();
      void addMyBase( BWTA::BaseLocation* location );
      void removeMyBase( BWTA::BaseLocation* location );
      const std::set< BWTA::Chokepoint* >& getMyBorder() const;
      const std::set< BWTA::Chokepoint* >& getEnemyBorder() const;
      const std::set< BWTA::Chokepoint* >& getMyRegions() const;
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