#pragma once
#include <BWAPI.h>

namespace BWSAL
{
  class TaskState
  {
    public:
      TaskState();
      TaskState( int id );
      TaskState( const TaskState& other );
      TaskState& operator=( const TaskState& other );
      operator int() const;
      int getID() const;
      std::string getName() const;
      bool isWaiting() const;
      bool isRunning() const;
      bool isCompleted() const;
    private:
      int id;
  };
  namespace TaskStates
  {
    TaskState getTaskState( std::string name );
    std::set< TaskState >& allTaskStates();
    void init();
    extern const TaskState Not_Scheduled;
    extern const TaskState Tentatively_Scheduled;
    extern const TaskState Aquiring;
    extern const TaskState Preparing;
    extern const TaskState Executing;
    extern const TaskState Warping;
    extern const TaskState Halted;
    extern const TaskState Completed;
    extern const TaskState None;
  }
}