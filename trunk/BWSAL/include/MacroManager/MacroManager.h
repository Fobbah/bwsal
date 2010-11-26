#pragma once
#include <BWAPI.h>
#include <Arbitrator.h>
#include <ResourceTimeline.h>
#include <UnitCountTimeline.h>
#include <TechTimeline.h>
#include <UpgradeTimeline.h>
#include <Task.h>
class TaskStream;
class MacroManager
{
  public:
    static MacroManager* create(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator);
    MacroManager(Arbitrator::Arbitrator<BWAPI::Unit*,double>* arbitrator);
    ~MacroManager();
    void update();
    bool insertTaskStreamAbove(TaskStream* newTS, TaskStream* existingTS);
    bool insertTaskStreamBelow(TaskStream* newTS, TaskStream* existingTS);
    bool swapTaskStreams(TaskStream* a, TaskStream* b);
    TaskStream* getTaskStream(BWAPI::Unit* unit) const;
    std::list< TaskStream* > taskStreams;
    std::set< TaskStream* > killSet;
    ResourceTimeline rtl;
    UnitCountTimeline uctl;
    TechTimeline ttl;
    UpgradeTimeline utl;
    Resources spentResources;
    std::map< BWAPI::Unit*, TaskStream* > unitToTaskStream;
    std::map< int, std::list< std::pair<TaskStream*, Task > > > plan;
    bool taskstream_list_visible;
};
extern MacroManager* TheMacroManager;
extern Arbitrator::Arbitrator<BWAPI::Unit*,double>* TheArbitrator;