Some things to do:

  * Eliminate memory leaks (Tasks, MacroTasks)
  * Switch to smart pointers ( Delete a Task/MacroTask when nothing else points to it )
  * Improve BFSBuildingPlacer
    * Don't build between Command Center and minerals/gas
    * Don't build non-Command Center buildings in base locations
  * Add BuildLocationClearer (clears friendly units out of a build location, given an arbitrator priority)
  * Improve ScoutManager so it continuously tries to find new enemy Bases (currently just finds enemy start location)
  * Update WorkerManager so that it automatically builds the optimal number of workers to saturate all active bases
  * Update WorkerManager so that it automatically changes the number of workers on gas as needed to match demands ( at the very least, take workers off gas if you have more than 1000 gas, and then put them back on when gas drops below 200 ).
  * Update the BuildOrderManager so that it handles all dependencies (currently handles most but not all (i.e. trying to build a Machine Shop when all Factories already have Machine Shops)
  * Update the Task Scheduler and Task Executor to handle Protoss Archon/Dark Archon
  * Implement a type of MacroTask that always tries to queue up as can be started in the next 60 seconds from a given unit Ratio ( Macro Mechanic principle 4)
  * Implement a Production Manager ( completely different from original ProductionManager ) - builds new production buildings as needed, following a desired production type order or overall ratios, so that production rate == gather rate.