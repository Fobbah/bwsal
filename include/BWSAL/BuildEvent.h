#pragma once
#include <set>
#include <BWSAL/BuildType.h>
#include <BWSAL/BuildEvent.h>
namespace BWSAL
{
  class BuildUnit;
  class BuildState;
  class BuildEvent
  {
    friend class BuildState;
    friend class BuildEventTimeline;
    public:
      BuildEvent( BuildType buildType, int deltaMinerals = 0, int deltaGas = 0, int deltaSupply = 0 );
      void set( BuildType buildType, int deltaMinerals, int deltaGas, int deltaSupply );
      std::string toString() const;
      BuildEvent& setBuildUnitUnavailable( BuildType type, BuildUnit* unit );
      BuildEvent& setBuildUnitAvailable( BuildType type, BuildUnit* unit );
      BuildEvent& setCompletedBuildType( BuildType type );
      BuildEvent& useLarva( BuildUnit* unit );
      BuildEvent& setAddon( BuildUnit* unit );
      BuildType getBuildType() const;
      int getDeltaMinerals() const;
      int getDeltaGas() const;
      int getDeltaSupply() const;
      BuildUnit* getUseLarva() const;

    private:
      std::pair< BuildType, BuildUnit* > m_nowUnavailable1;
      std::pair< BuildType, BuildUnit* > m_nowUnavailable2;
      std::pair< BuildType, BuildUnit* > m_nowAvailable1;
      std::pair< BuildType, BuildUnit* > m_nowAvailable2;
      int m_deltaMinerals;
      int m_deltaGas;
      int m_deltaSupply;
      BuildType m_buildType;
      BuildUnit* m_useLarva;
      BuildUnit* m_addonOfUnit;
      unsigned int m_completingBuildTypes;
  };
}