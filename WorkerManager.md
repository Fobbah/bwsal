Class header file: http://code.google.com/p/bwsal/source/browse/trunk/BWSAL/include/WorkerManager.h

The worker manager ensures that workers are mining minerals and gas at maximum capacity at each base. Transfers workers to new expansions and balances worker amounts across expansions as needed.

  * [update](#update.md)
  * [getName](#getName.md)
  * [getShortName](#getShortName.md)
  * [onRemoveUnit](#onRemoveUnit.md)
  * [setWorkersPerGas](#setWorkersPerGas.md)
  * [getMineralRate](#getMineralRate.md)
  * [getGasRate](#getGasRate.md)
  * [enableAutoBuild](#enableAutoBuild.md)
  * [disableAutoBuild](#disableAutoBuild.md)
  * [setAutoBuildPriority](#setAutoBuildPriority.md)

## update ##
virtual void update();

Should be called during AIModule::onFrame.

## getName ##
virtual std::string getName() const;

Returns std::string("Worker Manager");

## getShortName ##
virtual std::string getShortName() const;

Returns std::string("Work");

## onRemoveUnit ##
void onRemoveUnit(BWAPI::Unit`*` unit);

Should be called during AIModule::onUnitDestroy.

## setWorkersPerGas ##
void setWorkersPerGas(int count);

Used to set how many workers should be used per refinery. The default value is 3 and generally this is the optimal number, but some times in the early game you may want to have only 1 worker on gas.

## getMineralRate ##
double getMineralRate() const;

Returns the rate at which the worker manager is gathering minerals, in minerals per frame.

## getGasRate ##
double getGasRate() const;

Returns the rate at which the worker manager is gathering gas, in gas per frame.

## enableAutoBuild ##
void enableAutoBuild();

Enables auto-build, which automatically builds workers when needed to keep all mining bases gathering at maximum capacity.

## disableAutoBuild ##
void disableAutoBuild();

Disables auto-build.

## setAutoBuildPriority ##
void setAutoBuildPriority(int priority);

Sets the auto-build priority. Default priority level is 80.