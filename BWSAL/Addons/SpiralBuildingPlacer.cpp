#include <SpiralBuildingPlacer.h>
#include <ReservedMap.h>
using namespace BWAPI;
SpiralBuildingPlacer* instance = NULL;
SpiralBuildingPlacer* SpiralBuildingPlacer::getInstance()
{
  if (instance==NULL)
    instance = new SpiralBuildingPlacer();
  return instance;
}
SpiralBuildingPlacer::SpiralBuildingPlacer()
{
}
void SpiralBuildingPlacer::onAttach(TaskStream* ts)
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
void SpiralBuildingPlacer::onDetach(TaskStream* ts)
{
  taskStreamData.erase(ts);
}
void SpiralBuildingPlacer::onNewStatus(TaskStream* ts)
{
}
void SpiralBuildingPlacer::onCompletedTask(TaskStream* ts, WorkBench* wb, const Task &t)
{
  TheReservedMap->freeTiles(taskStreamData[ts].wbData[wb].reservePosition,taskStreamData[ts].wbData[wb].reserveWidth,taskStreamData[ts].wbData[wb].reserveHeight);
  taskStreamData[ts].wbData[wb].reserveWidth  = 0;
  taskStreamData[ts].wbData[wb].reserveHeight = 0;
}
void SpiralBuildingPlacer::onFrame(TaskStream* ts)
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
void SpiralBuildingPlacer::setRelocatable(TaskStream* ts, bool isRelocatable)
{
  taskStreamData[ts].isRelocatable = isRelocatable;
}
void SpiralBuildingPlacer::setBuildDistance(TaskStream* ts, int distance)
{
  taskStreamData[ts].buildDistance = distance;
}

BWAPI::TilePosition SpiralBuildingPlacer::getBuildLocationNear(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
{
  //returns a valid build location near the specified tile position.
  //searches outward in a spiral.
  if (type.isAddon()) type = type.whatBuilds().first;
  int x      = position.x();
  int y      = position.y();
  int length = 1;
  int j      = 0;
  bool first = true;
  int dx     = 0;
  int dy     = 1;
  int max    = BWAPI::Broodwar->mapWidth()*2;
  if (BWAPI::Broodwar->mapHeight()*2 > max)
    max = BWAPI::Broodwar->mapHeight()*2;

  while (length < max ) //We'll ride the spiral to the end
  {
    //if we can build here, return this tile position
    if (x >= 0 && x < BWAPI::Broodwar->mapWidth() && y >= 0 && y < BWAPI::Broodwar->mapHeight())
      if (this->canBuildHereWithSpace(builder, BWAPI::TilePosition(x, y), type, buildDist))
        return BWAPI::TilePosition(x, y);

    //otherwise, move to another position
    x = x + dx;
    y = y + dy;
    //count how many steps we take in this direction
    j++;
    if (j == length) //if we've reached the end, its time to turn
    {
      //reset step counter
      j = 0;

      //Spiral out. Keep going.
      if (!first)
        length++; //increment step counter if needed

      //first=true for every other turn so we spiral out at the right rate
      first =! first;

      //turn counter clockwise 90 degrees:
      if (dx == 0)
      {
        dx = dy;
        dy = 0;
      }
      else
      {
        dy = -dx;
        dx = 0;
      }
    }
    //Spiral out. Keep going.
  }
  if (buildDist>0)
    return getBuildLocationNear(builder, position, type, buildDist-1);
  return BWAPI::TilePositions::None;
}

bool SpiralBuildingPlacer::canBuildHere(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type) const
{
  if (type.isAddon()) type=type.whatBuilds().first;
  //returns true if we can build this type of unit here. Takes into account reserved tiles.
  if (!BWAPI::Broodwar->canBuildHere(builder, position, type))
    return false;
  for(int x = position.x(); x < position.x() + type.tileWidth(); x++)
    for(int y = position.y(); y < position.y() + type.tileHeight(); y++)
      if (TheReservedMap->isReserved(x,y))
        return false;
  return true;
}

bool SpiralBuildingPlacer::canBuildHereWithSpace(BWAPI::Unit* builder, BWAPI::TilePosition position, BWAPI::UnitType type, int buildDist) const
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

  //make sure we leave space for add-ons. These types of units can have addons:
  if (type==BWAPI::UnitTypes::Terran_Command_Center ||
    type==BWAPI::UnitTypes::Terran_Factory || 
    type==BWAPI::UnitTypes::Terran_Starport ||
    type==BWAPI::UnitTypes::Terran_Science_Facility)
  {
    width+=2;
  }
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

  if (position.x()>3)
  {
    int startx2=startx-2;
    if (startx2<0) startx2=0;
    for(int x = startx2; x < startx; x++)
      for(int y = starty; y < endy; y++)
      {
        std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(x, y);
        for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
        {
          if (!(*i)->isLifted() && *i != builder)
          {
            BWAPI::UnitType type=(*i)->getType();
            if (type==BWAPI::UnitTypes::Terran_Command_Center ||
              type==BWAPI::UnitTypes::Terran_Factory || 
              type==BWAPI::UnitTypes::Terran_Starport ||
              type==BWAPI::UnitTypes::Terran_Science_Facility)
            {
              return false;
            }
          }
        }
      }
  }
  return true;
}

bool SpiralBuildingPlacer::buildable(BWAPI::Unit* builder, int x, int y) const
{
  //returns true if this tile is currently buildable, takes into account units on tile
  if (!BWAPI::Broodwar->isBuildable(x,y)) return false;
  std::set<BWAPI::Unit*> units = BWAPI::Broodwar->unitsOnTile(x, y);
  for(std::set<BWAPI::Unit*>::iterator i = units.begin(); i != units.end(); i++)
    if ((*i)->getType().isBuilding() && !(*i)->isLifted() && !(*i)->getType().isFlyer() && *i != builder)
      return false;
  return true;
}
