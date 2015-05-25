#include <BWSAL/MacroTask.h>
namespace BWSAL
{
  MacroTask::MacroTask(BuildType type, int priority, bool isAdditional, int count, BWAPI::TilePosition seedLocation )
    : m_type( type ),
      m_priority( priority ),
      m_isAdditional( isAdditional ),
      m_count( count ),
      m_isCompleted( false ),
      m_seedLocation( seedLocation ),
      m_remainingCount( count )
  {
    if ( isAdditional )
    {
      for( int i = 0; i < count; i++ )
      {
        Task* t = new Task( type );
        t->setSeedLocation( seedLocation );
        m_tasks.push_back( t );
      }
    }
  }

  const std::list< Task* >& MacroTask::getTasks() const
  {
    return m_tasks;
  }

  BuildType MacroTask::getType() const
  {
    return m_type;
  }

  bool MacroTask::isAdditional() const
  {
    return m_isAdditional;
  }

  bool MacroTask::isCompleted() const
  {
    return m_isCompleted;
  }

  int MacroTask::getCount() const
  {
    return m_count;
  }

  int MacroTask::getRemainingCount() const
  {
    return m_remainingCount;
  }

  BWAPI::TilePosition MacroTask::getSeedLocation() const
  {
    return m_seedLocation;
  }

  int MacroTask::getPriority() const
  {
    return m_priority;
  }

  int MacroTask::computeRemainingCount()
  {
    m_remainingCount = 0;
    for( Task* t : m_tasks )
    {
      if ( t->isWaiting() && !t->isScheduledThisFrame() )
      {
        m_remainingCount++;
      }
    }
    return m_remainingCount;
  }
  
  void MacroTask::setRemainingCount( int remainingCount )
  {
    int m_remainingCount = computeRemainingCount();
    if ( m_remainingCount > remainingCount )
    {
      std::list< Task* >::iterator i = m_tasks.begin();
      std::list< Task* >::iterator i2 = i;
      for( ; i != m_tasks.end(); i = i2 )
      {
        i2++;
        if ( !(*i)->isScheduledThisFrame() && (*i)->isWaiting() )
        {
          m_tasks.erase( i );
          m_remainingCount--;
          if ( m_remainingCount <= remainingCount )
          {
            break;
          }
        }
      }
    }
    else
    {
      while ( m_remainingCount < remainingCount )
      {
        Task* t = new Task( getType() );
        t->setSeedLocation( getSeedLocation() );
        m_tasks.push_back( t );
        m_remainingCount++;
      }
    }
  }

  Task* MacroTask::getNextUnscheduledTask() const
  {
    for( Task* t : m_tasks)
    {
      if ( t->isWaiting() && t->isScheduledThisFrame() == false )
      {
        return t;
      }
    }
    return NULL;
  }

  int MacroTask::getWaitingCount() const
  {
    int waitingCount = 0;
    for( Task* t : m_tasks )
    {
      if ( t->isWaiting() )
      {
        waitingCount++;
      }
    }
    return waitingCount;
  }
  
  int MacroTask::getIncompleteCount() const
  {
    int incompleteCount = 0;
    for( Task* t : m_tasks )
    {
      if ( !t->isCompleted() )
      {
        incompleteCount++;
      }
    }
    return incompleteCount;
  }
}