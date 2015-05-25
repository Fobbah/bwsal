#include <BWSAL/BuildEvent.h>
#include <sstream>
namespace BWSAL
{
  BuildEvent::BuildEvent( BuildType buildType, int deltaMinerals, int deltaGas, int deltaSupply )
  {
    m_buildType = buildType;
    m_deltaMinerals = deltaMinerals;
    m_deltaGas = deltaGas;
    m_deltaSupply = deltaSupply;
    m_nowUnavailable1.second = NULL;
    m_nowUnavailable2.second = NULL;
    m_nowAvailable1.second = NULL;
    m_nowAvailable2.second = NULL;
    m_completingBuildTypes = 0;
    m_useLarva = NULL;
    m_addonOfUnit = NULL;
  }

  void BuildEvent::set( BuildType buildType, int deltaMinerals, int deltaGas, int deltaSupply )
  {
    m_buildType = buildType;
    m_deltaMinerals = deltaMinerals;
    m_deltaGas = deltaGas;
    m_deltaSupply = deltaSupply;
  }

  BuildEvent& BuildEvent::setBuildUnitUnavailable( BuildType type, BuildUnit* unit )
  {
    if ( m_nowUnavailable1.second == NULL )
    {
      m_nowUnavailable1 = std::make_pair( type, unit );
    }
    else
    {
      m_nowUnavailable2 = std::make_pair( type, unit );
    }
    return *this;
  }

  BuildEvent& BuildEvent::setBuildUnitAvailable( BuildType type, BuildUnit* unit )
  {
    if ( m_nowAvailable1.second == NULL )
    {
      m_nowAvailable1 = std::make_pair( type, unit );
    }
    else
    {
      m_nowAvailable2 = std::make_pair( type, unit );
    }
    return *this;
  }

  BuildEvent& BuildEvent::setCompletedBuildType( BuildType type )
  {
    m_completingBuildTypes |= type.getMask();
    if ( type == BuildTypes::Zerg_Hive || type == BuildTypes::Zerg_Lair )
    {
      m_completingBuildTypes |= BuildTypes::Zerg_Hatchery.getMask();
      m_completingBuildTypes |= BuildTypes::Zerg_Lair.getMask();
    }
    return *this;
  }

  BuildEvent& BuildEvent::useLarva( BuildUnit* unit )
  {
    m_useLarva = unit;
    return *this;
  }

  BuildEvent& BuildEvent::setAddon( BuildUnit* unit )
  {
    m_addonOfUnit = unit;
    return *this;
  }

  std::string BuildEvent::toString() const
  {
    std::stringstream ss;
    ss << "Event for build type " << m_buildType.getName().c_str();
    if ( m_deltaMinerals != 0 )
    {
      ss << ", change in minerals = " << m_deltaMinerals;
    }
    if ( m_deltaGas != 0 )
    {
      ss << ", change in gas = " << m_deltaGas;
    }
    if ( m_deltaSupply != 0 )
    {
       ss << ", delta supply = " << m_deltaSupply;
    }
    if ( m_completingBuildTypes != 0 )
    {
      ss << ", m_completingBuildTypes = " << m_completingBuildTypes;
    }
    if ( m_nowUnavailable1.second != NULL )
    {
      ss << ", Unit " << m_nowUnavailable1.second << "( a " << m_nowUnavailable1.first.getName() << ") is now unavailable";
    }
    if ( m_nowUnavailable2.second != NULL )
    {
      ss << ", Unit " << m_nowUnavailable2.second << "( a " << m_nowUnavailable2.first.getName() << ") is now unavailable";
    }
    if ( m_nowAvailable1.second != NULL )
    {
      ss << ", Unit " << m_nowAvailable1.second << "( a " << m_nowAvailable1.first.getName() << ") is now available";
    }
    if ( m_nowAvailable2.second != NULL )
    {
      ss << ", Unit " << m_nowAvailable2.second << "( a " << m_nowAvailable2.first.getName() << ") is now available";
    }
    return ss.str();
  }

  BuildType BuildEvent::getBuildType() const
  {
    return m_buildType;
  }

  BuildUnit* BuildEvent::getUseLarva() const
  {
    return m_useLarva;
  }

  int BuildEvent::getDeltaMinerals() const
  {
    return m_deltaMinerals;
  }

  int BuildEvent::getDeltaGas() const
  {
    return m_deltaGas;
  }

  int BuildEvent::getDeltaSupply() const
  {
    return m_deltaSupply;
  }

}
