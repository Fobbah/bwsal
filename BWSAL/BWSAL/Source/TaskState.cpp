#include <BWSAL/TaskState.h>
#include <BWSAL/Util.h>
#include <BWAPI.h>
#include <algorithm>
namespace BWSAL
{
  std::map< std::string, TaskState > taskStateMap;
  std::set< TaskState > taskStateSet;
  bool initializingTaskState = true;
  class TaskStateInternal
  {
    public:
      TaskStateInternal()
      {
        valid = false;
      }
      void set( std::string name, bool isWaiting, bool isRunning, bool isCompleted )
      {
        if ( initializingTaskState )
        {
          this->name = name;
          this->isWaiting = isWaiting;
          this->isRunning = isRunning;
          this->isCompleted = isCompleted;
          this->valid = true;
        }
      }
      std::string name;
      bool isWaiting;
      bool isRunning;
      bool isCompleted;

      bool valid;
  };
  TaskStateInternal taskStateData[9];
  namespace TaskStates
  {
    const TaskState Not_Scheduled( 0 );
    const TaskState Tentatively_Scheduled( 1 );
    const TaskState Aquiring( 2 );
    const TaskState Preparing( 3 );
    const TaskState Executing( 4 );
    const TaskState Warping( 5 );
    const TaskState Halted( 6 );
    const TaskState Completed( 7 );
    const TaskState None( 8 );
    void init()
    {
      taskStateData[Not_Scheduled].set( "Not Scheduled", 1, 0, 0 );
      taskStateData[Tentatively_Scheduled].set( "Tentatively Scheduled", 1, 0, 0 );
      taskStateData[Aquiring].set( "Aquiring", 0, 1, 0 );
      taskStateData[Preparing].set( "Preparing", 0, 1, 0 );
      taskStateData[Executing].set( "Executing", 0, 1, 0 );
      taskStateData[Warping].set( "Warping", 0, 1, 0 );
      taskStateData[Halted].set( "Halted", 0, 1, 0 );
      taskStateData[Completed].set( "Completed", 0, 0, 1 );
      taskStateData[None].set( "None", 0, 0, 0 );
      taskStateSet.insert( Not_Scheduled );
      taskStateSet.insert( Tentatively_Scheduled );
      taskStateSet.insert( Aquiring );
      taskStateSet.insert( Preparing );
      taskStateSet.insert( Executing );
      taskStateSet.insert( Warping );
      taskStateSet.insert( Halted );
      taskStateSet.insert( Completed );
      taskStateSet.insert( None );
      for( TaskState i : taskStateSet )
      {
        std::string name = i.getName();
        fixName( name );
        taskStateMap.insert( std::make_pair( name, i ) );
      }
      initializingTaskState = false;

    }
  }

  TaskState::TaskState()
  {
    this->id = TaskStates::None;
  }

  TaskState::TaskState( int id )
  {
    this->id = id;
    if ( !initializingTaskState && ( id < 0 || id >= 9 || !taskStateData[id].valid ) )
      this->id = TaskStates::None;
  }

  TaskState::TaskState( const TaskState& other )
  {
    this->id = other;
  }

  TaskState& TaskState::operator= ( const TaskState& other )
  {
    this->id = other;
    return *this;
  }

  TaskState::operator int() const
  {
    return this->id;
  }

  int TaskState::getID() const
  {
    return this->id;
  }

  std::string TaskState::getName() const
  {
    return taskStateData[this->id].name;
  }

  bool TaskState::isWaiting() const
  {
    return taskStateData[this->id].isWaiting;
  }

  bool TaskState::isRunning() const
  {
    return taskStateData[this->id].isRunning;
  }

  bool TaskState::isCompleted() const
  {
    return taskStateData[this->id].isCompleted;
  }

  TaskState TaskStates::getTaskState( std::string name )
  {
    fixName( name );
    std::map< std::string, TaskState >::iterator i = taskStateMap.find( name );
    if ( i == taskStateMap.end() )
      return TaskStates::None;
    return ( *i ).second;
  }

  std::set< TaskState >& TaskStates::allTaskStates()
  {
    return taskStateSet;
  }
}