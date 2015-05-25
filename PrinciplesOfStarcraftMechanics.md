# Priciples of Starcraft Mechanics #

Lowerlogic's principles of Starcraft Mechanics (hopefully to be implemented using BWSAL some day)

## Principles of Macro Mechanics ##
AIs that can implement the following principles effectively _should_ macro well in RTS games. Note this is for 'standard' gameplay and does not take into account all-in type strategies.

  1. Scout the opponent to find the following types of information:
    * Starting location
    * Bases (Expansions + Original Base).
      * Quantity - how many bases does the opponent have?
      * Location - where is each base?
      * Time of Construction - approximately when was each base constructed?
      * Worker saturation - use to approximate Mineral & Gas Gather Rates
    * Production buildings:
      * Type - what type of production buildings?
      * Quantity - how many production buildings of each type?
      * Time of Construction - approximately when was each production building constructed?
      * Location - where are the production buildings located?
    * Tech buildings:
      * Type - what tech path?
      * Quantity - how many tech buildings?
      * Time of Construction - approximately when was each tech building constructed?
      * Location - where are the tech buildings located?
    * Army unit composition:
      * Type - what types of units are in the opponent's army?
      * Quantity - how many of each unit type?
    * Army position and movement:
      * Where is the enemy's main army?
      * Is it moving? If so, why? (Attack | Defend another base | Map control)
  1. From the scouting information you should determine and continuously re-evaluate the following about your strategy:
    * Desired unit composition to counter the enemy's army
    * Desired production buildings composition needed to produce the desired unit composition
  1. Use all workers to their "maximal effectiveness"
    * Generally, this means keep workers gathering resources unless their are scouting or constructing a building
  1. Always use all production buildings that produce units in your desired army composition. Never let production buildings be idle.
  1. AIs should anticipate, predict, and plan to avoid these four blocking situations:
    1. Supply block: A situation where the limiting factor to your macro is insufficient supply.
      * Solution: Build another supply depot/pylon/overlord.
      * This blocking situation is mostly avoidable with proper build ordering (unless the enemy kills of your scouting overlord :P).
    1. Production block: A situation where the limiting factor to your macro is insufficient production buildings (i.e. building only 1 factory when you can afford to continuously produce out of 2 factories).
      * Solution: Build another production building.
      * This blocking situation is unavoidable in the short term because scouting information might drastically change your desired army composition.
      * In the long term, you can build sufficient production buildings to not be production blocked.
    1. Tech block: A situation where the limiting factory to your macro is insufficient tech buildings (i.e. not having a cybernetics core when you want to just make dragoons).
      * Solution: Build required tech buildings.
      * This blocking situation is unavoidable in the short term because scouting information might drastically change your desired army composition.
      * In the long term, you can build sufficient tech buildings to not be tech blocked.
    1. Resource block: A situation where the limiting factor to your macro is insufficient resources (i.e. building 3 factories when you can only afford to produce out of 2 factories).
      * Solution: Increase resource gather rates.
      * This blocking situation is unavoidable in both the short term and long term because you have a limited number of resources.
  1. If you are not being blocked by any of the four situations, and producing your desired unit composition, and your desired unit composition is optimal against the enemy's unit composition, you are macroing perfectly (impossible in practice).
  1. Construct the minimum number of desired production buildings, tech buildings, and supply depots to avoid the first three blocking situations. (i.e. don't produce 10 barracks if you just need three barracks to avoid being production blocked with your current income rate).
  1. Produce workers until they are able to fully saturate the number of bases you plan to have in X minutes. Maybe X is about 5?
  1. Never queue up more than 1 unit in any production building. Unless a unit is about to finish.
  1. Expand whenever it is "safe" to do so.
    * You are attacking, or
    * The opponent is retreating, or
    * The opponent is expanding, or
    * You have a larger army

## Principles of Middle Mechanics ##

Middle = between Micro and Macro. High level army decisions based on unit compositions and positioning

How to decide which base to attack, when to attack or retreat, what choke points to control, etc.

  * Based on relative army sizes, positioning


  * Retreat if you have a smaller army
  * Expand if you have a larger army
  * Attack if the opponent does not have a large enough army to defend all expansions
  * Harass if you do not have a large enough army to defend all expansions

## Principles of Micro Mechanics ##

How to control army units to their optimal effectiveness.

  * Target fire the unit with the highest damage rate/hit points.
  * Always attack something, move during weapon cooldown
  * Pull back units that are damaged.
  * Tell high level army manager to retreat if the enemy is inflicting more damage.