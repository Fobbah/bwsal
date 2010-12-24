#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
class BasicWorkerFinder : public TaskStreamObserver
{
  public:
    virtual void onFrame(TaskStream* ts);
    virtual void onAttach(TaskStream* ts);
  private:
    TaskStream* ts;
};