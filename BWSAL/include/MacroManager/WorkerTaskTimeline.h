#pragma once
#include <BWAPI.h>
#include <map>
#include <Task.h>

//Timeline of the planned amount of each UnitType
class WorkerTaskTimeline
{
  public:
    WorkerTaskTimeline();
    void reset();
    std::pair<int,int> getFirstFreeInterval(BWAPI::Unit* worker, Task* task, int earliestStartTime = -1);
    bool reserveTime(BWAPI::Unit* worker, int startFrame, Task* task);
    Task* getPlannedTask(BWAPI::Unit* worker, int frame);
  private:
    std::map<BWAPI::Unit*, std::map<int, Task*> > taskStartTimes;
};