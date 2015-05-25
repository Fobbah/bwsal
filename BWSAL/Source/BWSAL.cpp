#include <BWSAL.h>
#include <BWAPI.h>

namespace BWSAL
{
  void BWSAL_init()
  {
    BuildTypes::init();
    TaskStates::init();
  }
}