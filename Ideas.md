Ideas for new BWSAL functionality, in no particular order

# Macro Manager #
The Macro Manager would be a more modular and extensible approach to macro management. This would replace the build order manager, build manager, construction manager, production manager, morph manager, tech manager, and upgrade manager.

The Macro Manager could also be called the Task Stream Manager as it would maintain/manage a list of Task Streams, ordered by priority in descending order. Similar to the Build Order Manager, the Macro Manager would first try to execute the highest priority Task Stream, and then attempt to execute the second-highest Task Stream if doing so would not delay the highest Task Stream, etc. Unlike the Build Order Manager, however, there would usually be at most one Task Stream per production facility. The Task Streams would be editable and cancel-able (unlike orders given to the Build Order Manager). Also if a Task Stream was marked as 'Urgent', the Macro Manager may cancel active tasks of lower priority so as to begin execution of the urgent Task Stream as soon as possible.

The classes for a Task, Task Stream, and Task Stream Observer would look something like this:

```
class Task
{
  UnitType a;
  TechType b;
  UpgradeType c;
  TilePosition t;
}

class TaskStream
{
  public:
  void attach(TaskStreamObserver* o);
  void detach(TaskStreamObserver* o);
  void notifyObservers();
  enum Status
  {
    Executing_Task,
    Error_Worker_Not_Specified,
    Error_Task_Not_Specified,
    Error_Location_Not_Specified,
    Error_Location_Unreachable,
    Error_Location_Blocked,
    Error_Task_Requires_Addon,
    Waiting_For_Minerals,
    Waiting_For_Gas,
    Waiting_For_Supply,
    Waiting_For_RequiredBuildings
  }
  Status getStatus() const;

  void setWorker(Unit* w);
  Unit* getWorker() const;

  void setCurrentTask(Task t);
  Task getCurrentTask() const;

  void setUrgent(bool isUrgent);
  bool isUrgent() const;
  void setNextTask(Task* t);
  Task getNextTask() const;

  void setName(String s);
  String getName() const;

  //perhaps an arbitrarily long list of tasks?
  
}

class TaskStreamObserver
{
  public:
  receiveTaskStreamStatus(TaskStream* fromStream, Status s);
  receiveTaskStreamCompletion(TaskStream* fromStream, Task completedTask);
}


```

A Task object represents any sort of Macro-management task that can be executed, including training a unit, morphing a unit, constructing a building (with a field to specify build location), researching a technology, and upgrading an upgrade.

A Task Stream object maintains a Status, a current Task, a next Task, a Worker unit, and an Urgent flag:

  * Status - The status field will indicate if the task is executing, waiting for some condition to occur (like sufficient minerals), or has an error which needs to be resolved by a TaskStreamObserver (like the worker unit died, or the build location is blocked).
  * Current Task - the Task that this TaskStream is current trying to execute.
  * Next Task - the next Task that this Task Stream will immediately try to execute as soon as the current task is done.
  * Worker - the worker that is assigned to this task stream and will be used to execute the task. If the MacroManager cannot get ownership of the worker unit from the Arbitrator, the worker field will be reset to NULL and the status of the TaskStream will be set to ErrorWorkerNotSpecified.
  * Urgent flag - If a Task Stream is marked as Urgent, the Macro Manager may cancel lower priority Task Streams if doing so would enable the Urgent Task Stream to begin.

Two examples of how to use the Urgent flag:

Example A: Say the AI scouts that the opponent is getting Dark Templar, and suddenly as an urgent need to get a Comsat Station add-on. The AI could add a Task Stream for building a Comsat Station to the top of the Task Stream list and mark it as Urgent. Since the Comsat Station Task Stream has a higher priority than the SCV Pump Task Stream, the Macro Manager would cancel any SCV that might be in production, and then start building the Comsat Station. If the Comsat Station Task Stream was not marked as urgent in this scenario, the Macro Manager would wait for the SCV to complete before building the Comsat Station.

Example B: Say the AI scouts that the opponent is getting Mutalisks, and thus wants to build an E-bay as fast as possible, but does not want to cancel or delay its SCV production, but is willing to cancel/delay some lower priority Task Streams that are pumping marines. In this case, the AI could add an urgent Task Stream for building the E-bay and insert it right below the Task Stream for producing SCVs. If the AI did not have enough money to immediately begin producing the E-bay, but did have some Marines in production, the AI would cancel some of those Marines so it would have enough money to begin construction on the E-bay. The SCV Production Task Stream would be unaffected by the Urgent E-bay Task Stream as it occurs above the Urgent E-bay Task Stream in the list of Task Streams.

