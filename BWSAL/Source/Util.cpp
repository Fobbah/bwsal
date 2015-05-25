#include <BWSAL/Util.h>
#include <fstream>
#include <stdarg.h>
#include <sys/stat.h>
#include <BWAPI.h>
#include <BWSAL/Task.h>
#include <stdio.h>
namespace BWSAL
{
  void resetLog()
  {
    remove( "bwapi-data\\logs\\BWSAL.log" );
  }
  const int BUFFER_SIZE = 100000;
  char buffer[BUFFER_SIZE];
  char taskBuffer[BUFFER_SIZE];
  void log( const char* text, ... )
  {
    va_list ap;
    va_start( ap, text );
    vsnprintf_s( buffer, BUFFER_SIZE, BUFFER_SIZE, text, ap );
    va_end( ap );

    FILE *outfile;
    // BWAPI::Broodwar->printf( buffer );
    if ( fopen_s( &outfile, "bwapi-data\\logs\\BWSAL.log", "a+" ) == 0 )
    {    
      fprintf_s( outfile, "[%6d] ", BWAPI::Broodwar->getFrameCount() );
      fprintf_s( outfile, buffer );
      fprintf_s( outfile, "\n" );
      fclose( outfile );
    }
  }
  void logTask( Task* t, const char* text, ... )
  {
    va_list ap;
    va_start( ap, text );
    vsnprintf_s( taskBuffer, BUFFER_SIZE, BUFFER_SIZE, text, ap );
    va_end( ap );
    log( "%s %s", t->toString().c_str(), taskBuffer );
  }
  bool resourceDepotIsCompleted( BWAPI::Unit u )
  {
    return ( u->isCompleted() || 
             ( u->getType().isResourceDepot() && u->isMorphing() && u->getBuildType() != BWAPI::UnitTypes::Zerg_Hatchery ) );
  }
  
  void fixName( std::string& name )
  {
    for ( unsigned int j = 0; j < name.length(); ++j )
    {
      if ( name[j] == ' ' )
      {
        name[j] = '_';
      }
      else if ( name[j] >= 'a' && name[j] <= 'z' )
      {
        name[j] += 'A' - 'a';
      }
    }
  }
}