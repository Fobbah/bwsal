#pragma once
#include <BWSAL/BuildType.h>
#include <BWSAL/TaskState.h>
#include <BWAPI.h>
#include <BWSAL/BuildEvent.h>
namespace BWSAL
{
  class BuildUnit;
  class MetaUnit;
  class MetaUnitVariable;
  class TaskStream;
  class BuildingPlacer;
  class MacroTask
  {
    std::list< Task* > m_tasks;
    bool isTotal;
    int count;
  }
}