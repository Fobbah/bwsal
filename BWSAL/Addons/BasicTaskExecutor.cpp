#include <BasicTaskExecutor.h>
#include <MacroManager.h>
using namespace BWAPI;
BasicTaskExecutor* instance = NULL;
BasicTaskExecutor* BasicTaskExecutor::getInstance()
{
  if (instance==NULL)
    instance = new BasicTaskExecutor();
  return instance;
}
void BasicTaskExecutor::attached(TaskStream* ts)
{
  taskStreams[ts].isReady = false;
  taskStreams[ts].isExecuting = false;
  taskStreams[ts].targetLevel = -1;
}
void BasicTaskExecutor::detached(TaskStream* ts)
{
  taskStreams.erase(ts);
}
void BasicTaskExecutor::newStatus(TaskStream* ts)
{
}
void BasicTaskExecutor::completedTask(TaskStream* ts, const Task &t)
{
}
void BasicTaskExecutor::update(TaskStream* ts)
{
  if (ts == NULL) return;
  if (ts->isWorkerReady()==false) return;
  if (ts->isLocationReady()==false) return;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (ts->getStatus() != TaskStream::Executing_Task)
  {
    if (ts->getTask(0).getStartTime()>=0 &&
        ts->getTask(0).getType()==TaskTypes::Unit &&
        ts->getTask(0).getUnit().isBuilding() &&
        !ts->getTask(0).getWorkerType().isBuilding())
    {
      UnitType ut = ts->getTask(0).getUnit();
      Position targetPosition=Position(ts->getTask(0).getTilePosition());
      targetPosition.x()+=ut.tileWidth()*16;
      targetPosition.y()+=ut.tileHeight()*16;
      if (ts->getTask(0).getStartTime()<Broodwar->getFrameCount()+ts->getWorker()->getDistance(targetPosition)/ts->getTask(0).getWorkerType().topSpeed()*1.4+0.8*30)
      {
        if (worker->getDistance(targetPosition)>100)
        {
          if (Broodwar->getFrameCount()>=ts->getWorker()->getLastOrderFrame()+5)
            worker->rightClick(targetPosition);
        }
      }
    }
    return;
  }
  computeBuildUnit(ts);
  computeIsCompleted(ts);
  Broodwar->drawTextMap(worker->getPosition().x(),worker->getPosition().y()+15,"r: %d, e: %d, c: %d",taskStreams[ts].isReady,taskStreams[ts].isExecuting, ts->getTask(0).isCompleted());
  if (ts->getTask(0).isCompleted()) return;
  computeIsExecuting(ts);
  if (taskStreams[ts].isExecuting) return;
  computeIsReady(ts);
  if (!taskStreams[ts].isReady)
    getReady(ts);
  else
    execute(ts);
}
void BasicTaskExecutor::computeBuildUnit(TaskStream* ts)
{
  Unit* worker = ts->getWorker();
  UnitType ut = ts->getTask(0).getUnit();
  if (ts->getTask(0).getType() != TaskTypes::Unit) return;

  if (worker->getBuildUnit() != NULL && worker->getBuildUnit()->exists() && (worker->getBuildUnit()->getType() == ut || worker->getBuildUnit()->getBuildType() == ut))
    ts->setBuildUnit(worker->getBuildUnit());

  if (worker->getAddon() != NULL && worker->getAddon()->exists() && (worker->getAddon()->getType() == ut || worker->getAddon()->getBuildType() == ut))
    ts->setBuildUnit(worker->getAddon());

  //check to see if the worker is the right type
  //Zerg_Nydus_Canal is speciall since Zerg_Nydus_Canal can construct Zerg_Nydus_Canal
  if ((worker->getType() == ut || worker->getBuildType() == ut) && worker->getType()!=UnitTypes::Zerg_Nydus_Canal)
    ts->setBuildUnit(worker);

  //if the building dies, or isn't the right type, set it to null
  if (!(ts->getBuildUnit() != NULL && ts->getBuildUnit()->exists() && (ts->getBuildUnit()->getType() == ut || ts->getBuildUnit()->getBuildType() == ut)))
    ts->setBuildUnit(NULL);

  if (ts->getBuildUnit()==NULL && ut.isBuilding()) //if we don't have a building yet, look for it
  {
    TilePosition bl = ts->getTask(0).getTilePosition();
    //look at the units on the tile to see if it exists yet
    for each(Unit* u in Broodwar->unitsOnTile(bl.x(), bl.y()))
      if (u->getType() == ut && !u->isLifted())
      {
        //we found the building
        ts->setBuildUnit(u);
        break;
      }
  }
  if (ts->getBuildUnit()==NULL && ut.isAddon()) //if we don't have a building yet, look for it
  {
    TilePosition bl = ts->getTask(0).getTilePosition();
    bl.x()+=4;
    bl.y()++;
    for each(Unit* u in Broodwar->unitsOnTile(bl.x(), bl.y()))
      if (u->getType() == ut && !u->isLifted())
      {
        //we found the building
        ts->setBuildUnit(u);
        break;
      }
  }

}
void BasicTaskExecutor::computeIsExecuting(TaskStream* ts)
{
  taskStreams[ts].isExecuting = false;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (type == TaskTypes::Unit)
  {
    UnitType ut=ts->getTask(0).getUnit();
    if (ts->getBuildUnit()!=NULL && ts->getBuildUnit()->exists() && (ts->getBuildUnit()->getType() == ut || ts->getBuildUnit()->getBuildType() == ut))
      taskStreams[ts].isExecuting = true;
    if (Broodwar->getFrameCount()>ts->getWorker()->getLastOrderFrame()+5)
    {
      if (worker->getType()==UnitTypes::Terran_SCV && worker->isConstructing()==false)
        worker->rightClick(ts->getBuildUnit());
    }
  }
  else if (type == TaskTypes::Tech)
  {
    if (worker->isResearching() && worker->getTech()==ts->getTask(0).getTech())
      taskStreams[ts].isExecuting = true;
  }
  else if (type == TaskTypes::Upgrade)
  {
    if (worker->isUpgrading() && worker->getUpgrade()==ts->getTask(0).getUpgrade())
      taskStreams[ts].isExecuting = true;
  }
  if (taskStreams[ts].isExecuting) ts->getTask(0).setSpentResources(true);
}
void BasicTaskExecutor::computeIsCompleted(TaskStream* ts)
{
  if (ts->getTask(0).isCompleted()) return;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (type == TaskTypes::Unit)
  {
    if (ts->getBuildUnit()!=NULL &&
        ts->getBuildUnit()->exists() &&
        ts->getBuildUnit()->isCompleted() &&
        ts->getBuildUnit()->getType() == ts->getTask(0).getUnit())
    {
      ts->getTask(0).setCompleted(true);
    }
  }
  else if (type == TaskTypes::Tech)
  {
    if (Broodwar->self()->hasResearched(ts->getTask(0).getTech()))
      ts->getTask(0).setCompleted(true);
  }
  else if (type == TaskTypes::Upgrade)
  {
    if (taskStreams[ts].targetLevel>0 && Broodwar->self()->getUpgradeLevel(ts->getTask(0).getUpgrade()) >= taskStreams[ts].targetLevel)
      ts->getTask(0).setCompleted(true);
  }
}
void BasicTaskExecutor::computeIsReady(TaskStream* ts)
{
  if (taskStreams[ts].isReady) return;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  taskStreams[ts].isReady = !(worker->isResearching() || worker->isUpgrading() || worker->isTraining() || worker->isConstructing() || worker->isMorphing());
}
void BasicTaskExecutor::getReady(TaskStream* ts)
{
  if (taskStreams[ts].isReady) return;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (worker->isResearching() || worker->isUpgrading() || worker->isTraining() || worker->isConstructing() || worker->isMorphing())
    worker->stop();
}
void BasicTaskExecutor::execute(TaskStream* ts)
{
  if (ts->getTask(0).isCompleted()) return;
  if (taskStreams[ts].isExecuting) return;
  if (taskStreams[ts].isReady==false) return;
  Unit* worker = ts->getWorker();
  TaskTypes::Enum type = ts->getTask(0).getType();
  if (Broodwar->getFrameCount()<ts->getWorker()->getLastOrderFrame()+5) return;

  if (type == TaskTypes::Unit)
  {
    UnitType ut=ts->getTask(0).getUnit();
    if (ut.getRace()==Races::Zerg && ut.isBuilding()==ut.whatBuilds().first.isBuilding())
    {
      if (worker->morph(ut))
      {
        if (ts->getTask(0).hasSpentResources()==false)
        {
          ts->getTask(0).setSpentResources(true);
          TheMacroManager->spentResources+=ts->getTask(0).getResources();
        }
      }

    }
    else if (ut.isAddon())
    {
      if (worker->isLifted()==false)
      {
        if (worker->getTilePosition()!=ts->getTask(0).getTilePosition())
        {
          worker->lift();
        }
        else
        {
          if (worker->buildAddon(ut))
          {
            if (ts->getTask(0).hasSpentResources()==false)
            {
              ts->getTask(0).setSpentResources(true);
              TheMacroManager->spentResources+=ts->getTask(0).getResources();
            }
          }
        }
      }
      else
      {
        worker->land(ts->getTask(0).getTilePosition());
      }
    }
    else if (ut.isBuilding())
    {
      Position targetPosition=Position(ts->getTask(0).getTilePosition());
      targetPosition.x()+=ut.tileWidth()*16;
      targetPosition.y()+=ut.tileHeight()*16;
      if (worker->getDistance(targetPosition)>100)
        worker->rightClick(targetPosition);
      else
      {
        if (worker->build(ts->getTask(0).getTilePosition(),ut))
        {
          if (ts->getTask(0).hasSpentResources()==false)
          {
            ts->getTask(0).setSpentResources(true);
            TheMacroManager->spentResources+=ts->getTask(0).getResources();
          }
        }
      }
    }
    else
    {
      if (worker->train(ut))
      {
        if (ts->getTask(0).hasSpentResources()==false)
        {
          ts->getTask(0).setSpentResources(true);
          TheMacroManager->spentResources+=ts->getTask(0).getResources();
        }
      }
    }
  }
  else if (type == TaskTypes::Tech)
  {
    if (worker->research(ts->getTask(0).getTech()))
    {
      if (ts->getTask(0).hasSpentResources()==false)
      {
        ts->getTask(0).setSpentResources(true);
        TheMacroManager->spentResources+=ts->getTask(0).getResources();
      }
    }
  }
  else if (type == TaskTypes::Upgrade)
  {
    if (worker->upgrade(ts->getTask(0).getUpgrade()))
    {
      if (ts->getTask(0).hasSpentResources()==false)
      {
        ts->getTask(0).setSpentResources(true);
        taskStreams[ts].targetLevel = Broodwar->self()->getUpgradeLevel(ts->getTask(0).getUpgrade())+1;
        TheMacroManager->spentResources+=ts->getTask(0).getResources();
      }
    }
  }
}