#pragma once
#include <list>
#include <BWSAL/BuildType.h>
#include <BWSAL/BuildEvent.h>
#include <BWSAL/BuildState.h>
#include <BWSAL/Types.h>
namespace BWSAL
{
  class BuildUnitManager;
  class BuildEventTimeline : public BWAPI::AIModule
  {
    public:
      static BuildEventTimeline* create( BuildUnitManager* buildUnitManager );
      static BuildEventTimeline* getInstance();
      static void destroy();
      void initialize();
      void draw();
      void drawLarvaCounts();
      std::list< std::pair< int, BuildEvent > >::iterator addEvent( int time, BuildEvent &e );
      std::list< std::pair< int, BuildEvent > >::iterator addEvent( int time, BuildEvent &e, std::list< std::pair< int, BuildEvent > >::iterator startSearchAt );
      std::list< std::pair< int, BuildEvent > >::iterator begin();
      std::list< std::pair< int, BuildEvent > >::iterator end();
      std::string toString() const;
      void reset();
      BuildState m_initialState;

    private:
      BuildEventTimeline();
      ~BuildEventTimeline();
      int m_finalSupplyUsed;
      int m_finalSupplyTotal;
      std::list< std::pair< int, BuildEvent > > m_events;
      BuildUnitManager* m_buildUnitManager;
      static BuildEventTimeline* s_buildEventTimeline;
  };
}