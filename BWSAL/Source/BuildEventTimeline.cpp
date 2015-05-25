#include <BWSAL/BuildEventTimeline.h>
#include <BWSAL/BuildUnit.h>
#include <BWSAL/BuildUnitManager.h>
#include <BWSAL/Util.h>
#include <BWAPI.h>
#include <sstream>
namespace BWSAL
{
  BuildEventTimeline* BuildEventTimeline::s_buildEventTimeline = NULL;

  BuildEventTimeline* BuildEventTimeline::create( BuildUnitManager* buildUnitManager )
  {
    if ( s_buildEventTimeline )
    {
      return s_buildEventTimeline;
    }
    s_buildEventTimeline = new BuildEventTimeline();
    s_buildEventTimeline->m_buildUnitManager = buildUnitManager;
    return s_buildEventTimeline;
  }

  BuildEventTimeline* BuildEventTimeline::getInstance()
  {
    return s_buildEventTimeline;
  }

  void BuildEventTimeline::destroy()
  {
    if ( s_buildEventTimeline )
    {
      delete s_buildEventTimeline;
    }
  }

  BuildEventTimeline::BuildEventTimeline()
  {
  }

  void BuildEventTimeline::initialize()
  {
    m_initialState.createUnclaimedBuildUnits();
  }

  BuildEventTimeline::~BuildEventTimeline()
  {
    s_buildEventTimeline = NULL;
  }

  void BuildEventTimeline::draw()
  {
    BuildState state = m_initialState;
    int now = state.getTime();
    int cumulativeMineralsGathered = (int)state.getMinerals();
    int cumulativeMineralsSpent = 0;
    double vscale = 0.3;
    double hscale = 0.3;

    for ( std::list< std::pair< int, BuildEvent > >::iterator e = m_events.begin(); e != m_events.end(); e++ )
    {
      int time1 = state.getTime();
      int minerals1 = (int)state.getMinerals();
      int cumulativeMineralsGathered1 = cumulativeMineralsGathered;
      state.continueToTime( e->first );
      int time2 = state.getTime();
      int minerals2 = (int)state.getMinerals();
      cumulativeMineralsGathered += minerals2 - minerals1;
      int cumulativeMineralsGathered2 = cumulativeMineralsGathered;
      int cumulativeMineralsSpent1 = cumulativeMineralsSpent;
      state.doEvent( e->second );
      int minerals3 = (int)state.getMinerals();
      cumulativeMineralsSpent -= ( minerals3 - minerals2 );
      int cumulativeMineralsSpent2 = cumulativeMineralsSpent;
      int y0 = 300;
      int x_time1 = (int)( ( time1 - now ) * hscale );
      int x_time2 = (int)( ( time2 - now ) * hscale );

      if ( e->second.getDeltaMinerals() < 0 )
      {
        BWAPI::Broodwar->drawTextScreen( x_time2 + 2, y0 - (int)( vscale * cumulativeMineralsSpent2 ), e->second.getBuildType().getName().c_str(), BWAPI::Colors::White );
        int x_time3 = (int)( ( time2 - now + e->second.getBuildType().builderTime() ) * hscale );
      }
      if ( e->second.getBuildType().whatBuilds().first == BuildTypes::Protoss_Nexus )
      {
        if ( e->second.m_nowUnavailable1.first != NULL )
        {
          BWAPI::Broodwar->drawLineScreen( x_time2 - 1, y0 - (int)( vscale * 50 ), x_time2 - 1, y0, BWAPI::Colors::Red );
        }
        if ( e->second.m_nowAvailable1.first != NULL )
        {
          BWAPI::Broodwar->drawLineScreen( x_time2, y0 - (int)( vscale * 50 ), x_time2, y0, BWAPI::Colors::Green );
        }
      }

      BWAPI::Broodwar->drawLineScreen( x_time1, y0 - (int)( vscale * cumulativeMineralsGathered1 ), x_time2, y0 - (int)( vscale * cumulativeMineralsGathered2 ), BWAPI::Colors::Cyan );
      BWAPI::Broodwar->drawLineScreen( x_time1, y0 - (int)( vscale * cumulativeMineralsSpent1 ), x_time2, y0 - (int)( vscale * cumulativeMineralsSpent1 ), BWAPI::Colors::Green );
      BWAPI::Broodwar->drawLineScreen( x_time2, y0 - (int)( vscale * cumulativeMineralsSpent1 ), x_time2, y0 - (int)( vscale * cumulativeMineralsSpent2 ), BWAPI::Colors::Green );
    }
  }

  class HLHPlanData
  {
    public:
    int currentNextLarvaSpawnTime;
    int currentLarvaCount;
  };

  void drawLarvaChange( int now, int time1, int lc1, int time2, int lc2 )
  {
    double vscale = 5;
    double hscale = 0.3;
    int x_time1 = (int)( ( time1 - now ) * hscale );
    int x_time2 = (int)( ( time2 - now ) * hscale );
    int y0 = 50;
    BWAPI::Broodwar->drawLineScreen( x_time1, y0 - (int)( vscale * lc1 ), x_time2, y0 - (int)( vscale * lc1 ), BWAPI::Colors::Yellow );
    BWAPI::Broodwar->drawLineScreen( x_time2, y0 - (int)( vscale * lc1 ), x_time2, y0 - (int)( vscale * lc2 ), BWAPI::Colors::Yellow );
  }

