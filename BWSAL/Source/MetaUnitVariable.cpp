#include <BWSAL/MetaUnitVariable.h>

namespace BWSAL
{
  MetaUnitVariable::MetaUnitVariable()
  {
    m_metaUnit = NULL;
  }

  BuildUnit* MetaUnitVariable::getBuildUnit()
  {
    if ( m_metaUnit != NULL )
    {
      return m_metaUnit->getBuildUnit();
    }
    return NULL;
  }

  BuildType MetaUnitVariable::getType() const
  {
    if ( m_metaUnit != NULL )
    {
      return m_metaUnit->getType();
    }
    return BuildTypes::None;
  }

  void MetaUnitVariable::assign( MetaUnit* metaUnit )
  {
    m_metaUnit = metaUnit;
  }
}