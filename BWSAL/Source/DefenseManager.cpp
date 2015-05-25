#include <BWSAL/DefenseManager.h>
#include <BWSAL/BorderManager.h>
namespace BWSAL
{
  DefenseManager* DefenseManager::s_defenseManager = NULL;

  DefenseManager* DefenseManager::create( UnitArbitrator* arbitrator, BorderManager* borderManager )
  {
    if ( s_defenseManager )
    {
      return s_defenseManager;
    }
    s_defenseManager = new DefenseManager();
    s_defenseManager->m_arbitrator = arbitrator;
    s_defenseManager->m_borderManager = borderManager;
    return s_defenseManager;
  }

  DefenseManager* DefenseManager::getInstance()
  {
    return s_defenseManager;
  }

  void DefenseManager::destroy()
  {
    if ( s_defenseManager )
    {
      delete s_defenseManager;
    }
  }

  DefenseManager::DefenseManager()
  {
  }

  DefenseManager::~DefenseManager()
  {
    s_defenseManager = NULL;
  }

  void DefenseManager::onOffer( std::set< BWAPI::Unit > units )
  {
    for( BWAPI::Unit u : units )
    {
      if ( m_defenders.find( u ) == m_defenders.end() )
      {
        m_arbitrator->accept( this, u );
        DefenseData temp;
        m_defenders[u] = temp;
      }
    }
  }

  void DefenseManager::onRevoke( BWAPI::Unit unit, double bid )
  {
    m_defenders.erase( unit );
  }

  void DefenseManager::onFrame()
  {
    // Bid on all completed military units
    for( BWAPI::Unit u : BWAPI::Broodwar->self()->getUnits() )
    {
      if ( u->isCompleted() && 
         !u->getType().isWorker() && 
         !u->getType().isBuilding() &&
          u->getType() != BWAPI::UnitTypes::Zerg_Egg &&
          u->getType() != BWAPI::UnitTypes::Zerg_Larva )
      {
        m_arbitrator->setBid( this, u, 20 );
      }
    }
    bool borderUpdated = false;
    if ( m_myBorder != m_borderManager->getMyBorder() )
    {
      m_myBorder = m_borderManager->getMyBorder();
      m_myBorderVector.clear();
      for( BWTA::Chokepoint* c : m_myBorder )
      {
        m_myBorderVector.push_back( c );
      }
      borderUpdated = true;
    }
    // Order all units to choke
    int i = 0;
    if ( !m_myBorder.empty() )
    {
      // TODO: Update to use computeAssignments
      for ( std::map< BWAPI::Unit, DefenseData >::iterator d = m_defenders.begin(); d != m_defenders.end(); d++ )
      {
        if ( d->second.mode == DefenseData::Idle || borderUpdated )
        {
          BWAPI::Position chokePosition = m_myBorderVector[i]->getCenter();
          i++;
          if ( i >= (int)m_myBorderVector.size() )
          {
            i = 0;
          }
          d->first->attack( chokePosition );
          d->second.mode = DefenseData::Moving;
        }
      }
    }
  }

  std::string DefenseManager::getName() const
  {
    return "Defense Manager";
  }
}