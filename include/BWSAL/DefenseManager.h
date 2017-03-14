#pragma once
#include <BWSAL/Arbitrator.h>
#include <BWAPI.h>
#include <BWTA.h>
namespace BWSAL
{
  class BorderManager;

  /*! \brief The Defense Manager sends all non - worker units to a border assigned by the Border Manager.
   *         The defense manager will bid for ALL non-worker units at priority 20.
   */
  class DefenseManager : public UnitController, public BWAPI::AIModule
  {
    public:
	  //! Create the DefenseManager singleton instance
      static DefenseManager* create( UnitArbitrator* arbitrator, BorderManager* borderManager );

	  //! Return the DefenseManager singleton instance
      static DefenseManager* getInstance();

	  //! Destroy the DefenseManager singleton instance
      static void destroy();

	  //! UnitController notification, the arbitrator has offered these units to the DefenseManager
      virtual void onOffer( std::set< BWAPI::Unit > units );

	  //! UnitController notification, the arbitrator has revoked access to these units from the DefenseManager
      virtual void onRevoke( BWAPI::Unit unit, double bid );

	  //! Should be called every frame from BWAPI::AIModule::onFrame()
      virtual void onFrame();

	  //! Returns the name of this component
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
      std::map< BWAPI::Unit, DefenseData > m_defenders;
      static DefenseManager* s_defenseManager;
  };
}