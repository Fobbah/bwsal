#pragma once
#include <BWSAL/Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
namespace BWSAL
{
  class BorderManager;
  class DefenseManager : public UnitController, public BWAPI::AIModule
  {
    public:
      static DefenseManager* create( UnitArbitrator* arbitrator, BorderManager* borderManager );
      static DefenseManager* getInstance();
      static void destroy();
      virtual void onOffer( std::set< BWAPI::Unit* > units );
      virtual void onRevoke( BWAPI::Unit* unit, double bid );
      virtual void onFrame();
      virtual std::string getName() const;

    private:
      DefenseManager();
      ~DefenseManager();

      class DefenseData
      {
        public:
          enum DefenseMode
          {
            Idle,
            Moving
          };
          DefenseData(){ mode = Idle; }
          DefenseMode mode;
      };
      BorderManager* m_borderManager;
      UnitArbitrator* m_arbitrator;
      std::set< BWTA::Chokepoint* > m_myBorder;
      std::vector< BWTA::Chokepoint* > m_myBorderVector;
      std::map< BWAPI::Unit*, DefenseData > m_defenders;
      static DefenseManager* s_defenseManager;
  };
}