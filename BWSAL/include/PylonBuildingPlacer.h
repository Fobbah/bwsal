#pragma once
#include <MacroManager/TaskStreamObserver.h>
#include <MacroManager/TaskStream.h>
#include <RectangleArray.h>
class PylonBuildingPlacer : public TaskStreamObserver
{
  public:
    static PylonBuildingPlacer* getInstance();
    PylonBuildingPlacer();
    virtual void onAttach(TaskStream* ts);
    virtual void onDetach(TaskStream* ts);
    virtual void onNewStatus(TaskStream* ts);
    virtual void onCompletedTask(TaskStream* ts, WorkBench* wb, const Task &t);
    virtual void onFrame(TaskStream* ts);
    void setRelocatable(TaskStream* ts, bool isRelocatable);
    void setBuildDistance(TaskStream* ts, int distance);
    void setPylonDistance(int pylonDistance);
  private:
    BWAPI::TilePosition getBuildLocationNear(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const;
    bool canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const;
    bool canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const;
    bool buildable(BWAPI::Unit* builder, int x, int y) const;
    struct tsData
    {
      bool isRelocatable;
      int buildDistance;
      struct wdata
      {
        BWAPI::TilePosition reservePosition;
        int reserveWidth;
        int reserveHeight;
      };
      std::map<WorkBench*, wdata> wbData;
    };
    int pylonDistance;
    std::map< TaskStream*, tsData > taskStreamData;

};
