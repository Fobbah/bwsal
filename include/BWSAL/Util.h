#include <BWSAL/RectangleArray.h>
#include <BWAPI.h>
#include <map>
#include <algorithm>
namespace BWSAL
{
  class Task;
  void resetLog();
  void log( const char* text, ... );
  void logTask( Task* t, const char* text, ... );
  std::map< int, int >& computeAssignments( RectangleArray< double > &cost );
  bool resourceDepotIsCompleted( BWAPI::Unit* u );
  void fixName( std::string& name );

  #ifndef max
    #define max std::max
  #endif
  #ifndef min
    #define min std::min
  #endif
}