  void BuildEventTimeline::drawLarvaCounts()
  {
    BuildState state = m_initialState;
    m_buildUnitManager->resetPlanningData();
    int now = state.getTime();
    int lastLarvaChangeTime = now;

    std::map< BuildUnit*, HLHPlanData > hlhPlans;

    int lastLarvaCount = 0;
    for( BuildUnit* bu : m_buildUnitManager->getUnits() )
    {
      if ( bu->getType().getUnitType().producesLarva() )
      {
        lastLarvaCount += bu->m_currentState.m_larvaCount;
        hlhPlans[bu].currentNextLarvaSpawnTime = bu->m_currentState.m_nextLarvaSpawnTime;
        hlhPlans[bu].currentLarvaCount = bu->m_currentState.m_larvaCount;
      }
    }

    // iterate over the timeline
    for ( std::list< std::pair < int, BuildEvent > >::iterator tEvent = m_events.begin(); tEvent != m_events.end(); tEvent++ )
    {
      // continue build state to current time
      state.continueToTime( tEvent->first );
      state.doEvent( tEvent->second );

      // update HLHPlanData for - each HLH
      for ( std::map< BuildUnit*, HLHPlanData >::iterator h = hlhPlans.begin(); h != hlhPlans.end(); h++ )
      {
        // continue current timeline to current time
        while ( h->second.currentNextLarvaSpawnTime <= state.getTime() )
        {
          h->second.currentLarvaCount++;

          drawLarvaChange( now, lastLarvaChangeTime, lastLarvaCount, h->second.currentNextLarvaSpawnTime, h->second.currentLarvaCount );
          lastLarvaChangeTime = h->second.currentNextLarvaSpawnTime;
          lastLarvaCount = h->second.currentLarvaCount;

          h->second.currentNextLarvaSpawnTime += LARVA_SPAWN_TIME;

          // We have three larva, next larva NEVER spawns
          if ( h->second.currentLarvaCount == 3 )
          {
            h->second.currentNextLarvaSpawnTime = NEVER;
          }
        }
      }

      // handle use larva events
      if ( tEvent->second.getUseLarva() != NULL )
      {
        BuildUnit* h = tEvent->second.getUseLarva();
        if ( hlhPlans[h].currentLarvaCount == 3 )
        {
          hlhPlans[h].currentNextLarvaSpawnTime = state.getTime() + LARVA_SPAWN_TIME;
        }
        hlhPlans[h].currentLarvaCount--;
        drawLarvaChange( now, lastLarvaChangeTime, lastLarvaCount, state.getTime(), hlhPlans[h].currentLarvaCount );
        lastLarvaChangeTime = state.getTime();
        lastLarvaCount = hlhPlans[h].currentLarvaCount;
      }
    }
    for ( std::map< BuildUnit*, HLHPlanData >::iterator h = hlhPlans.begin(); h != hlhPlans.end(); h++ )
    {
      // continue current timeline to current time
      while ( h->second.currentNextLarvaSpawnTime < NEVER )
      {
        h->second.currentLarvaCount++;
        drawLarvaChange( now, lastLarvaChangeTime, lastLarvaCount, h->second.currentNextLarvaSpawnTime, h->second.currentLarvaCount );
        lastLarvaChangeTime = h->second.currentNextLarvaSpawnTime;
        lastLarvaCount = h->second.currentLarvaCount;
        h->second.currentNextLarvaSpawnTime += LARVA_SPAWN_TIME;

        // We have three larva, next larva NEVER spawns
        if ( h->second.currentLarvaCount == 3 )
        {
          h->second.currentNextLarvaSpawnTime = NEVER;
        }
      }
    }
    BWAPI::Broodwar->drawLineScreen( 0, 50, 640, 50, BWAPI::Colors::Red );
  }

  std::list< std::pair< int, BuildEvent > >::iterator BuildEventTimeline::addEvent( int time, BuildEvent &e )
  {
    return addEvent( time, e, m_events.begin() );
  }

  std::list< std::pair< int, BuildEvent > >::iterator BuildEventTimeline::addEvent( int time, BuildEvent &e, std::list< std::pair< int, BuildEvent > >::iterator i )
  {
    if ( e.getDeltaSupply() > 0 )
    {
      m_finalSupplyTotal += e.getDeltaSupply();
    }
    else
    {
      m_finalSupplyUsed += -e.getDeltaSupply();
    }
    while ( i != m_events.end() )
    {
      if ( ( *i ).first > time )
      {
        return m_events.insert( i, std::make_pair( time, e ) );
      }
      i++;
    }
    return m_events.insert( i, std::make_pair( time, e ) );
  }

  std::list< std::pair< int, BuildEvent > >::iterator BuildEventTimeline::begin()
  {
    return m_events.begin();
  }

  std::list< std::pair< int, BuildEvent > >::iterator BuildEventTimeline::end()
  {
    return m_events.end();
  }

  void BuildEventTimeline::reset()
  {
    m_initialState.updateWithCurrentGameState();
    m_events.clear();
    m_finalSupplyUsed = m_initialState.getSupplyUsed();
    m_finalSupplyTotal = m_initialState.getSupplyTotal();
  }

  std::string BuildEventTimeline::toString() const
  {
    std::stringstream ss;
    for ( std::list< std::pair<int, BuildEvent> >::const_iterator i = m_events.begin(); i != m_events.end(); i++ )
    {
      ss << "  [" << i->first << "] " << i->second.toString() << "\n";
    }
    return ss.str();
  }
}