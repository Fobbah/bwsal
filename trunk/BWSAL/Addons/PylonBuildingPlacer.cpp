#include <PylonBuildingPlacer.h>
#include <ReservedMap.h>
#include <Heap.h>
using namespace std;
using namespace BWAPI;
PylonBuildingPlacer* instance = NULL;
PylonBuildingPlacer* PylonBuildingPlacer::getInstance()
{
  if (instance==NULL)
    instance = new PylonBuildingPlacer();
  return instance;
}
PylonBuildingPlacer::PylonBuildingPlacer()
{
  pylonDistance = 3;
}
void PylonBuildingPlacer::onAttach(TaskStream* ts)
{
  taskStreamData[ts].isRelocatable   = true;
  taskStreamData[ts].buildDistance   = 1;
  for each(WorkBench* wb in ts->workBenches)
  {
    if (wb->getCurrentTask() && wb->getCurrentTask()->getTilePosition().isValid() == false)
      wb->getCurrentTask()->setTilePosition(Broodwar->self()->getStartLocation());
    taskStreamData[ts].wbData[wb].reservePosition = Broodwar->self()->getStartLocation();
    taskStreamData[ts].wbData[wb].reserveWidth    = 0;
    taskStreamData[ts].wbData[wb].reserveHeight   = 0;
  }
}
void PylonBuildingPlacer::onDetach(TaskStream* ts)
{
  taskStreamData.erase(ts);
}
void PylonBuildingPlacer::onNewStatus(TaskStream* ts)
{
}
void PylonBuildingPlacer::onCompletedTask(TaskStream* ts, WorkBench* wb, const Task &t)
{
  TheReservedMap->freeTiles(taskStreamData[ts].wbData[wb].reservePosition,taskStreamData[ts].wbData[wb].reserveWidth,taskStreamData[ts].wbData[wb].reserveHeight);
  taskStreamData[ts].wbData[wb].reserveWidth  = 0;
  taskStreamData[ts].wbData[wb].reserveHeight = 0;
}
void PylonBuildingPlacer::onFrame(TaskStream* ts)
{
  /*
  for each(WorkBench* wb in ts->workBenches)
  {
    if (wb->getTask().getType()!=TaskTypes::Unit) continue;
    int width = wb->getTask().getUnit().tileWidth();
    UnitType type = wb->getTask().getUnit();
    if (type.isAddon()) type = type.whatBuilds().first;
    //don't look for a build location if this building requires power and we have no pylons
    if (type.requiresPsi() && Broodwar->self()->completedUnitCount(UnitTypes::Protoss_Pylon)==0) return;
    if (Broodwar->getFrameCount()%10!=0) return;
    if (wb->isLocationReady()==false)
    {
      if (wb->getTask().getTilePosition().isValid()==false)
        wb->getTask().setTilePosition(Broodwar->self()->getStartLocation());
      if (taskStreamData[ts].isRelocatable)
      {
        TilePosition newtp = TilePositions::None;
        int bd = taskStreamData[ts].buildDistance;
        while ( newtp == TilePositions::None)
        {
          newtp = getBuildLocationNear(wb->getWorker(),wb->getTask().getTilePosition(),type,bd);
          bd--;
        }
        wb->getTask().setTilePosition(newtp);
      }
    }
    if (type==BWAPI::UnitTypes::Terran_Command_Center ||
        type==BWAPI::UnitTypes::Terran_Factory || 
        type==BWAPI::UnitTypes::Terran_Starport ||
        type==BWAPI::UnitTypes::Terran_Science_Facility)
    {
      width+=2;
    }
    if (taskStreamData[ts].wbData[wb].reserveWidth    != width ||
        taskStreamData[ts].wbData[wb].reserveHeight   != wb->getTask().getUnit().tileHeight() ||
        taskStreamData[ts].wbData[wb].reservePosition != wb->getTask().getTilePosition())
    {
      TheReservedMap->freeTiles(taskStreamData[ts].wbData[wb].reservePosition,
                                taskStreamData[ts].wbData[wb].reserveWidth,
                                taskStreamData[ts].wbData[wb].reserveHeight);
      taskStreamData[ts].wbData[wb].reserveWidth    = width;
      taskStreamData[ts].wbData[wb].reserveHeight   = wb->getTask().getUnit().tileHeight();
      taskStreamData[ts].wbData[wb].reservePosition = wb->getTask().getTilePosition();
      TheReservedMap->reserveTiles(taskStreamData[ts].wbData[wb].reservePosition, type,
                                   taskStreamData[ts].wbData[wb].reserveWidth,
                                   taskStreamData[ts].wbData[wb].reserveHeight);
    }
  }
  */
}
void PylonBuildingPlacer::setRelocatable(TaskStream* ts, bool isRelocatable)
{
  taskStreamData[ts].isRelocatable = isRelocatable;
}
void PylonBuildingPlacer::setBuildDistance(TaskStream* ts, int distance)
{
  taskStreamData[ts].buildDistance = distance;
}
void PylonBuildingPlacer::setPylonDistance(int pylonDistance)
{
  this->pylonDistance = pylonDistance;
}