Once the current task in a Task Stream is done, the next task is moved into the current task field and the field for next task is cleared/emptied.

From the MacroManager class, there would be functions to observe the list of Task Streams, modify the order of Task Streams, and add or remove Task Streams.

Also, any class that implements the TaskStreamObserver class could attach itself to a TaskStream, and would then get pinged whenever the status of the TaskStream changes.

Some basic TaskStreamObservers that could be created/used:

Basic Worker Finder - When this Task Stream Observer is pinged by a Task Stream with a status of Error\_Worker\_Not\_Specified, it finds a suitable worker for the Task Stream based on the type of unit needed to execute the current Task in the Task Stream. When multiple workers are found, the Basic Worker Finder prefers workers that are closer to the build location of the current task in the task stream.

Basic Building Placer - When this Task Stream Observer is pinged by a Task Stream with a status of Error\_Location\_Not\_Specified, Error\_Location\_Unreachable, or Error\_Location\_Blocked, it finds a new place to build the building by looking in an outward search from its current location.

Basic Task Stream Terminator - When this Task Stream Observer is pinged by a Task Stream with a status of Error\_Task\_Not\_Specified, it will terminate the task stream.

Basic Worker Terminator - When this Task Stream Observer is pinged by a Task Stream with a status of Error\_Worker\_Not\_Specified it will terminate the task stream.

Example of how to use Task Streams and Task Stream Observers:

  * Say the Base Manager wants to expand to a BaseLocation at (137,42). The BaseManager could create a new TaskStream for this task, attach itself to this TaskStream as an Observer, set the current Task in the task stream to "Build Nexus at (137,42)", set the next Task in the task stream to "Build Pylon at (137,45)", and then insert the TaskStream into the Macro Manager's TaskStream list at the desired location depending on the priority of expanding. For example if you don't want the act of expanding to halt or delay army production, the TaskStream for expanding should be below all the task streams responsible for army production. The BaseManager doesn't really care what probe is used to complete this task, so the BaseManager would also attach the Basic Worker Finder as an observer to this Task Stream, which would find a free nearby worker to use for the Task Stream. However the BaseManager would NOT attach the Basic Building Placer because it doesn't want the build location to change if the original build location of (137,42) is blocked. Instead it will handle the Error\_Location\_Blocked status message itself (perhaps by telling the army manager to destroy whatever is blocking it). Once both tasks in the task stream are done, the task stream should automatically terminate so the Base Manager could also attach the Basic Task Stream Terminator to the task stream.

  * Say the Supply Manager determines that it is time to make a new supply depot. The Supply Manager would create a new Task Stream for this task, attach itself to this Task Stream as an observer, set the current Task in the task stream to "Build Supply Depot at TilePosition::None", and then insert the Task Stream into the top of the Macro Manager's Task Stream list. The Supply Manager would also attach the Basic Building Placer, Basic Worker Finder, and Basic Task Stream Terminator as observers to this Task Stream.

  * Say you want to pump Marines out of one of your barracks. You could make a basic unit pump observer that always sets the current task and next task to "Train Marine", and then attach this observer to a new task stream. In this new task stream you set the worker to the barracks you want to pump marines out of. You could also then decide to attach the Basic Worker Terminator, or the Basic Worker Finding to the task stream depending on if you want the task stream of pumping marines to terminate if the barracks gets destroyed, or continue with a different barracks if the first gets destroyed.

Example of a possible Task Stream list:

| Task Stream Name | Worker ID | Current Task | Next Task | Observers |
|:-----------------|:----------|:-------------|:----------|:----------|
| Pump SCVs        | 6 (a command center) | Train SCV    | Train SCV | Unit Pump, Basic Worker Terminator |
| Unit Composition | 7 (a barracks) | Train Marine | Train Medic | Unit Composition Manager, Basic Worker Terminator |
| Unit Composition | 8 (another barracks) | Train Marine | Train Marine | Unit Composition Manager, Basic Worker Terminator |
| Unit Composition | 9 (another barracks) | Train Medic  | Train Marine | Unit Composition Manager, Basic Worker Terminator |
| Make Factory     | 3 (an scv) | Build Factory | None      | Basic Worker Finder, Basic Building Placer, Basic Task Stream Terminator |


# Threat Manager #
Will create a virtual "heatmap" of danger it will encounter if it moves to that position. Can be used to route units through the path with minimum danger. Takes into account tech levels. For example, a Dark Templar will have 0 threat against Zerg where the Overlords cannot see. Against Protoss it will generally have a small baselevel threat due to the possibility of hidden enemy Observers. Dropships will route themselves through areas where they cannot be seen (edge of map, elevated unaccessible territory, etc). Dragoons will prefer to maneuver with an Observor to negate the threat of mines. A full microing solution will have to take into account temporal elements, so this is just a stand-in or foundation for a better solution.

# Force Manager #

Not a controller.

A simple manager that can implement basic AI functionality for attacking. A force manager can be used to determine what groups a player's or enemy's units consist of, and can choose a strategy of where to position the player's units. We can use it currently to assign ScoutManager the positions it needs to scout and when. We can also use it to assign DefenseManager a new chokepoint to defend in the case that an expansions is made or lost. This way we can move strategic logic out of ScoutManager (and I would say DefenseManager, but it doesn't even have that =) to pave the way towards fixing it. Later we can split it up into a more complex array of managers, but for now we can use it to implement basic defend, retreat, or later, attack functionality.

# Scout Manager #

Improve the scout manager so that it tries to stay alive as long as possible inside the enemy base once it finds the enemy base

Might be possible to split the scout manager into 3 overall tasks or goals:

1) Scout empty base locations to check for new enemy bases

2) Scout enemy bases for tech buildings and production capabilities

3) Scout enemy army for size, unit composition, and position

The first two tasks usually occur at specific points in time (i.e. scout on frame X to see if a Spire or Hydralisk Den was constructed) and might be possible to plan in advance, while the third task is more of a continuous process that starts as soon as the first military unit is created.

The Scout Manager does not determine how important each of these tasks are, or how much units to use. Rather the AI tells the scout manager this information. The role of the scout manager is to scout based on the importance levels (i.e. find the best way to enter and scout the enemy base, given the units it has).

# Evasion Tool #

Primary usage: To be used by scouting manager to keep scouting unit(s) alive.

Inputs: Friendly unit to control, set of enemy units to evade.

To keep the initial implementation simple, we can assume enemy units all move at the same speed and move at the speed of the friendly unit.

Output: Set of tile positions, any of which will let the unit survive as long as possible, assuming perfect chase behavior by enemy units. AI can then pick which tile to order the evading unit to move to.

Algorithm:
Two simultaneous breadth-first searches - one starting from friendly unit (call this search BFS1) and one starting from all enemy units (call it BFS2) (just insert the tile position of each enemy unit into heap). Expand each BFS at same movement speed so tiles X tiles from friendly unit in BFS1 are considered at the same time as tiles X tiles from enemy units in BFS2.

If a tile in BFS1 has already been reached by BFS2, don't expand it (handle it as an unwalkable tile or something). This tile can be reached by the enemy first so the evading unit cannot consider it as a possibility.

Let A = Tiles reached by BFS1. Let B = tiles reached by BFS2. Once A - B = 0, backstep 1 iteration to consider the last tiles reached by BFS1 but not BFS2.

Return this set of tiles.

# Chase Tool #

Primary usage: For killing enemy scouting units.

Inputs: Friendly units to control, enemy unit to chase.

To keep the initial implementation simple, we can assume all friendly units all move at the same speed and move at the speed of the enemy unit.

Output: A tile position for each friendly unit that is adjacent to the unit's current location which will move units in correct direction for chase/containment of enemy unit.

Algorithm:
Not sure how best to approach this problem. 1 possible solution is to consider all possible adjacent tiles for each friendly unit (9^n cases), and run the evasion algorithm for each case, then pick the case that results in the minimum life-span of enemy unit.

# Expected Damage Calculator #
Compute expected average damage the enemy can inflict per frame on each tile. Update incrementally so the game doesn't slow to a crawl.

# Risk Adverse Path Planner #

Given set of fixed-location enemy units, start location A, goal location B, Plan path from A to B using A`*`. Give extra cost to tiles in range of enemy units based on data from the Expected Damage Calculator.

# Other Ideas #

  * Improve the Supply Manager so it uses a more precise algorithm (currently just uses a simple heuristic)
  * Attack Manager - attack stuff, maybe break down into smaller managers
  * Contain Manager - contains the enemy with siege tanks, lurkers, etc.
  * Repair Manager - repair units that are damaged
  * Threat Assessment Manager - Assign a priority to units that are attacking our base.
  * Transport Manager - transports units to a given position, perhaps with a priority, using the risk adverse path planner to find safest paths

  * Add a function to the building placer which takes a choke point and list of building unit types (possibly with repetitions) and returns a list of unit type - build position pairs that can be used to block off the choke point.