BWAPI::TilePosition PylonBuildingPlacer::getBuildLocationNear(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
{
  //returns a valid build location near the specified tile position.
  //searches outward in a spiral.
  if (type.isAddon()) type = type.whatBuilds().first;
  Heap<TilePosition, int> searchHeap(true);
  searchHeap.push(std::make_pair(position,0));
  std::set< TilePosition > closed;
  while (searchHeap.empty()==false)
  {
    TilePosition t = searchHeap.top().first;
    int s          = searchHeap.top().second;
    searchHeap.pop();
    if (this->canBuildHereWithSpace(builder, t, type, buildDist))
      return t;
    closed.insert(t);
    int tx=t.x();
    int ty=t.y();
    int minx = tx-1; if (minx<0) minx=0;
    int maxx = tx+1; if (maxx>=BWAPI::Broodwar->mapWidth()) maxx=BWAPI::Broodwar->mapWidth()-1;
    int miny = ty-1; if (miny<0) miny=0;
    int maxy = ty+1; if (maxy>=BWAPI::Broodwar->mapHeight()) maxy=BWAPI::Broodwar->mapHeight()-1;
    for(int x=minx;x<=maxx;x++)
    {
      for(int y=miny;y<=maxy;y++)
      {
        if (!Broodwar->isWalkable(x*4,y*4)) continue;
        if (!Broodwar->isWalkable(x*4+3,y*4+3)) continue;
        if (!Broodwar->isWalkable(x*4+3,y*4)) continue;
        if (!Broodwar->isWalkable(x*4,y*4+3)) continue;
        TilePosition t2(x,y);
        if (closed.find(t2)==closed.end())
        {
          int ds=10;
          if (x!=tx && y!=ty) ds=14;
          searchHeap.push(std::make_pair(t2,s+ds));
        }
      }
    }
  }
  if (buildDist>0)
    return getBuildLocationNear(builder, position, type, buildDist-1);
  return BWAPI::TilePositions::None;
}

bool PylonBuildingPlacer::canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const
{
  if (type.isAddon()) type=type.whatBuilds().first;
  //returns true if we can build this type of unit here. Takes into account reserved tiles.
  if (!BWAPI::Broodwar->canBuildHere(builder, position, type))
    return false;
  
  int minx = position.x() - pylonDistance; if (minx<0) minx=0;
  int maxx = position.x() + 2 + pylonDistance; if (maxx>=BWAPI::Broodwar->mapWidth()) maxx=BWAPI::Broodwar->mapWidth()-1;
  int miny = position.y() - pylonDistance; if (miny<0) miny=0;
  int maxy = position.y() + 2 + pylonDistance; if (maxy>=BWAPI::Broodwar->mapHeight()) maxy=BWAPI::Broodwar->mapHeight()-1;
  for(int x=minx;x<=maxx;x++)
    for(int y=miny;y<=maxy;y++)
    {
      if (TheReservedMap->getReservedType(x,y)==UnitTypes::Protoss_Pylon)
        return false;
      for each(Unit* u in Broodwar->unitsOnTile(x,y))
        if (u->getPlayer()==Broodwar->self() && u->getType()==UnitTypes::Protoss_Pylon)
          return false;
    }
  return true;
}

bool PylonBuildingPlacer::canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
{
  if (type.isAddon()) type=type.whatBuilds().first;
  //returns true if we can build this type of unit here with the specified amount of space.
  //space value is stored in this->buildDistance.

  //if we can't build here, we of course can't build here with space
  if (!this->canBuildHere(builder,position, type))
    return false;
  if (type.isRefinery())
    return true;

  int width=type.tileWidth();
  int height=type.tileHeight();
  int startx = position.x() - buildDist;
  if (startx<0) return false;
  int starty = position.y() - buildDist;
  if (starty<0) return false;
  int endx = position.x() + width + buildDist;
  if (endx>BWAPI::Broodwar->mapWidth()) return false;
  int endy = position.y() + height + buildDist;
  if (endy>BWAPI::Broodwar->mapHeight()) return false;

  for(int x = startx; x < endx; x++)
    for(int y = starty; y < endy; y++)
      if (!buildable(builder, x, y) || TheReservedMap->isReserved(x,y))
        return false;
  return true;
}

bool PylonBuildingPlacer::buildable(BWAPI::Unit* builder, int x, int y) const
{
  //returns true if this tile is currently buildable, takes into account units on tile
  if (!BWAPI::Broodwar->isBuildable(x,y)) return false;
  std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(x, y);
  for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
    if ((*i)->getType().isBuilding() && !(*i)->isLifted() && !(*i)->getType().isFlyer() && *i != builder)
      return false;
  return true;
}
